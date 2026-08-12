package beacon

import (
	"net"
	"testing"
	"time"
)

func TestTrackAndClearPending(t *testing.T) {
	s := &Session{}
	s.trackPending(1, []byte("frame1"))
	s.trackPending(2, []byte("frame2"))
	s.trackPending(3, []byte("frame3"))

	if len(s.pending) != 3 {
		t.Fatalf("got %d pending, want 3", len(s.pending))
	}

	s.clearPending([]uint16{2})

	if len(s.pending) != 2 {
		t.Fatalf("got %d pending after ack, want 2", len(s.pending))
	}
	for _, p := range s.pending {
		if p.seq == 2 {
			t.Errorf("seq 2 should have been cleared but is still pending")
		}
	}
}

// TestTrackPendingDoesNotEvictRecoveredDurableEntries guards against a bug
// the old RAM-only slot cap would reintroduce: Dial appends recovered
// durable records to s.pending *before* calling trackPending for the login
// beacon (see Dial), so trackPending must never evict from the front of
// s.pending - doing so would silently drop one of those durable entries'
// in-memory tracking instead of bounding anything login-related.
func TestTrackPendingDoesNotEvictRecoveredDurableEntries(t *testing.T) {
	s := &Session{}
	for i := 0; i < 40; i++ { // comfortably more than the old 32-slot cap
		s.pending = append(s.pending, &pendingBeacon{id: uint64(i), durable: true})
	}

	s.trackPending(loginSeqID, []byte("login-frame"))

	if len(s.pending) != 41 {
		t.Fatalf("got %d pending after trackPending, want 41 (40 durable + login, none evicted)", len(s.pending))
	}
	if s.pending[0].id != 0 || !s.pending[0].durable {
		t.Errorf("oldest durable entry (id 0) should not have been evicted, got %+v", s.pending[0])
	}
	last := s.pending[len(s.pending)-1]
	if last.seq != loginSeqID || string(last.frame) != "login-frame" {
		t.Errorf("login entry should be last, got %+v", last)
	}
}

func TestRetryStaleResendsOnlyTimedOutFrames(t *testing.T) {
	serverSide, clientSide := net.Pipe()
	defer serverSide.Close()
	defer clientSide.Close()

	s := &Session{conn: clientSide}
	s.pending = []*pendingBeacon{
		{seq: 1, frame: []byte("stale-frame"), sentAt: time.Now().Add(-2 * retryTimeout)},
		{seq: 2, frame: []byte("fresh-frame"), sentAt: time.Now()},
	}

	received := make(chan []byte, 1)
	go func() {
		buf := make([]byte, 64)
		n, _ := serverSide.Read(buf)
		received <- buf[:n]
	}()

	s.retryStale()

	select {
	case got := <-received:
		if string(got) != "stale-frame" {
			t.Errorf("resent frame = %q, want %q (only the stale one should be resent)", got, "stale-frame")
		}
	case <-time.After(2 * time.Second):
		t.Fatal("timed out waiting for the stale frame to be resent")
	}

	s.mu.Lock()
	defer s.mu.Unlock()
	if s.pending[0].sentAt.Before(time.Now().Add(-time.Second)) {
		t.Errorf("sentAt for the retried frame was not refreshed")
	}
}

// TestApplicationBeaconSurvivesSimulatedReconnect exercises the full
// durability contract this feature is for: SendBeacon must persist before
// transmitting, an unacked beacon must still be on disk after the Session
// that sent it is gone (simulating a crash/reboot/dropped connection), a
// freshly recovered Session must be able to resend it, and only an ack
// under the new session's own seq numbering erases it from disk.
func TestApplicationBeaconSurvivesSimulatedReconnect(t *testing.T) {
	dir := t.TempDir()
	store, err := OpenPendingStore(dir)
	if err != nil {
		t.Fatalf("OpenPendingStore: %v", err)
	}

	serverSide1, clientSide1 := net.Pipe()
	defer serverSide1.Close()
	defer clientSide1.Close()

	firstSend := make(chan []byte, 1)
	go func() {
		buf := make([]byte, 4096)
		n, _ := serverSide1.Read(buf)
		firstSend <- buf[:n]
	}()

	s1 := &Session{conn: clientSide1, store: store}
	if err := s1.SendBeacon([]byte("device-info-payload")); err != nil {
		t.Fatalf("SendBeacon: %v", err)
	}
	select {
	case <-firstSend:
	case <-time.After(2 * time.Second):
		t.Fatal("timed out waiting for the initial send")
	}

	// s1 (and its in-RAM pending list) is now abandoned without an ack ever
	// arriving - simulating a crash, a reboot, or the connection dropping.
	// The durable record must still be there.
	records, err := store.LoadAll()
	if err != nil {
		t.Fatalf("LoadAll: %v", err)
	}
	if len(records) != 1 || string(records[0].Payload) != "device-info-payload" {
		t.Fatalf("got %+v, want the unacked beacon still durably queued", records)
	}

	// A brand new Session (fresh seq numbering, same durable store) recovers
	// it exactly like Dial does on every reconnect.
	serverSide2, clientSide2 := net.Pipe()
	defer serverSide2.Close()
	defer clientSide2.Close()

	s2 := &Session{conn: clientSide2, store: store}
	for _, rec := range records {
		s2.pending = append(s2.pending, &pendingBeacon{id: rec.ID, inner: rec.Payload, durable: true})
	}

	resend := make(chan []byte, 1)
	go func() {
		buf := make([]byte, 4096)
		n, _ := serverSide2.Read(buf)
		resend <- buf[:n]
	}()

	s2.retryStale() // recovered record's zero-value sentAt is always "overdue"

	select {
	case <-resend:
	case <-time.After(2 * time.Second):
		t.Fatal("timed out waiting for the recovered beacon to be resent")
	}

	s2.mu.Lock()
	newSeq := s2.pending[0].seq
	s2.mu.Unlock()
	if newSeq == 0 {
		t.Fatal("recovered beacon was never actually transmitted (seq still 0)")
	}

	// Only now - acked under the new session's own seq - is it erased.
	s2.clearPending([]uint16{newSeq})
	remaining, err := store.LoadAll()
	if err != nil {
		t.Fatalf("LoadAll after ack: %v", err)
	}
	if len(remaining) != 0 {
		t.Fatalf("got %+v, want the durable record erased once acked", remaining)
	}
}
