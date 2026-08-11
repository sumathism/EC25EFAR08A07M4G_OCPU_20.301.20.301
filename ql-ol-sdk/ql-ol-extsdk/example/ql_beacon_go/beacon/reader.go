package beacon

import "io"

// FrameReader incrementally reads complete, CRC-valid LA5 frames off a byte
// stream (TCP has no message boundaries of its own - this is what restores
// them).
type FrameReader struct {
	r   io.Reader
	buf []byte
}

// NewFrameReader wraps r (typically the Session's net.Conn) for frame-at-a-time reads.
func NewFrameReader(r io.Reader) *FrameReader {
	return &FrameReader{r: r}
}

// ReadFrame blocks until one complete frame is available and returns it.
// Garbage bytes (bad magic/CRC/malformed module list) are silently skipped
// one byte at a time so a single corrupt frame can't wedge the reader.
func (fr *FrameReader) ReadFrame() (ParsedFrame, error) {
	readBuf := make([]byte, 4096)
	for {
		frame, consumed, ok := tryParseFrame(fr.buf)
		if ok {
			fr.buf = fr.buf[consumed:]
			return frame, nil
		}
		if consumed > 0 {
			fr.buf = fr.buf[consumed:]
			continue
		}

		n, err := fr.r.Read(readBuf)
		if n > 0 {
			fr.buf = append(fr.buf, readBuf[:n]...)
			continue
		}
		if err != nil {
			return ParsedFrame{}, err
		}
	}
}
