#global namespace compress

struct CompressStream
{
    stream:      const [..] u8
    lenStream:   uint
    curByte:     uint
    code_buffer: u32
    num_bits:    s8
    eof:         bool
}

impl CompressStream
{
    func init(using self, buf: const [..] u8)
    {
        @init(self)
        stream = buf
        lenStream = @countof(buf)
    }

    func refill_lsb(using self, width = 24'u8)
    {
        if num_bits > width
            return
		if code_buffer == 0 and num_bits == -1
            num_bits = 0

        if code_buffer >= 1 << num_bits
        {
			num_bits = -100
            return
        }

        if curByte >= lenStream
        {
            num_bits = -42
            eof = true
            return
        }

        c := stream[curByte]
        curByte += 1

        code_buffer |= c << num_bits
		num_bits += 8
    }

    func peek_bits_lsb(using self, width: u8) -> u32
    {
	    if num_bits < width refill_lsb()
    	return code_buffer & ~(0xffffffff <<,safe width)
    }

    func read_bits_lsb(using self, width: u8)->u32
    {
        k := peek_bits_lsb(width)
        consume_bits_lsb(width)
        return k
    }

    func consume_bits_lsb(using self, width: u8)
    {
	    code_buffer >>=,safe width
	    num_bits -= width
    }
}
