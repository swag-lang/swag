#global public
#global namespace Compress

// A structure for reading a bit stream, useful for decompressors or parsers
struct BitStream
{
    stream:         const [..] u8     // Input byte stream
    curByte:        u64               // Index of current byte being processed
    codeBuffer:     u32               // Bit buffer holding bits to be consumed
    numBits:        u8                // Number of bits currently in the buffer
    eof:            bool              // Flag to signal end-of-stream
}

impl BitStream
{
    // Initializes the bit stream with an input buffer
    mtd init(buf: const [..] u8)
    {
        @init(me, 1)
        .stream = buf
    }

    // Returns a pointer to the beginning of the stream
    #[Swag.Inline]
    mtd startPtr() => @dataof(.stream)

    // Returns a pointer to the current byte in the stream
    #[Swag.Inline]
    mtd curPtr() => @dataof(.stream) + .curByte

    // Returns the total size of the stream
    #[Swag.Inline]
    mtd size() => @countof(.stream)

    // Refills the bit buffer until at least 'width' bits are available
    mtd refill(width = 24'u8)
    {
        while .numBits <= width
        {
            // Detect and reset broken state (if buffer was empty and numBits was max)
            if .codeBuffer == 0 and .numBits == cast #bit (u8) -1 do
                .numBits = 0

            // If buffer is already full, prevent overflow
            if .codeBuffer >= 1'u32 << .numBits
            {
                .numBits = 255
                return
            }

            // Stop if we reached end of stream
            if .curByte >= @countof(.stream)
            {
                .eof = true
                return
            }

            // Read next byte and shift it into the buffer
            let c = .stream[.curByte]
            .curByte += 1
            .codeBuffer |= cast(u32) c << .numBits
            .numBits += 8
        }
    }

    // Returns next 'width' bits without consuming them (refills if needed)
    #[Swag.Inline]
    mtd peekBits(width: u8)->u32
    {
        if .numBits < width do
            .refill()
        return .codeBuffer & ~(0xffffffff << width)
    }

    // Like peekBits, but assumes enough bits are already available
    mtd peekBitsNoRefill(width: u8)->u32
    {
        Debug.assert(.numBits >= width)
        return .codeBuffer & ~(0xffffffff << width)
    }

    // Reads and consumes 'width' bits from the buffer
    #[Swag.Inline]
    mtd readBits(width: u8)->u32
    {
        let k = .peekBits(width)
        .consumeBits(width)
        return k
    }

    // Reads and consumes 'width' bits without checking buffer availability
    mtd readBitsNoRefill(width: u8)->u32
    {
        let k = .peekBitsNoRefill(width)
        .consumeBits(width)
        return k
    }

    // Consumes 'width' bits from the buffer (advances the bit position)
    #[Swag.Inline]
    mtd consumeBits(width: u8)
    {
        .codeBuffer >>= width
        .numBits -= #wrap width
    }

    // Discards bits to align to the next byte boundary
    #[Swag.Inline]
    mtd discardToNextByte()
    {
        .consumeBits(cast(u8) .numBits & 7)
    }

    // Reverts stream to last byte-aligned state (undo partial read)
    mtd revert()
    {
        Debug.assert(!(.numBits & 7)) // Must be aligned
        .curByte -= .numBits / 8
        .numBits    = 0
        .codeBuffer = 0
    }

    // Peeks 'width' bytes into 'dest', consuming bits after each byte
    mtd peekBytes(width: u8, dest: [*] u8)
    {
        // Could be optimized further
        for i in width
        {
            dest[i] = cast(u8) .peekBits(8)
            .consumeBits(8)
        }
    }

    // Peeks 4 bytes (little-endian) into 'dest'
    mtd peek32(dest: [*] u8)
    {
        dest[0] = cast(u8) .peekBits(8)
        .consumeBits(8)
        dest[1] = cast(u8) .peekBits(8)
        .consumeBits(8)
        dest[2] = cast(u8) .peekBits(8)
        .consumeBits(8)
        dest[3] = cast(u8) .peekBits(8)
        .consumeBits(8)
    }

    // Peeks 4 bytes (big-endian) into 'dest', auto-switches based on platform
    mtd peek32Be(dest: [*] u8)
    {
        #if BIG_ENDIAN do
            peek32(dest) // Already big-endian
        #else
        {
            dest[3] = cast(u8) .peekBits(8)
            .consumeBits(8)
            dest[2] = cast(u8) .peekBits(8)
            .consumeBits(8)
            dest[1] = cast(u8) .peekBits(8)
            .consumeBits(8)
            dest[0] = cast(u8) .peekBits(8)
            .consumeBits(8)
        }
    }
}
