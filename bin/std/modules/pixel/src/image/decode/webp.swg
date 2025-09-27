#global namespace Webp
using Swag, Core, Core.Errors

// Step 1: WebP Container and RIFF parsing
// This handles the outer WebP format before we get to VP8 data

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
        // Need at least RIFF header (12 bytes)
        if me.streamLen < 12:
            throw InvalidFormatError{"WebP: File too small"}

        // Check RIFF signature
        if !me.isTag(0, 'R', 'I', 'F', 'F'):
            throw InvalidFormatError{"WebP: Not a RIFF file"}

        // Check WEBP signature
        if !me.isTag(8, 'W', 'E', 'B', 'P'):
            throw InvalidFormatError{"WebP: Not a WebP file"}

        let riffSize = cast(s32) me.dU32(4)

        // Validate RIFF size
        if riffSize <= 0 or riffSize > MaxSize:
            throw InvalidFormatError{"WebP: Invalid RIFF size"}

        // RIFF size should be file size - 8
        let expectedFileSize = riffSize + 8
        if expectedFileSize > me.streamLen:
            throw InvalidFormatError{"WebP: RIFF size exceeds file size"}

        Console.printf("RIFF container valid, size: % bytes\n", riffSize)
    }

    // Find and parse chunks in the WebP file
    mtd parseChunks()->Array'ChunkInfo throw
    {
        var chunks: Array'ChunkInfo
        var offset  = 12                // Skip RIFF header

        while (offset + 8) <= me.streamLen
        {
            let tag  = me.dU32(offset)
            let size = cast(s32) me.dU32(offset + 4)

            // Validate chunk size
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

            // Move to next chunk (chunks are padded to even boundaries)
            offset += 8 + ((size + 1) & ~1)
        }

        return chunks
    }

    // Find specific chunk by tag
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

    // Initialize decoder with WebP data
    mtd init(buffer: const [..] u8) throw
    {
        me.stream    = @dataof(buffer)
        me.streamLen = cast(s32) @countof(buffer)

        Console.printf("Initializing WebP decoder with % bytes\n", me.streamLen)

        // Parse RIFF container
        me.parseRiff()

        // Parse all chunks
        let chunks = me.parseChunks()
        if @countof(chunks) == 0:
            throw InvalidFormatError{"WebP: No chunks found"}

        // Look for VP8 chunk (simple WebP)
        let vp8Tag   = makeTag('V', 'P', '8', ' ')
        let vp8Chunk = findChunk(chunks, vp8Tag)

        if vp8Chunk.offset >= 0
        {
            Console.printf("Found VP8 chunk, parsing...\n")
            me.parseVP8Header(vp8Chunk.offset, vp8Chunk.size)
            return
        }

        // Look for VP8L chunk (lossless WebP)
        let vp8lTag   = makeTag('V', 'P', '8', 'L')
        let vp8lChunk = findChunk(chunks, vp8lTag)

        if vp8lChunk.offset >= 0
        {
            throw UnsupportedFormatError{"WebP: VP8L (lossless) not yet supported"}
        }

        // Look for VP8X chunk (extended WebP)
        let vp8xTag   = makeTag('V', 'P', '8', 'X')
        let vp8xChunk = findChunk(chunks, vp8xTag)

        if vp8xChunk.offset >= 0
        {
            throw UnsupportedFormatError{"WebP: VP8X (extended) not yet supported"}
        }

        throw InvalidFormatError{"WebP: No supported chunk found"}
    }

    // Parse VP8 header to get image dimensions
    mtd parseVP8Header(offset: s32, size: s32) throw
    {
        if size < 10:
            throw InvalidFormatError{"VP8: Chunk too small"}

        // Read VP8 frame header
        let frameTag  = me.dU8(offset) | (me.dU8(offset + 1) << 8) | (me.dU8(offset + 2) << 16)
        let keyFrame  = (frameTag & 1) == 0
        let version   = (frameTag >> 1) & 7
        let showFrame = (frameTag >> 4) & 1

        if !keyFrame:
            throw UnsupportedFormatError{"VP8: Only keyframes supported currently"}

        Console.printf("VP8 frame: keyframe=%, version=%, show=%\n", keyFrame, version, showFrame)

        // Check keyframe start code
        if !(me.dU8(offset + 3) == 0x9D and
            me.dU8(offset + 4) == 0x01 and
            me.dU8(offset + 5) == 0x2A):
            throw InvalidFormatError{"VP8: Invalid keyframe start code"}

        // Read dimensions
        let width  = cast(s32) (me.dU16(offset + 6) & 0x3FFF)
        let height = cast(s32) (me.dU16(offset + 8) & 0x3FFF)

        if width <= 0 or height <= 0 or width > MaxDim or height > MaxDim:
            throw InvalidFormatError{"VP8: Invalid dimensions"}

        me.imageSizeX = width
        me.imageSizeY = height

        Console.printf("VP8 dimensions: %x%\n", width, height)
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
