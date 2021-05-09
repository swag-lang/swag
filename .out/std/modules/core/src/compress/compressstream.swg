#global namespace compress

struct CompressStream
{
    stream:         const [..] u8
    curByte:        uint
    codeBuffer:     u32
    numBits:        s8
    eof:            bool
}

impl CompressStream
{
    func init(using self, buf: const [..] u8)
    {
        @init(self)
        stream = buf
    }

    func curPtr(using self) => @dataof(stream) + curByte

    func refill(using self, width = 24'u8)
    {
        while numBits < width
        {
            if codeBuffer == 0 and numBits == -1
                numBits = 0

            if codeBuffer >= 1'u32 << numBits
            {
                numBits = -100
                return
            }

            if curByte >= @countof(stream)
            {
                numBits = -42
                eof = true
                return
            }

            c := stream[curByte]
            curByte += 1

            codeBuffer |= cast(u32) c << numBits
            numBits += 8
        }
    }

    func peekBits(using self, width: u8) -> u32
    {
	    if numBits < width refill()
    	return codeBuffer & ~(0xffffffff <<,safe width)
    }

    func readBits(using self, width: u8)->u32
    {
        k := peekBits(width)
        consumeBits(width)
        return k
    }

    func consumeBits(using self, width: u8)
    {
	    codeBuffer >>=,safe width
	    numBits -= width
    }

    func discardToNextByte(using self)
    {
        consumeBits(cast(u8) numBits & 7)
    }

    func revert(using self)
    {
        debug.assert(!(numBits & 7))
        curByte -= numBits / 8
        numBits = 0
        codeBuffer = 0
    }
}
