#global namespace Webp
using Swag, Core, Core.Errors

// Step 3: VP8 Frame Header Parsing
// Parse VP8 control information: color space, quantization, loop filter, etc.

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
        range:        u32
        value:        u32
        bitCount:     s32
        buffer:       const [*] u8
        pos:          s32
        end:          s32
    }

    // VP8 Frame Header Information
    struct VP8FrameHeader
    {
        // Basic frame info
        width:      s32
        height:     s32

        // Color space and pixel format
        colorSpace:       u32     // 0 = YUV, 1 = reserved
        clampingType:     u32     // 0 = [0,255], 1 = [16,235]

        // Segmentation
        useSegment:           bool
        updateSegmentMap:     bool
        segmentProbs:         [3] u32

        // Loop filter
        filterType:         u32     // 0 = normal, 1 = simple
        filterLevel:        u32     // 0-63
        sharpnessLevel:     u32     // 0-7

        // Quantization
        baseQIndex:     u32     // Base quantizer 0-127
        yDCDelta:       s32     // Y DC quantizer delta
        y2DCDelta:      s32     // Y2 DC quantizer delta
        y2ACDelta:      s32     // Y2 AC quantizer delta
        uvDCDelta:      s32     // UV DC quantizer delta
        uvACDelta:      s32     // UV AC quantizer delta

        // Token partitions
        numPartitions: s32 // Number of DCT token partitions
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

        if size >= 2
        {
            bd.value = (cast(u32) data[0] << 8) | cast(u32) data[1]
            bd.pos   = 2
        }
        elif size >= 1
        {
            bd.value = cast(u32) data[0] << 8
            bd.pos   = 1
        }
    }

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

    func normalize(bd: *BoolDecoder)
    {
        while bd.range < 128
        {
            bd.range <<= 1
            bd.value <<= 1
            bd.bitCount += 1

            if bd.bitCount == 8
            {
                shiftBytes(bd)
            }
        }
    }

    func readBool(bd: *BoolDecoder, probability: u32)->u32
    {
        let prob   = Math.min(probability, 255)
        let split  = 1 + (((bd.range - 1) * prob) >> 8)
        let bigBit = (bd.value >> 8) >= split

        if bigBit
        {
            bd.range -= split
            bd.value -= cast(u32) split << 8
        }
        else
        {
            bd.range = split
        }

        normalize(bd)
        return bigBit ? 1 : 0
    }

    func readBit(bd: *BoolDecoder)->u32
    {
        return readBool(bd, 128)
    }

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

    // Read signed integer (delta values)
    func readSignedDelta(bd: *BoolDecoder)->s32
    {
        if readBit(bd) == 0:
            return 0

        let magnitude = cast(s32) readBits(bd, 4)     // 4-bit magnitude
        let sign      = readBit(bd)

        return sign != 0 ? -magnitude : magnitude
    }

    // Parse VP8 frame header from boolean decoder
    func parseVP8FrameHeader(bd: *BoolDecoder, width: s32, height: s32)->VP8FrameHeader
    {
        var header: VP8FrameHeader
        header.width  = width
        header.height = height

        Console.printf("Parsing VP8 frame header...\n")

        // Color space and clamping
        header.colorSpace   = readBit(bd)
        header.clampingType = readBit(bd)

        Console.printf("Color space: %, clamping: %\n", header.colorSpace, header.clampingType)

        // Segmentation
        header.useSegment = readBit(bd) != 0
        if header.useSegment
        {
            header.updateSegmentMap = readBit(bd) != 0

            if readBit(bd) != 0 // update_segment_feature_data
            {
                // Skip segment feature data for now
                discard readBit(bd) // segment_feature_mode

                // Skip quantizer and loop filter deltas
                for i in 4 // segments
                {
                    for j in 2 // features (quantizer, loop_filter)
                    {
                        if readBit(bd) != 0 // feature enabled
                        {
                            discard readBits(bd, 7) // feature data
                            discard readBit(bd) // feature sign
                        }
                    }
                }
            }

            if header.updateSegmentMap
            {
                // Read segment tree probabilities
                for i in 3
                {
                    header.segmentProbs[i] = readBit(bd) != 0 ? readBits(bd, 8) : 255
                }
            }
        }

        Console.printf("Segmentation: use=%, update_map=%\n", header.useSegment, header.updateSegmentMap)

        // Loop filter
        header.filterType     = readBit(bd)
        header.filterLevel    = readBits(bd, 6)
        header.sharpnessLevel = readBits(bd, 3)

        Console.printf("Filter: type=%, level=%, sharpness=%\n", header.filterType, header.filterLevel, header.sharpnessLevel)

        // Loop filter adjustments
        if readBit(bd) != 0 // loop_filter_adj_enable
        {
            if readBit(bd) != 0 // mode_ref_lf_delta_update
            {
                // Skip reference frame deltas
                for i in 4 // ref frames
                {
                    if readBit(bd) != 0:
                        discard readSignedDelta(bd)
                }

                // Skip mode deltas
                for i in 4 // prediction modes
                {
                    if readBit(bd) != 0:
                        discard readSignedDelta(bd)
                }
            }
        }

        // Number of DCT token partitions
        header.numPartitions = 1 << readBits(bd, 2) // 1, 2, 4, or 8 partitions
        Console.printf("Token partitions: %\n", header.numPartitions)

        // Quantization parameters
        header.baseQIndex = readBits(bd, 7) // Base quantizer index (0-127)

        header.yDCDelta  = readSignedDelta(bd)
        header.y2DCDelta = readSignedDelta(bd)
        header.y2ACDelta = readSignedDelta(bd)
        header.uvDCDelta = readSignedDelta(bd)
        header.uvACDelta = readSignedDelta(bd)

        Console.printf("Quantization: base=%, deltas=[%,%,%,%,%]\n", header.baseQIndex, header.yDCDelta, header.y2DCDelta, header.y2ACDelta, header.uvDCDelta, header.uvACDelta)

        return header
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
        var offset  = 12

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
            me.parseVP8Complete(vp8Chunk.offset, vp8Chunk.size)
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

    // Parse complete VP8 frame (header + frame data)
    mtd parseVP8Complete(offset: s32, size: s32) throw
    {
        if size < 10:
            throw InvalidFormatError{"VP8: Chunk too small"}

        // Parse VP8 frame tag
        let frameTag:          u32 = me.dU8(offset) | (me.dU8(offset + 1) << 8) | (me.dU8(offset + 2) << 16)
        let keyFrame           = (frameTag & 1) == 0
        let version            = (frameTag >> 1) & 7
        let showFrame          = (frameTag >> 4) & 1
        let firstPartitionSize = (frameTag >> 5) | (cast(u32) me.dU8(offset + 1) << 3) | (cast(u32) me.dU8(offset + 2) << 11)

        if !keyFrame:
            throw UnsupportedFormatError{"VP8: Only keyframes supported currently"}

        Console.printf("VP8 frame: keyframe=%, version=%, show=%, first_partition=%\n", keyFrame, version, showFrame, firstPartitionSize)

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

        // Parse frame header using boolean decoder
        let partitionStart = offset + 10
        let partitionSize  = cast(s32) Math.min(firstPartitionSize, cast(u32) (size - 10))

        if partitionSize <= 0:
            throw InvalidFormatError{"VP8: Invalid first partition size"}

        var bd: BoolDecoder
        initBoolDecoder(&bd, me.stream + partitionStart, partitionSize)

        let frameHeader = parseVP8FrameHeader(&bd, width, height)

        Console.printf("VP8 frame header parsed successfully!\n")
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
