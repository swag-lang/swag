#global namespace Webp
using Swag, Core, Core.Errors

// Step 4: DCT Coefficient Decoding - FIXED VERSION
// Parse VP8 coefficient tokens and reconstruct DCT blocks

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

    struct BoolDecoder
    {
        range:      u32
        value:      u32
        bitCount:   s32
        buffer:     const [*] u8
        pos:        s32
        end:        s32
    }

    struct VP8FrameHeader
    {
        width:              s32
        height:             s32
        colorSpace:         u32
        clampingType:       u32
        useSegment:         bool
        updateSegmentMap:   bool
        segmentProbs:       [3] u32
        filterType:         u32
        filterLevel:        u32
        sharpnessLevel:     u32
        baseQIndex:         u32
        yDCDelta:          s32
        y2DCDelta:         s32
        y2ACDelta:         s32
        uvDCDelta:         s32
        uvACDelta:         s32
        numPartitions:      s32
    }

    // VP8 coefficient token probabilities (default values from spec)
    const DefaultCoeffProbs = [
        // Y plane [band][context][token]
        [
            [[255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255],
             [255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255],
             [255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255]],
            [[176, 246, 255, 255, 255, 255, 255, 255, 255, 255, 255],
             [223, 241, 252, 255, 255, 255, 255, 255, 255, 255, 255],
             [249, 253, 253, 255, 255, 255, 255, 255, 255, 255, 255]],
            [[255, 244, 252, 255, 255, 255, 255, 255, 255, 255, 255],
             [234, 254, 254, 255, 255, 255, 255, 255, 255, 255, 255],
             [253, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255]],
            [[255, 246, 254, 255, 255, 255, 255, 255, 255, 255, 255],
             [239, 253, 254, 255, 255, 255, 255, 255, 255, 255, 255],
             [254, 255, 254, 255, 255, 255, 255, 255, 255, 255, 255]],
            [[255, 248, 254, 255, 255, 255, 255, 255, 255, 255, 255],
             [251, 255, 254, 255, 255, 255, 255, 255, 255, 255, 255],
             [255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255]],
            [[255, 253, 254, 255, 255, 255, 255, 255, 255, 255, 255],
             [251, 254, 254, 255, 255, 255, 255, 255, 255, 255, 255],
             [254, 255, 254, 255, 255, 255, 255, 255, 255, 255, 255]],
            [[255, 254, 253, 255, 255, 255, 255, 255, 255, 255, 255],
             [250, 255, 254, 255, 255, 255, 255, 255, 255, 255, 255],
             [254, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255]],
            [[255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255],
             [255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255],
             [255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255]]],
        // UV plane [band][context][token]
        [
            [[255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255],
             [255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255],
             [255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255]],
            [[181, 239, 251, 255, 255, 255, 255, 255, 255, 255, 255],
             [209, 219, 229, 255, 255, 255, 255, 255, 255, 255, 255],
             [255, 248, 253, 255, 255, 255, 255, 255, 255, 255, 255]],
            [[255, 236, 253, 255, 255, 255, 255, 255, 255, 255, 255],
             [236, 253, 254, 255, 255, 255, 255, 255, 255, 255, 255],
             [251, 253, 253, 255, 255, 255, 255, 255, 255, 255, 255]],
            [[255, 240, 253, 255, 255, 255, 255, 255, 255, 255, 255],
             [243, 254, 254, 255, 255, 255, 255, 255, 255, 255, 255],
             [254, 255, 254, 255, 255, 255, 255, 255, 255, 255, 255]],
            [[255, 244, 254, 255, 255, 255, 255, 255, 255, 255, 255],
             [250, 255, 254, 255, 255, 255, 255, 255, 255, 255, 255],
             [255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255]],
            [[255, 251, 254, 255, 255, 255, 255, 255, 255, 255, 255],
             [248, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255],
             [255, 255, 254, 255, 255, 255, 255, 255, 255, 255, 255]],
            [[255, 253, 255, 255, 255, 255, 255, 255, 255, 255, 255],
             [252, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255],
             [255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255]],
            [[255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255],
             [255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255],
             [255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255]]]]

    // VP8 zigzag scan order for 4x4 DCT
    const ZigZag4x4: [16] u8 = [0, 1, 4, 8, 5, 2, 3, 6, 9, 12, 13, 10, 7, 11, 14, 15]

    // Band mapping for coefficient positions
    const CoeffBands: [16] u8 = [0, 1, 2, 3, 6, 4, 5, 6, 6, 6, 6, 6, 6, 6, 6, 7]

    // DCT coefficient tokens
    const DCT_EOB_TOKEN         = 0   // End of block
    const DCT_0_TOKEN           = 1   // Zero run
    const DCT_1_TOKEN           = 2   // +/-1
    const DCT_2_TOKEN           = 3   // +/-2
    const DCT_3_TOKEN           = 4   // +/-3
    const DCT_4_TOKEN           = 5   // +/-4
    const DCT_CAT1_TOKEN        = 6   // 5-6
    const DCT_CAT2_TOKEN        = 7   // 7-10
    const DCT_CAT3_TOKEN        = 8   // 11-18
    const DCT_CAT4_TOKEN        = 9   // 19-34
    const DCT_CAT5_TOKEN        = 10  // 35-66
    const DCT_CAT6_TOKEN        = 11  // 67-2048

    // Token partition structure
    struct TokenPartitions
    {
        decoders:   [8] BoolDecoder
        count:      s32
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
            bd.pos = 2
        }
        elif size >= 1
        {
            bd.value = cast(u32) data[0] << 8
            bd.pos = 1
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
            bd.range  <<= 1
            bd.value  <<= 1
            bd.bitCount += 1

            if bd.bitCount == 8
            {
                shiftBytes(bd)
            }
        }
    }

    func readBool(bd: *BoolDecoder, probability: u32)->u32
    {
        let prob = Math.min(probability, 255)
        let split = 1 + (((bd.range - 1) * prob) >> 8)
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

    func readSignedDelta(bd: *BoolDecoder)->s32
    {
        if readBit(bd) == 0:
            return 0
        let magnitude = cast(s32) readBits(bd, 4)
        let sign = readBit(bd)
        return sign != 0 ? -magnitude : magnitude
    }

    // Decode single DCT coefficient token
    func decodeCoeffToken(bd: *BoolDecoder, plane: s32, band: s32, ctx: s32)->{value: s32, hasMore: bool}
    {
        let planeIdx = Math.min(plane, 1)  // Y=0, UV=1
        let bandIdx = Math.min(band, 7)
        let ctxIdx = Math.min(ctx, 2)

        // Read token using coefficient probabilities
        var token = 0
        while token < 11
        {
            if readBool(bd, DefaultCoeffProbs[planeIdx][bandIdx][ctxIdx][token]) == 0:
                break
            token += 1
        }

        // Handle different token types
        switch token
        {
        case DCT_EOB_TOKEN:
            return {0, false}  // End of block

        case DCT_0_TOKEN:
            return {0, true}   // Zero coefficient

        case DCT_1_TOKEN:
            let sign = readBit(bd)
            return {sign != 0 ? -1 : 1, true}

        case DCT_2_TOKEN:
            let sign = readBit(bd)
            return {sign != 0 ? -2 : 2, true}

        case DCT_3_TOKEN:
            let sign = readBit(bd)
            return {sign != 0 ? -3 : 3, true}

        case DCT_4_TOKEN:
            let sign = readBit(bd)
            return {sign != 0 ? -4 : 4, true}

        case DCT_CAT1_TOKEN:  // 5-6
            let extra = readBit(bd)
            let base = 5 + cast(s32) extra
            let sign = readBit(bd)
            return {sign != 0 ? -base : base, true}

        case DCT_CAT2_TOKEN:  // 7-10
            let extra = readBits(bd, 2)
            let base = 7 + cast(s32) extra
            let sign = readBit(bd)
            return {sign != 0 ? -base : base, true}

        case DCT_CAT3_TOKEN:  // 11-18
            let extra = readBits(bd, 3)
            let base = 11 + cast(s32) extra
            let sign = readBit(bd)
            return {sign != 0 ? -base : base, true}

        case DCT_CAT4_TOKEN:  // 19-34
            let extra = readBits(bd, 4)
            let base = 19 + cast(s32) extra
            let sign = readBit(bd)
            return {sign != 0 ? -base : base, true}

        case DCT_CAT5_TOKEN:  // 35-66
            let extra = readBits(bd, 5)
            let base = 35 + cast(s32) extra
            let sign = readBit(bd)
            return {sign != 0 ? -base : base, true}

        case DCT_CAT6_TOKEN:  // 67-2048
            let extra = readBits(bd, 11)
            let base = 67 + cast(s32) extra
            let sign = readBit(bd)
            return {sign != 0 ? -base : base, true}
        }

        return {0, false}
    }

    // Decode 4x4 DCT block coefficients
    func decode4x4Block(bd: *BoolDecoder, coeffs: [*] s16, plane: s32)->s32  // returns number of non-zero coeffs
    {
        Memory.set(coeffs, 0, #sizeof(s16) * 16)

        var nonZeroCount = 0
        var ctx = 0

        for pos in 16
        {
            let band = cast(s32) CoeffBands[pos]
            let (value, hasMore) = decodeCoeffToken(bd, plane, band, ctx)

            if !hasMore:
                break  // EOB token

            if value != 0
            {
                coeffs[ZigZag4x4[pos]] = cast(s16) value
                nonZeroCount += 1
                ctx = 1  // Non-zero context for next coefficient
            }
            else
            {
                ctx = 0  // Zero context
            }
        }

        return nonZeroCount
    }

    // Initialize token partitions - PROPER CONSUMPTION TRACKING
    func initTokenPartitions(tp: *TokenPartitions, data: const [*] u8, offset: s32, size: s32, numPartitions: s32, bd: *BoolDecoder)
    {
        tp.count = Math.min(numPartitions, 8)

        if tp.count == 1
        {
            // Single partition - remaining data from boolean decoder
            let remainingSize = bd.end - bd.pos
            Console.printf("Single partition mode: remaining % bytes from frame header parsing\n", remainingSize)

            if remainingSize > 0
            {
                initBoolDecoder(&tp.decoders[0], bd.buffer + bd.pos, remainingSize)
                Console.printf("Initialized 1 token partition, size: %\n", remainingSize)
            }
            else
            {
                Console.printf("Warning: no remaining data for single partition\n")
                initBoolDecoder(&tp.decoders[0], bd.buffer, 0)
            }
            return
        }

        Console.printf("Multi-partition mode: consumed % bytes for frame header\n", bd.pos)

        // Debug: show bytes around the expected partition header location
        Console.printf("Debugging partition header area:\n")
        let debugStart = Math.max(0, bd.pos - 10)
        let debugEnd = Math.min(bd.end, bd.pos + 30)
        Console.printf("Bytes from offset % to %: ", debugStart, debugEnd - 1)
        for i in debugStart until debugEnd
        {
            Console.printf("%, ", bd.buffer[i])
        }
        Console.printf("\n")
        Console.printf("Boolean decoder state: pos=%, end=%, range=%, value=%\n", bd.pos, bd.end, bd.range, bd.value)

        // Calculate where token partition headers start
        let consumedBytes = bd.pos
        let tokenHeaderStart = offset + consumedBytes
        let availableForTokens = size - consumedBytes

        Console.printf("Token partition headers start at offset %, available bytes: %\n", tokenHeaderStart, availableForTokens)

        // Multiple partitions - read partition sizes from remaining first partition
        var partitionSizes: [8] s32
        let totalHeaderSize = (tp.count - 1) * 3  // 3 bytes per partition size

        if totalHeaderSize > availableForTokens
        {
            Console.printf("Warning: partition header too large (need %, have %), falling back to single partition\n",
                          totalHeaderSize, availableForTokens)
            tp.count = 1
            let remainingSize = bd.end - bd.pos
            if remainingSize > 0
            {
                initBoolDecoder(&tp.decoders[0], bd.buffer + bd.pos, remainingSize)
            }
            else
            {
                initBoolDecoder(&tp.decoders[0], bd.buffer, 0)
            }
            return
        }

        Console.printf("Reading % partition sizes from offset %\n", tp.count - 1, tokenHeaderStart)

        // Read partition sizes (24-bit little endian) from the correct location
        var dataOffset = tokenHeaderStart
        var totalPartitionSize = 0

        for i in (tp.count - 1)
        {
            let b0 = cast(s32) data[dataOffset + 0]
            let b1 = cast(s32) data[dataOffset + 1]
            let b2 = cast(s32) data[dataOffset + 2]
            partitionSizes[i] = b0 | (b1 << 8) | (b2 << 16)

            Console.printf("  Partition % header: bytes=[%,%,%] -> size=%\n", i, b0, b1, b2, partitionSizes[i])

            // Validate partition size against remaining data
            if partitionSizes[i] < 0 or partitionSizes[i] > availableForTokens
            {
                Console.printf("Error: invalid partition % size: % (max %)\n", i, partitionSizes[i], availableForTokens)
                tp.count = 1
                let remainingSize = bd.end - bd.pos
                if remainingSize > 0
                {
                    initBoolDecoder(&tp.decoders[0], bd.buffer + bd.pos, remainingSize)
                }
                else
                {
                    initBoolDecoder(&tp.decoders[0], bd.buffer, 0)
                }
                return
            }

            totalPartitionSize += partitionSizes[i]
            dataOffset += 3
        }

        // Last partition gets remaining data after all headers
        let remainingForLastPartition = availableForTokens - totalHeaderSize - totalPartitionSize
        partitionSizes[tp.count - 1] = remainingForLastPartition

        Console.printf("Calculated last partition size: %\n", remainingForLastPartition)
        Console.printf("Total partition sizes: %, available after headers: %\n",
                      totalPartitionSize + remainingForLastPartition, availableForTokens - totalHeaderSize)

        // Validate total size
        if totalPartitionSize + remainingForLastPartition > availableForTokens - totalHeaderSize or remainingForLastPartition < 0
        {
            Console.printf("Error: total partition sizes exceed available data or negative last partition\n")
            tp.count = 1
            let remainingSize = bd.end - bd.pos
            if remainingSize > 0
            {
                initBoolDecoder(&tp.decoders[0], bd.buffer + bd.pos, remainingSize)
            }
            else
            {
                initBoolDecoder(&tp.decoders[0], bd.buffer, 0)
            }
            return
        }

        Console.printf("Token partitions: %\n", tp.count)

        // Initialize decoders starting after partition headers
        var currentOffset = tokenHeaderStart + totalHeaderSize
        for i in tp.count
        {
            if partitionSizes[i] <= 0
            {
                Console.printf("Error: partition % has invalid size %\n", i, partitionSizes[i])
                tp.count = i
                break
            }

            initBoolDecoder(&tp.decoders[i], data + currentOffset, partitionSizes[i])
            Console.printf("  Partition %: offset=%, size=%\n", i, currentOffset, partitionSizes[i])
            currentOffset += partitionSizes[i]
        }
    }

    // Get token partition decoder for macroblock
    func getTokenDecoder(tp: *TokenPartitions, mbRow: s32)->*BoolDecoder
    {
        if tp.count == 1:
            return &tp.decoders[0]

        let partition = mbRow % tp.count
        return &tp.decoders[partition]
    }

    // Test coefficient decoding
    func testCoefficientDecoding(tp: *TokenPartitions)
    {
        Console.printf("Testing coefficient decoding...\n")

        // Test a few blocks from different partitions
        for p in Math.min(tp.count, 3)
        {
            Console.printf("Testing partition %:\n", p)
            let bd = &tp.decoders[p]

            var coeffs: [16] s16

            // Test Y block
            let nonZeroY = decode4x4Block(bd, &coeffs[0], 0)
            Console.printf("  Y block: % non-zero coeffs\n", nonZeroY)
            if nonZeroY > 0
            {
                Console.printf("    First few coeffs: [%, %, %, %]\n",
                              coeffs[0], coeffs[1], coeffs[2], coeffs[3])
            }

            // Test UV block
            let nonZeroUV = decode4x4Block(bd, &coeffs[0], 1)
            Console.printf("  UV block: % non-zero coeffs\n", nonZeroUV)
            if nonZeroUV > 0
            {
                Console.printf("    First few coeffs: [%, %, %, %]\n",
                              coeffs[0], coeffs[1], coeffs[2], coeffs[3])
            }
        }
    }

    func parseVP8FrameHeader(bd: *BoolDecoder, width: s32, height: s32)->VP8FrameHeader
    {
        var header: VP8FrameHeader
        header.width = width
        header.height = height

        Console.printf("Parsing VP8 frame header...\n")

        header.colorSpace = readBit(bd)
        header.clampingType = readBit(bd)
        Console.printf("Color space: %, clamping: %\n", header.colorSpace, header.clampingType)

        header.useSegment = readBit(bd) != 0
        if header.useSegment
        {
            header.updateSegmentMap = readBit(bd) != 0

            if readBit(bd) != 0  // update_segment_feature_data
            {
                discard readBit(bd)  // segment_feature_mode
                for i in 4  // segments
                {
                    for j in 2  // features
                    {
                        if readBit(bd) != 0  // feature enabled
                        {
                            discard readBits(bd, 7)  // feature data
                            discard readBit(bd)      // feature sign
                        }
                    }
                }
            }

            if header.updateSegmentMap
            {
                for i in 3
                {
                    header.segmentProbs[i] = readBit(bd) != 0 ? readBits(bd, 8) : 255
                }
            }
        }

        Console.printf("Segmentation: use=%, update_map=%\n", header.useSegment, header.updateSegmentMap)

        header.filterType = readBit(bd)
        header.filterLevel = readBits(bd, 6)
        header.sharpnessLevel = readBits(bd, 3)
        Console.printf("Filter: type=%, level=%, sharpness=%\n",
                      header.filterType, header.filterLevel, header.sharpnessLevel)

        if readBit(bd) != 0  // loop_filter_adj_enable
        {
            if readBit(bd) != 0  // mode_ref_lf_delta_update
            {
                for i in 4
                {
                    if readBit(bd) != 0:
                        discard readSignedDelta(bd)
                }
                for i in 4
                {
                    if readBit(bd) != 0:
                        discard readSignedDelta(bd)
                }
            }
        }

        header.numPartitions = 1 << readBits(bd, 2)
        Console.printf("Token partitions: %\n", header.numPartitions)

        header.baseQIndex = readBits(bd, 7)
        header.yDCDelta  = readSignedDelta(bd)
        header.y2DCDelta = readSignedDelta(bd)
        header.y2ACDelta = readSignedDelta(bd)
        header.uvDCDelta = readSignedDelta(bd)
        header.uvACDelta = readSignedDelta(bd)

        Console.printf("Quantization: base=%, deltas=[%,%,%,%,%]\n",
                      header.baseQIndex,
                      header.yDCDelta, header.y2DCDelta, header.y2ACDelta,
                      header.uvDCDelta, header.uvACDelta)

        return header
    }
}

impl Decoder
{
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

    // Parse complete VP8 frame (header + coefficients)
    mtd parseVP8Complete(offset: s32, size: s32) throw
    {
        if size < 10:
            throw InvalidFormatError{"VP8: Chunk too small"}

        // Parse VP8 frame tag
        let frameTag: u32  = me.dU8(offset) | (me.dU8(offset + 1) << 8) | (me.dU8(offset + 2) << 16)
        let keyFrame  = (frameTag & 1) == 0
        let version   = (frameTag >> 1) & 7
        let showFrame = (frameTag >> 4) & 1
        let firstPartitionSize = (frameTag >> 5) | (cast(u32) me.dU8(offset + 1) << 3) | (cast(u32) me.dU8(offset + 2) << 11)

        if !keyFrame:
            throw UnsupportedFormatError{"VP8: Only keyframes supported currently"}

        Console.printf("VP8 frame: keyframe=%, version=%, show=%, first_partition=%\n",
                      keyFrame, version, showFrame, firstPartitionSize)

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
        let partitionSize = cast(s32) Math.min(firstPartitionSize, cast(u32) (size - 10))

        if partitionSize <= 0:
            throw InvalidFormatError{"VP8: Invalid first partition size"}

        var bd: BoolDecoder
        initBoolDecoder(&bd, me.stream + partitionStart, partitionSize)

        let frameHeader = parseVP8FrameHeader(&bd, width, height)

        Console.printf("VP8 frame header parsed successfully!\n")

        // Initialize token partitions for coefficient data
        let tokenStart = partitionStart + partitionSize
        let tokenSize = size - 10 - partitionSize

        if tokenSize > 0
        {
            var tokenPartitions: TokenPartitions
            initTokenPartitions(&tokenPartitions, me.stream, partitionStart, partitionSize, frameHeader.numPartitions, &bd)

            // Test coefficient decoding
            testCoefficientDecoding(&tokenPartitions)
        }
        else
        {
            Console.printf("No token partition data found\n")
        }
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