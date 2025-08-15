#global marked
#global public
#global namespace Compress

// A structure for reading a bit self.stream, useful for decompressors or parsers
struct BitStream
{
    stream:         const [..] u8     // Input byte self.stream
    curByte:        u64               // Index of current byte being processed
    codeBuffer:     u32               // Bit buffer holding bits to be consumed
    numBits:        u8                // Number of bits currently in the buffer
    eof:            bool              // Flag to signal end-of-stream
}

impl BitStream
{
    // Initializes the bit self.stream with an input buffer
    mtd init(buf: const [..] u8)
    {
        @init(self, 1)
        self.stream = buf
    }

    // Returns a pointer to the beginning of the self.stream
    #[Swag.Inline]
    mtd startPtr() => @dataof(self.stream)

    // Returns a pointer to the current byte in the self.stream
    #[Swag.Inline]
    mtd curPtr() => @dataof(self.stream) + self.curByte

    // Returns the total size of the self.stream
    #[Swag.Inline]
    mtd size() => @countof(self.stream)

    // Refills the bit buffer until at least 'width' bits are available
    mtd refill(width = 24'u8)
    {
        while self.numBits <= width
        {
            // Detect and reset broken state (if buffer was empty and self.numBits was max)
            if self.codeBuffer == 0 and self.numBits == cast #bit (u8) -1:
                self.numBits = 0

            // If buffer is already full, prevent overflow
            if self.codeBuffer >= 1'u32 << self.numBits
            {
                self.numBits = 255
                return
            }

            // Stop if we reached end of self.stream
            if self.curByte >= @countof(self.stream)
            {
                self.eof = true
                return
            }

            // Read next byte and shift it into the buffer
            let c = self.stream[self.curByte]
            self.curByte += 1
            self.codeBuffer |= cast(u32) c << self.numBits
            self.numBits += 8
        }
    }

    // Returns next 'width' bits without consuming them (refills if needed)
    #[Swag.Inline]
    mtd peekBits(width: u8)->u32
    {
        if self.numBits < width:
            self.refill()
        return self.codeBuffer & ~(0xffffffff << width)
    }

    // Like peekBits, but assumes enough bits are already available
    mtd peekBitsNoRefill(width: u8)->u32
    {
        Debug.assert(self.numBits >= width)
        return self.codeBuffer & ~(0xffffffff << width)
    }

    // Reads and consumes 'width' bits from the buffer
    #[Swag.Inline]
    mtd readBits(width: u8)->u32
    {
        let k = self.peekBits(width)
        self.consumeBits(width)
        return k
    }

    // Reads and consumes 'width' bits without checking buffer availability
    mtd readBitsNoRefill(width: u8)->u32
    {
        let k = self.peekBitsNoRefill(width)
        self.consumeBits(width)
        return k
    }

    // Consumes 'width' bits from the buffer (advances the bit position)
    #[Swag.Inline]
    mtd consumeBits(width: u8)
    {
        self.codeBuffer >>= width
        self.numBits -= #wrap width
    }

    // Discards bits to align to the next byte boundary
    #[Swag.Inline]
    mtd discardToNextByte()
    {
        self.consumeBits(cast(u8) self.numBits & 7)
    }

    // Reverts self.stream to last byte-aligned state (undo partial read)
    mtd revert()
    {
        Debug.assert(!(self.numBits & 7)) // Must be aligned
        self.curByte -= self.numBits / 8
        self.numBits    = 0
        self.codeBuffer = 0
    }

    // Peeks 'width' bytes into 'dest', consuming bits after each byte
    mtd peekBytes(width: u8, dest: ^u8)
    {
        // Could be optimized further
        for i in width
        {
            dest[i] = cast(u8) self.peekBits(8)
            self.consumeBits(8)
        }
    }

    // Peeks 4 bytes (little-endian) into 'dest'
    mtd peek32(dest: ^u8)
    {
        dest[0] = cast(u8) self.peekBits(8)
        self.consumeBits(8)
        dest[1] = cast(u8) self.peekBits(8)
        self.consumeBits(8)
        dest[2] = cast(u8) self.peekBits(8)
        self.consumeBits(8)
        dest[3] = cast(u8) self.peekBits(8)
        self.consumeBits(8)
    }

    // Peeks 4 bytes (big-endian) into 'dest', auto-switches based on platform
    mtd peek32Be(dest: ^u8)
    {
        #if BIG_ENDIAN:
            peek32(dest) // Already big-endian
        #else
        {
            dest[3] = cast(u8) self.peekBits(8)
            self.consumeBits(8)
            dest[2] = cast(u8) self.peekBits(8)
            self.consumeBits(8)
            dest[1] = cast(u8) self.peekBits(8)
            self.consumeBits(8)
            dest[0] = cast(u8) self.peekBits(8)
            self.consumeBits(8)
        }
    }
}
