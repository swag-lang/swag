#global public
#global namespace Compress

struct BitStream
{
    stream:         const [..] u8
    curByte:        u64
    codeBuffer:     u32
    numBits:        u8
    eof:            bool
}

impl BitStream
{
    mtd init(buf: const [..] u8)
    {
        @init(self, 1)
        stream = buf
    }

    #[Swag.Inline]
    mtd startPtr() => @dataof(stream)

    #[Swag.Inline]
    mtd curPtr() => @dataof(stream) + curByte

    #[Swag.Inline]
    mtd size() => @countof(stream)

    mtd refill(width = 24'u8)
    {
        while numBits <= width
        {
            if codeBuffer == 0 and numBits == cast<bit>(u8) -1:
                numBits = 0

            if codeBuffer >= 1'u32 << numBits
            {
                numBits = 255
                return
            }

            if curByte >= @countof(stream)
            {
                eof = true
                return
            }

            let c = stream[curByte]
            curByte += 1

            codeBuffer |= cast(u32) c << numBits
            numBits += 8
        }
    }

    #[Swag.Inline]
    mtd peekBits(width: u8)->u32
    {
        if numBits < width:
            refill()
        return codeBuffer & ~(0xffffffff << width)
    }

    mtd peekBitsNoRefill(width: u8)->u32
    {
        Debug.assert(numBits >= width)
        return codeBuffer & ~(0xffffffff << width)
    }

    #[Swag.Inline]
    mtd readBits(width: u8)->u32
    {
        let k = peekBits(width)
        consumeBits(width)
        return k
    }

    mtd readBitsNoRefill(width: u8)->u32
    {
        let k = peekBitsNoRefill(width)
        consumeBits(width)
        return k
    }

    #[Swag.Inline]
    mtd consumeBits(width: u8)
    {
        codeBuffer >>= width
        numBits -= #over width
    }

    #[Swag.Inline]
    mtd discardToNextByte()
    {
        consumeBits(cast(u8) numBits & 7)
    }

    mtd revert()
    {
        Debug.assert(!(numBits & 7))
        curByte -= numBits / 8
        numBits    = 0
        codeBuffer = 0
    }

    mtd peekBytes(width: u8, dest: ^u8)
    {
        // To optim
        for i in width
        {
            dest[i] = cast(u8) peekBits(8)
            consumeBits(8)
        }
    }

    mtd peek32(dest: ^u8)
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

    mtd peek32Be(dest: ^u8)
    {
        #if BIG_ENDIAN:
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
