package beacon

import (
	"log"
	"time"
)

// retryTimeout matches the firmware's LAFM_CONFIG_APP_COMM_TIMEOUT_NOACK_RETRY (10s).
const (
	retryTimeout    = 10 * time.Second
	retryCheckEvery = 1 * time.Second
)

// pendingBeacon is one beacon sent but not yet acked. Exactly one of two
// shapes applies:
//
//   - Login handshake entries set frame (and a fixed seq, loginSeqID): the
//     outer frame is pre-built and replayed byte-for-byte, since it's
//     regenerated fresh on every Dial anyway and carries no user data.
//   - Application beacon entries set inner/id/durable instead: inner is
//     the payload durably persisted in PendingStore under id (see
//     SendBeacon), and seq/the encrypted outer frame are (re)built fresh on
//     every send attempt by transmit - see transmit for why the outer frame
//     itself can't just be replayed after a reconnect.
type pendingBeacon struct {
	seq    uint16
	sentAt time.Time

	frame []byte // login only

	id      uint64 // application beacons only: PendingStore record id
	inner   []byte // application beacons only: payload backing id
	durable bool
}

// trackPending records a just-sent login frame so it gets retried if never
// acked. Application beacons use SendBeacon instead, which durably queues
// them before attempting to send. This is called at most once per Dial (for
// the login handshake only), so unlike the old RAM-only design this
// replaced, it needs no slot cap of its own - and must not have one, since
// s.pending may already hold recovered durable records by the time this
// runs (see Dial), and evicting from the front would silently drop one of
// those instead.
func (s *Session) trackPending(seq uint16, frame []byte) {
	s.mu.Lock()
	defer s.mu.Unlock()
	s.pending = append(s.pending, &pendingBeacon{seq: seq, frame: frame, sentAt: time.Now()})
}

// clearPending drops any pending entries the server has now acked.
func (s *Session) clearPending(ackedSeqs []uint16) {
	if len(ackedSeqs) == 0 {
		return
	}
	acked := make(map[uint16]bool, len(ackedSeqs))
	for _, seq := range ackedSeqs {
		acked[seq] = true
	}

	type deletion struct {
		id  uint64
		seq uint16
	}

	s.mu.Lock()
	var toDelete []deletion
	remaining := s.pending[:0]
	for _, p := range s.pending {
		if !acked[p.seq] {
			remaining = append(remaining, p)
			continue
		}
		if p.durable {
			toDelete = append(toDelete, deletion{id: p.id, seq: p.seq})
		}
	}
	s.pending = remaining
	s.mu.Unlock()

	// Pop from the durable queue outside the lock, after acked entries are
	// already gone from s.pending - the literal "erase from flash only
	// after ack" step. A delete failure isn't fatal: worst case the record
	// gets resent once more on a future reconnect and re-acked/re-deleted
	// then, a harmless duplicate rather than lost data.
	for _, d := range toDelete {
		if err := s.store.Delete(d.id); err != nil {
			log.Printf("beacon: removing acked durable record %d (seq %d) from flash: %v", d.id, d.seq, err)
			continue
		}
		log.Printf("beacon: removed record %d (seq %d, acked) from durable store", d.id, d.seq)
	}
}

// retryLoop resends any pending beacon that's been waiting longer than
// retryTimeout without an ack, until the session is closed.
func (s *Session) retryLoop() {
	defer close(s.retries)
	ticker := time.NewTicker(retryCheckEvery)
	defer ticker.Stop()
	for {
		select {
		case <-s.closed:
			return
		case <-ticker.C:
			s.retryStale()
		}
	}
}

// retryStale resends anything that's been waiting longer than retryTimeout
// without an ack. This is also how a durable beacon recovered from disk on
// Dial (sentAt left at its zero value, which is always "overdue") gets its
// first real send attempt - no separate resume codepath is needed.
func (s *Session) retryStale() {
	s.mu.Lock()
	var stale []*pendingBeacon
	now := time.Now()
	for _, p := range s.pending {
		if now.Sub(p.sentAt) >= retryTimeout {
			stale = append(stale, p)
			p.sentAt = now
		}
	}
	s.mu.Unlock()

	// Resend outside the lock - a slow/blocked socket write shouldn't stall
	// anything else touching the pending list (e.g. an incoming ack).
	for _, p := range stale {
		if p.frame != nil {
			// Login handshake: same bytes, same fixed seq, every time.
			if _, err := s.conn.Write(p.frame); err != nil {
				continue // a failed retry here just gets retried again next tick
			}
		} else {
			// Application beacon: re-wrap with a fresh seq + nonce - see
			// transmit for why the previous outer frame isn't safe to
			// replay verbatim once it may have crossed a reconnect.
			if err := s.transmit(p); err != nil {
				continue
			}
		}
		select {
		case s.retries <- p.seq:
		default: // don't block if nobody's draining Retries()
		}
	}
}
