#global namespace Webp
using Swag, Core, Core.Errors

// Step 2: Add VP8 Boolean Decoder (Arithmetic Decoder)
// This handles reading bits from the compressed VP8 bitstream

public struct Decoder
{
    stream:         const [*] u8
    streamLen:      s32
    imageSizeX:     s32
    imageSizeY:     s32
}

private
{
    const MaxSize = 1_000_000_000
    const MaxDim  = 16384

    struct ChunkInfo
    {
        tag:        u32
        offset:     s32
        size:       s32
    }

    // VP8 Boolean (Arithmetic) Decoder
    struct BoolDecoder
    {
        range:      u32         // Current range
        value:      u32         // Current value
        bitCount:   s32         // Bits consumed from current byte
        buffer:     const [*] u8 // Input buffer
        pos:        s32         // Current position in buffer
        end:        s32         // End of buffer
    }

    // Initialize boolean decoder
    func initBoolDecoder(bd: *BoolDecoder, data: const [*] u8, size: s32)
    {
        bd.buffer   = data
        bd.pos      = 0
        bd.end      = size
        bd.range    = 255
        bd.bitCount = 0
        bd.value    = 0

        // Load initial bytes into value
        if size >= 2
        {
            bd.value = (cast(u32) data[0] << 8) | cast(u32) data[1]
            bd.pos = 2
        }
        elif size >= 1
        {
            bd.value = cast(u32) data[0] << 8
            bd.pos = 1
        }
    }

    // Read next byte and shift it into value
    func shiftBytes(bd: *BoolDecoder)
    {
        if bd.pos < bd.end
        {
            bd.value = (bd.value << 8) | cast(u32) bd.buffer[bd.pos]
            bd.pos += 1
        }
        else
        {
            bd.value <<= 8
        }
        bd.bitCount = 0
    }

    // Normalize the decoder state
    func normalize(bd: *BoolDecoder)
    {
        while bd.range < 128
        {
            bd.range  <<= 1
            bd.value  <<= 1
            bd.bitCount += 1

            if bd.bitCount == 8
            {
                shiftBytes(bd)
            }
        }
    }

    // Read a boolean value with given probability (0-255)
    func readBool(bd: *BoolDecoder, probability: u32)->u32
    {
        // Clamp probability to valid range
        let prob = Math.min(probability, 255)

        // Calculate split point
        let split = 1 + (((bd.range - 1) * prob) >> 8)

        // Check if value is in upper or lower partition
        let bigBit = (bd.value >> 8) >= split

        if bigBit
        {
            // Upper partition - bit is 1
            bd.range -= split
            bd.value -= cast(u32) split << 8
        }
        else
        {
            // Lower partition - bit is 0
            bd.range = split
        }

        // Renormalize
        normalize(bd)

        return bigBit ? 1 : 0
    }

    // Read a bit with 50/50 probability
    func readBit(bd: *BoolDecoder)->u32
    {
        return readBool(bd, 128)
    }

    // Read multiple bits as an integer (LSB first)
    func readBits(bd: *BoolDecoder, numBits: s32)->u32
    {
        var result: u32 = 0

        for i in numBits
        {
            let bit = readBit(bd)
            result |= bit << i
        }

        return result
    }

    // Read signed integer with given number of bits
    func readSignedBits(bd: *BoolDecoder, numBits: s32)->s32
    {
        let value = cast(s32) readBits(bd, numBits)
        let sign = readBit(bd)
        return sign != 0 ? -value : value
    }

    // Test the boolean decoder with some basic operations
    func testBooleanDecoder(bd: *BoolDecoder)
    {
        Console.printf("Testing boolean decoder...\n")

        // Read a few bits to test
        for i in 8
        {
            let bit = readBit(bd)
            Console.printf("Bit %: %\n", i, bit)
        }

        // Read a few boolean values with different probabilities
        for i in 4
        {
            let prob = 64 + (i * 48) // 64, 112, 160, 208
            let bit = readBool(bd, cast(u32) prob)
            Console.printf("Bool % (prob %): %\n", i, prob, bit)
        }

        // Read some multi-bit values
        for i in 3
        {
            let numBits = 2 + i // 2, 3, 4 bits
            let value = readBits(bd, numBits)
            Console.printf("% bits: %\n", numBits, value)
        }
    }
}

impl Decoder
{
    // Safe stream reading helpers
    mtd dU8(at: s32)->u8
    {
        if at < 0 or at >= me.streamLen:
            return 0
        return me.stream[at]
    }

    mtd dU16(at: s32)->u16
    {
        return cast(u16) me.dU8(at) | (cast(u16) me.dU8(at + 1) << 8)
    }

    mtd dU32(at: s32)->u32
    {
        return cast(u32) me.dU8(at) |
               (cast(u32) me.dU8(at + 1) << 8) |
               (cast(u32) me.dU8(at + 2) << 16) |
               (cast(u32) me.dU8(at + 3) << 24)
    }

    mtd isTag(at: s32, a: u8, b: u8, c: u8, d: u8)->bool
    {
        return me.dU8(at + 0) == a and
               me.dU8(at + 1) == b and
               me.dU8(at + 2) == c and
               me.dU8(at + 3) == d
    }

    func makeTag(a: u8, b: u8, c: u8, d: u8)->u32
    {
        return cast(u32) a |
               (cast(u32) b << 8) |
               (cast(u32) c << 16) |
               (cast(u32) d << 24)
    }

    // Parse RIFF container
    mtd parseRiff() throw
    {
        if me.streamLen < 12:
            throw InvalidFormatError{"WebP: File too small"}

        if !me.isTag(0, 'R', 'I', 'F', 'F'):
            throw InvalidFormatError{"WebP: Not a RIFF file"}

        if !me.isTag(8, 'W', 'E', 'B', 'P'):
            throw InvalidFormatError{"WebP: Not a WebP file"}

        let riffSize = cast(s32) me.dU32(4)

        if riffSize <= 0 or riffSize > MaxSize:
            throw InvalidFormatError{"WebP: Invalid RIFF size"}

        let expectedFileSize = riffSize + 8
        if expectedFileSize > me.streamLen:
            throw InvalidFormatError{"WebP: RIFF size exceeds file size"}

        Console.printf("RIFF container valid, size: % bytes\n", riffSize)
    }

    mtd parseChunks()->Array'ChunkInfo throw
    {
        var chunks: Array'ChunkInfo
        var offset = 12

        while (offset + 8) <= me.streamLen
        {
            let tag  = me.dU32(offset)
            let size = cast(s32) me.dU32(offset + 4)

            if size < 0 or offset + 8 + size > me.streamLen
            {
                Console.printf("Warning: Invalid chunk size at offset %\n", offset)
                break
            }

            var chunk: ChunkInfo
            chunk.tag    = tag
            chunk.offset = offset + 8
            chunk.size   = size

            chunks.add(chunk)

            Console.printf("Found chunk at offset %, size % bytes\n", chunk.offset, chunk.size)
            offset += 8 + ((size + 1) & ~1)
        }

        return chunks
    }

    func findChunk(chunks: const Array'ChunkInfo, tag: u32)->ChunkInfo
    {
        foreach chunk in chunks
        {
            if chunk.tag == tag:
                return chunk
        }

        var empty: ChunkInfo
        empty.offset = -1
        return empty
    }

    mtd init(buffer: const [..] u8) throw
    {
        me.stream    = @dataof(buffer)
        me.streamLen = cast(s32) @countof(buffer)

        Console.printf("Initializing WebP decoder with % bytes\n", me.streamLen)

        me.parseRiff()

        let chunks = me.parseChunks()
        if @countof(chunks) == 0:
            throw InvalidFormatError{"WebP: No chunks found"}

        let vp8Tag   = makeTag('V', 'P', '8', ' ')
        let vp8Chunk = findChunk(chunks, vp8Tag)

        if vp8Chunk.offset >= 0
        {
            Console.printf("Found VP8 chunk, parsing...\n")
            me.parseVP8Header(vp8Chunk.offset, vp8Chunk.size)
            me.testVP8BooleanDecoder(vp8Chunk.offset, vp8Chunk.size)
            return
        }

        let vp8lTag   = makeTag('V', 'P', '8', 'L')
        let vp8lChunk = findChunk(chunks, vp8lTag)

        if vp8lChunk.offset >= 0:
            throw UnsupportedFormatError{"WebP: VP8L (lossless) not yet supported"}

        let vp8xTag   = makeTag('V', 'P', '8', 'X')
        let vp8xChunk = findChunk(chunks, vp8xTag)

        if vp8xChunk.offset >= 0:
            throw UnsupportedFormatError{"WebP: VP8X (extended) not yet supported"}

        throw InvalidFormatError{"WebP: No supported chunk found"}
    }

    mtd parseVP8Header(offset: s32, size: s32) throw
    {
        if size < 10:
            throw InvalidFormatError{"VP8: Chunk too small"}

        let frameTag  = me.dU8(offset) | (me.dU8(offset + 1) << 8) | (me.dU8(offset + 2) << 16)
        let keyFrame  = (frameTag & 1) == 0
        let version   = (frameTag >> 1) & 7
        let showFrame = (frameTag >> 4) & 1

        if !keyFrame:
            throw UnsupportedFormatError{"VP8: Only keyframes supported currently"}

        Console.printf("VP8 frame: keyframe=%, version=%, show=%\n", keyFrame, version, showFrame)

        if !(me.dU8(offset + 3) == 0x9D and
            me.dU8(offset + 4) == 0x01 and
            me.dU8(offset + 5) == 0x2A):
            throw InvalidFormatError{"VP8: Invalid keyframe start code"}

        let width  = cast(s32) (me.dU16(offset + 6) & 0x3FFF)
        let height = cast(s32) (me.dU16(offset + 8) & 0x3FFF)

        if width <= 0 or height <= 0 or width > MaxDim or height > MaxDim:
            throw InvalidFormatError{"VP8: Invalid dimensions"}

        me.imageSizeX = width
        me.imageSizeY = height

        Console.printf("VP8 dimensions: %x%\n", width, height)
    }

    // Test the VP8 boolean decoder on actual VP8 data
    mtd testVP8BooleanDecoder(offset: s32, size: s32) throw
    {
        if size < 10:
            return

        Console.printf("Testing VP8 boolean decoder...\n")

        // Skip VP8 header and start from bitstream data
        let frameTag: u32 = me.dU8(offset) | (me.dU8(offset + 1) << 8) | (me.dU8(offset + 2) << 16)
        let firstPartitionSize = (frameTag >> 5) | (cast(u32) me.dU8(offset + 1) << 3) | (cast(u32) me.dU8(offset + 2) << 11)

        Console.printf("First partition size: % bytes\n", firstPartitionSize)

        // Initialize boolean decoder with first partition data
        let partitionStart = offset + 10 // Skip 10-byte VP8 header
        let partitionSize = cast(s32) Math.min(firstPartitionSize, cast(u32) (size - 10))

        if partitionSize <= 0:
            return

        var bd: BoolDecoder
        initBoolDecoder(&bd, me.stream + partitionStart, partitionSize)

        // Test reading some values from the actual VP8 stream
        Console.printf("Reading from VP8 bitstream:\n")
        testBooleanDecoder(&bd)
    }
}

public impl IImageDecoder for Decoder
{
    mtd impl canDecode(fileName: string)->bool
    {
        let ext = Path.getExtensionLowerCase(fileName)
        return ext == ".webp"
    }

    mtd impl decode(buffer: const [..] u8, options: DecodeOptions)->Image throw
    {
        .init(buffer)

        var result: Image
        result.init(.imageSizeX, .imageSizeY, PixelFormat.BGR8)

        if !options.decodePixels:
            return result

        result.allocPixels()
        return result
    }
}