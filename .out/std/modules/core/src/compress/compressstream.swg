#global namespace compress

public struct CompressStream
{
    stream:         const [..] u8
    curByte:        uint
    codeBuffer:     u32
    numBits:        u8
    eof:            bool
}

public impl CompressStream
{
    func init(using self, buf: const [..] u8)
    {
        @init(self)
        stream = buf
    }

    func curPtr(using self) => @dataof(stream) + curByte

    func refill(using self, width = 24'u8)
    {
        while numBits <= width
        {
            if codeBuffer == 0 and numBits == bitcast(u8) -1
                numBits = 0

            if codeBuffer >= 1'u32 << numBits
            {
                numBits = bitcast(u8) -100
                return
            }

            if curByte >= @countof(stream)
            {
                eof = true
                return
            }

            c := stream[curByte]
            curByte += 1

            codeBuffer |= cast(u32) c << numBits
            numBits += 8
        }
    }

    #[swag.inline]
    func peekBits(using self, width: u8)->u32
    {
	    if numBits < width refill()
    	return codeBuffer & ~(0xffffffff <<,safe width)
    }

    func peekBitsNoRefill(using self, width: u8)->u32
    {
	    debug.assert(numBits >= width)
    	return codeBuffer & ~(0xffffffff <<,safe width)
    }

    #[swag.inline]
    func readBits(using self, width: u8)->u32
    {
        k := peekBits(width)
        consumeBits(width)
        return k
    }

    func readBitsNoRefill(using self, width: u8)->u32
    {
        k := peekBitsNoRefill(width)
        consumeBits(width)
        return k
    }

    #[swag.inline]
    func consumeBits(using self, width: u8)
    {
	    codeBuffer >>=,safe width
	    numBits -= width
    }

    #[swag.inline]
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

    func peekBytes(using self, width: u8, dest: *u8)
    {
        // To optim
        loop i: width
        {
            dest[i] = cast(u8) peekBits(8)
            consumeBits(8)
        }
    }

    func peek32(using self, dest: *u8)
    {
        dest[0] = cast(u8) peekBits(8)
        consumeBits(8)
        dest[1] = cast(u8) peekBits(8)
        consumeBits(8)
        dest[2] = cast(u8) peekBits(8)
        consumeBits(8)
        dest[3] = cast(u8) peekBits(8)
        consumeBits(8)
    }

    func peek32Be(using self, dest: *u8)
    {
        #if BIG_ENDIAN
            peek32(dest)
        #else
        {
            dest[3] = cast(u8) peekBits(8)
            consumeBits(8)
            dest[2] = cast(u8) peekBits(8)
            consumeBits(8)
            dest[1] = cast(u8) peekBits(8)
            consumeBits(8)
            dest[0] = cast(u8) peekBits(8)
            consumeBits(8)
        }
    }
}
