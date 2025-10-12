/*
    :LICENCE
    This code is based on the miniz library (https://github.com/richgel999/miniz)
    The original code has been modified for Swag
    See LICENCE.md for the corresponding licence.
*/

#global namespace Compress
#global #[Swag.CanOverflow(true)]
using Core.Errors

private
{
    const PackedCodeSizeSymsSwizzle: [?] u8 = [
        16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15]

    const Bitmasks: [17] u32 = [
        0x0000, 0x0001, 0x0003, 0x0007, 0x000F, 0x001F, 0x003F, 0x007F, 0x00FF, 0x01FF, 0x03FF, 0x07FF, 0x0FFF, 0x1FFF, 0x3FFF, 0x7FFF, 0xFFFF]

    const LenExtra: [256] u8 = [
        0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
        5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
        5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 0]

    const LenSym: [256] u16 = [
        257, 258, 259, 260, 261, 262, 263, 264, 265, 265, 266, 266, 267, 267, 268, 268, 269, 269, 269, 269, 270, 270, 270, 270, 271, 271, 271, 271, 272, 272, 272, 272,
        273, 273, 273, 273, 273, 273, 273, 273, 274, 274, 274, 274, 274, 274, 274, 274, 275, 275, 275, 275, 275, 275, 275, 275, 276, 276, 276, 276, 276, 276, 276, 276,
        277, 277, 277, 277, 277, 277, 277, 277, 277, 277, 277, 277, 277, 277, 277, 277, 278, 278, 278, 278, 278, 278, 278, 278, 278, 278, 278, 278, 278, 278, 278, 278,
        279, 279, 279, 279, 279, 279, 279, 279, 279, 279, 279, 279, 279, 279, 279, 279, 280, 280, 280, 280, 280, 280, 280, 280, 280, 280, 280, 280, 280, 280, 280, 280,
        281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281,
        282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282,
        283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283,
        284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 285]

    const SmallDistSym: [512] u8 = [
        0, 1, 2, 3, 4, 4, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 9, 9, 9, 9, 9, 9, 9, 9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11,
        11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 13,
        13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
        14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
        14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
        15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
        16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
        16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
        16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17,
        17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17,
        17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17,
        17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17]

    const SmallDistExtra: [512] u8 = [
        0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5,
        5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
        6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
        6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
        7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
        7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
        7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
        7, 7, 7, 7, 7, 7, 7, 7]

    const LargeDistExtra: [128] u8 = [
        0, 0, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
        12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
        13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13]

    const LargeDistSym: [128] u8 = [
        0, 0, 18, 19, 20, 20, 21, 21, 22, 22, 22, 22, 23, 23, 23, 23, 24, 24, 24, 24, 24, 24, 24, 24, 25, 25, 25, 25, 25, 25, 25, 25, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26,
        26, 26, 26, 26, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
        28, 28, 28, 28, 28, 28, 28, 28, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29]

    const NumProbes: [11] u32 = [
        0, 1, 6, 32, 16, 32, 128, 256, 512, 768, 1500]

    const MaxSupportedHuffCodeSize = 32
    const MaxHuffTables            = 3
    const MaxHuffSymbols0          = 288'u32
    const MaxHuffSymbols1          = 32'u32
    const MaxHuffSymbols2          = 19'u32
    const LzDictSize               = 32768
    const LzDictSizeMask           = LzDictSize - 1
    const MinMatchLen              = 3
    const MaxMatchLen              = 258

    const LzCodeBufSize = 24 * 1024
    const LzHashBits    = 12

    const OutBufSize         = (LzCodeBufSize * 13) / 10
    const MaxHuffSymbols     = 288
    const Level1HashSizeMask = 4095
    const LzHashShift        = (LzHashBits + 2) / 3
    const LzHashSize         = 1 << LzHashBits

    const GreedyParsing           = 0x04000
    const NonDeterministicParsing = 0x08000
    const RleMatches              = 0x10000
    const FilterMatches           = 0x20000
    const ForceAllStaticBlocks    = 0x40000
    const ForceAllRawBlocks       = 0x80000

    const HuffmanOnly      = 0
    const DefaultMaxProbes = 128
    const MaxProbesMask    = 0xFFF

    struct SymFreq
    {
        key:          u16
        symIndex:     u16
    }

    struct Compressor
    {
        compressFlags:        u32
        maxProbes:            [2] u32
        greedyParsing:        bool
        lookaheadPos:         u32
        lookaheadSize:        u32
        dictSize:             u32
        lzCodeBufAddr:        [*] u8
        lzFlags:              *u8
        outputBufAddr:        [*] u8
        outputBufEndAddr:     [*] u8
        numFlagsLeft:         u32
        totalLzBytes:         u32
        lzCodeBufDictPos:     u32
        bitsIn:               u32
        bitBuffer:            u32
        savedMatchDist:       u32
        savedMatchLen:        u32
        savedLit:             u32
        outputFlushOfs:       u32
        blockIndex:           u32
        inBuf:                const [*] u8
        inSrc:                const [*] u8
        srcBufLeft:           u64
        outBufOfs:            u64
        dict:                 [LzDictSize + MaxMatchLen - 1] u8
        huffCount:            [MaxHuffTables, MaxHuffSymbols] u16
        huffCodes:            [MaxHuffTables, MaxHuffSymbols] u16
        huffCodeSizes:        [MaxHuffTables, MaxHuffSymbols] u8
        lzCodeBuf:            [LzCodeBufSize] u8
        next:                 [LzDictSize] u16
        hash:                 [LzHashSize] u16
        outputBuf:            [OutBufSize] u8
        result:               *Array'u8
    }
}

// Main structure representing a Deflate compression context
#[Swag.Opaque]
public struct Deflate
{
    state:         *Compressor                    // Internal compressor state
    adler:         Hash.Adler32                   // Adler-32 checksum calculator
    initFlags:     CompressionFlags = Default     // Initialization flags (e.g. compute checksum)
}

impl Deflate
{
    func createCompFlagsFromZipParams(level: CompressionLevel, strategy: CompressionStrategy)->u32
    {
        var compFlags = 0'u32
        compFlags = NumProbes[Math.min(10'u32, level)]
        if level <= 3 do
            compFlags |= GreedyParsing

        if level == .NoCompression do
            compFlags |= ForceAllRawBlocks
        elif strategy == .Filtered do
            compFlags |= FilterMatches
        elif strategy == .HuffmanOnly do
            compFlags &= ~MaxProbesMask
        elif strategy == .Fixed do
            compFlags |= ForceAllStaticBlocks
        elif strategy == .Rle do
            compFlags |= RleMatches

        return compFlags
    }

    mtd setup(flags: u32)
    {
        let d = .state
        d.compressFlags    = flags
        d.maxProbes[0]     = 1 + ((flags & 0xFFF) + 2) / 3
        d.greedyParsing    = (flags & GreedyParsing) != 0
        d.maxProbes[1]     = 1 + (((flags & 0xFFF) >> 2) + 2) / 3
        d.lzCodeBufAddr    = &d.lzCodeBuf[1]
        d.lzFlags          = &d.lzCodeBuf[0]
        d.numFlagsLeft     = 8
        d.outputBufAddr    = d.outputBuf
        d.outputBufEndAddr = d.outputBuf
    }

    mtd recordLiteral(lit: u8)
    {
        let d = .state

        d.totalLzBytes += 1
        dref d.lzCodeBufAddr = lit
        d.lzCodeBufAddr += 1
        dref d.lzFlags = cast(u8) (dref d.lzFlags >> 1)

        d.numFlagsLeft -= 1
        if d.numFlagsLeft == 0
        {
            d.numFlagsLeft = 8
            d.lzFlags      = d.lzCodeBufAddr
            d.lzCodeBufAddr += 1
        }

        d.huffCount[0, lit] += 1
    }

    mtd recordMatch(matchLen: u32, md: u32)
    {
        var matchDist = md
        let d         = .state

        d.totalLzBytes += matchLen

        d.lzCodeBufAddr[0] = cast(u8) (matchLen - MinMatchLen)

        matchDist -= 1
        d.lzCodeBufAddr[1] = cast(u8) (matchDist & 0xFF)
        d.lzCodeBufAddr[2] = cast(u8) (matchDist >> 8)
        d.lzCodeBufAddr += 3

        dref d.lzFlags = cast(u8) ((dref d.lzFlags >> 1) | 0x80)
        d.numFlagsLeft -= 1
        if d.numFlagsLeft == 0
        {
            d.numFlagsLeft = 8
            d.lzFlags      = d.lzCodeBufAddr
            d.lzCodeBufAddr += 1
        }

        let s0 = SmallDistSym[matchDist & 511]
        let s1 = LargeDistSym[(matchDist >> 8) & 127]
        d.huffCount[1, (matchDist < 512) ? s0 : s1] += 1
        d.huffCount[0, LenSym[matchLen - MinMatchLen]] += 1
    }

    #[Swag.Safety(.All, .All, false)]
    mtd findMatch(laPos, maxDist, maxMatchLen: u32, matchDistAddr, matchLenAddr: *u32)
    {
        let d = .state

        let pos      = laPos & LzDictSizeMask
        var matchLen = dref matchLenAddr
        var probePos = pos

        var numProbesLeft = d.maxProbes[matchLen >= 32 ? 1 : 0]
        let s             = &d.dict[0] + pos
        var c0            = d.dict[pos + matchLen]
        var c1            = d.dict[pos + matchLen - 1]

        if maxMatchLen <= matchLen do
            return

        for
        {
            var nextProbePos: u32
            var dist:         u16

            for
            {
                numProbesLeft -= 1
                if numProbesLeft == 0 do
                    return

                #[Swag.Mixin, Swag.CalleeReturn]
                func probe()
                {
                    nextProbePos = d.next[probePos]
                    if !nextProbePos do
                        return
                    dist = cast(u16) (laPos - nextProbePos)
                    if dist > maxDist do
                        return
                    probePos = nextProbePos & LzDictSizeMask
                    if (d.dict[probePos + matchLen] == c0) and (d.dict[probePos + matchLen - 1] == c1) do
                        break
                }

                probe()
                probe()
                probe()
            }

            if !dist do
                break

            var p         = s
            var q         = &d.dict[0] + probePos
            var probeLen: u32
            while probeLen < maxMatchLen
            {
                let pp = dref p
                let qq = dref q
                p, q += 1
                if pp != qq do
                    break
                probeLen += 1
            }

            if probeLen > matchLen
            {
                dref matchDistAddr = dist
                dref matchLenAddr  = probeLen
                matchLen           = probeLen
                if matchLen == maxMatchLen do
                    return
                c0 = d.dict[pos + matchLen]
                c1 = d.dict[pos + matchLen - 1]
            }
        }
    }

    //#[Swag.Macro]
    mtd putBits(b, l: u32)
    {
        let d    = .state
        let bits = b
        let len  = l

        d.bitBuffer |= (bits << d.bitsIn)
        d.bitsIn += len
        while d.bitsIn >= 8
        {
            if d.outputBufAddr < d.outputBufEndAddr
            {
                dref d.outputBufAddr = cast(u8) d.bitBuffer
                d.outputBufAddr += 1
            }

            d.bitBuffer >>= 8
            d.bitsIn -= 8
        }
    }

    func radixSortSyms(numSyms: u32, sym0, sym1: [*] SymFreq)->[*] SymFreq
    {
        var hist: [256 * 2] u32
        for i in numSyms
        {
            let freq = sym0[i].key
            hist[freq & 0xFF] += 1
            hist[256 + ((freq >> 8) & 0xFF)] += 1
        }

        var totalPasses = 2
        while (totalPasses > 1) and (numSyms == hist[(totalPasses - 1) * 256]) do
            totalPasses -= 1

        var curSym    = sym0
        var newSym    = sym1
        var passShift = 0
        for pass in totalPasses
        {
            let pHist  = &hist[pass << 8]
            var curOfs = 0

            var offsets: [256] u32 = undefined
            for i in 256
            {
                offsets[i] = curOfs
                curOfs += pHist[i]
            }

            for i in numSyms
            {
                var ptr = &offsets[(curSym[i].key >> passShift) & 0xFF]
                newSym[dref ptr] = curSym[i]
                dref ptr += 1
            }

            swap(&curSym, &newSym)
            passShift += 8
        }

        return curSym
    }

    func calculateMinimumRedundancy(symFreq: [*] SymFreq, n: u32)
    {
        if n == 0 do
            return

        if n == 1
        {
            symFreq[0].key = 1
            return
        }

        symFreq[0].key += symFreq[1].key
        var root = 0
        var leaf = 2
        for var next = 1; next < n - 1; next += 1
        {
            if leaf >= n or symFreq[root].key < symFreq[leaf].key
            {
                symFreq[next].key = symFreq[root].key
                symFreq[root].key = cast(u16) next
                root += 1
            }
            else
            {
                symFreq[next].key = symFreq[leaf].key
                leaf += 1
            }

            if leaf >= n or (root < next and symFreq[root].key < symFreq[leaf].key)
            {
                symFreq[next].key = cast(u16) (symFreq[next].key + symFreq[root].key)
                symFreq[root].key = cast(u16) next
                root += 1
            }
            else
            {
                symFreq[next].key = cast(u16) (symFreq[next].key + symFreq[leaf].key)
                leaf += 1
            }
        }

        symFreq[n - 2].key = 0
        for var next = n - 3; cast(s32) next >= 0; next -= 1 do
            symFreq[next].key = cast(u16) (symFreq[symFreq[next].key].key + 1)

        var avbl = 1
        var used = 0
        var dpth = 0
        root = n - 2
        var next = n - 1
        while avbl > 0
        {
            while root >= 0 and cast(s32) symFreq[root].key == dpth
            {
                used += 1
                root -= 1
            }

            while avbl > used
            {
                symFreq[next].key = cast(u16) dpth
                next -= 1
                avbl -= 1
            }

            avbl = 2 * used
            dpth += 1
            used = 0
        }
    }

    func huffmanEnforceMaxCodeSize(numCodes: [*] s32, codeListLen, maxCodeSize: u32)
    {
        if codeListLen <= 1 do
            return

        for var i = maxCodeSize + 1; i <= MaxSupportedHuffCodeSize; i += 1 do
            numCodes[maxCodeSize] += numCodes[i]

        var total = 0'u32
        for var i = cast(s32) maxCodeSize; i > 0; i -= 1 do
            total += (cast(u32) numCodes[i] << (maxCodeSize - i))

        while total != (1'u32 << maxCodeSize)
        {
            numCodes[maxCodeSize] -= 1
            for var i = cast(s32) maxCodeSize - 1; i > 0; i -= 1
            {
                if numCodes[i]
                {
                    numCodes[i] -= 1
                    numCodes[i + 1] += 2
                    break
                }
            }

            total -= 1
        }
    }

    mtd optimizeHuffmanTable(tableNum, tableLen, codeSizeLimit: u32, staticTable: bool)
    {
        let d = .state

        var numCodes: [1 + MaxSupportedHuffCodeSize] s32
        var nextCode: [MaxSupportedHuffCodeSize + 1] u32

        if staticTable
        {
            for var i = 0; i < tableLen; i += 1
            {
                numCodes[d.huffCodeSizes[tableNum, i]] += 1
            }
        }
        else
        {
            var syms0: [MaxHuffSymbols] SymFreq
            var syms1: [MaxHuffSymbols] SymFreq

            var numUsedSyms = 0'u32
            let symCount    = &d.huffCount[tableNum, 0]

            for i in tableLen where symCount[i]
            {
                syms0[numUsedSyms].key      = cast(u16) symCount[i]
                syms0[numUsedSyms].symIndex = cast(u16) i
                numUsedSyms += 1
            }

            let syms = radixSortSyms(numUsedSyms, syms0, syms1)
            calculateMinimumRedundancy(syms, numUsedSyms)

            for i in numUsedSyms
            {
                numCodes[syms[i].key] += 1
            }

            huffmanEnforceMaxCodeSize(numCodes, numUsedSyms, codeSizeLimit)

            Memory.clear(&d.huffCodeSizes[tableNum], #sizeof(d.huffCodeSizes[tableNum]))
            Memory.clear(&d.huffCodes[tableNum], #sizeof(d.huffCodes[tableNum]))

            for var i = 1, j = numUsedSyms; i <= codeSizeLimit; i += 1
            {
                for var l = numCodes[i]; l > 0; l -= 1
                {
                    j -= 1
                    d.huffCodeSizes[tableNum, syms[j].symIndex] = cast(u8) i
                }
            }
        }

        nextCode[1] = 0
        var j = 0
        for var i = 2; i <= codeSizeLimit; i += 1
        {
            j           = (j + numCodes[i - 1]) << 1
            nextCode[i] = j
        }

        for var i = 0; i < tableLen; i += 1
        {
            var rev_code = 0'u32
            let codeSize = d.huffCodeSizes[tableNum, i]
            if codeSize == 0 do
                continue
            var cde = nextCode[codeSize]
            nextCode[codeSize] += 1
            for var l = codeSize; l > 0; l -= 1
            {
                rev_code = (rev_code << 1) | (cde & 1)
                cde >>= 1
            }

            d.huffCodes[tableNum, i] = cast(u16) rev_code
        }
    }

    mtd startStaticBlock()
    {
        let d = .state
        var p = &d.huffCodeSizes[0, 0]

        var i = 0
        while i <= 143
        {
            dref p = 8
            p += 1
            i += 1
        }

        while i <= 255
        {
            dref p = 9
            p += 1
            i += 1
        }

        while i <= 279
        {
            dref p = 7
            p += 1
            i += 1
        }

        while i <= 287
        {
            dref p = 8
            p += 1
            i += 1
        }

        Memory.set(d.huffCodeSizes[1], 5, 32)

        .optimizeHuffmanTable(0, 288, 15, true)
        .optimizeHuffmanTable(1, 32, 15, true)

        .putBits(1, 2)
    }

    #[Swag.Mixin]
    mtd rlePrevCodeSize()
    {
        if rleRepeatCount
        {
            let d = .state
            if rleRepeatCount < 3
            {
                d.huffCount[2, prevCodeSize] = cast(u16) (d.huffCount[2, prevCodeSize] + rleRepeatCount)
                while rleRepeatCount
                {
                    packedCodeSizes[numPackedCodeSizes] = prevCodeSize
                    numPackedCodeSizes += 1
                    rleRepeatCount -= 1
                }
            }
            else
            {
                d.huffCount[2, 16]                  = cast(u16) (d.huffCount[2, 16] + 1)
                packedCodeSizes[numPackedCodeSizes] = 16
                numPackedCodeSizes += 1
                packedCodeSizes[numPackedCodeSizes] = cast(u8) (rleRepeatCount - 3)
                numPackedCodeSizes += 1
            }

            rleRepeatCount = 0
        }
    }

    #[Swag.Mixin]
    mtd rleZeroCodeSize()
    {
        if rleZCount
        {
            if rleZCount < 3
            {
                d.huffCount[2, 0] = cast(u16) (d.huffCount[2, 0] + rleZCount)
                while rleZCount
                {
                    packedCodeSizes[numPackedCodeSizes] = 0
                    numPackedCodeSizes += 1
                    rleZCount -= 1
                }
            }
            elif rleZCount <= 10
            {
                d.huffCount[2, 17]                  = cast(u16) (d.huffCount[2, 17] + 1)
                packedCodeSizes[numPackedCodeSizes] = 17
                numPackedCodeSizes += 1
                packedCodeSizes[numPackedCodeSizes] = cast(u8) (rleZCount - 3)
                numPackedCodeSizes += 1
            }
            else
            {
                d.huffCount[2, 18]                  = cast(u16) (d.huffCount[2, 18] + 1)
                packedCodeSizes[numPackedCodeSizes] = 18
                numPackedCodeSizes += 1
                packedCodeSizes[numPackedCodeSizes] = cast(u8) (rleZCount - 11)
                numPackedCodeSizes += 1
            }

            rleZCount = 0
        }
    }

    mtd startDynamicBlock()
    {
        let d = .state

        var codeSizesToPack: [MaxHuffSymbols0 + MaxHuffSymbols1] u8 = undefined
        var packedCodeSizes: [MaxHuffSymbols0 + MaxHuffSymbols1] u8 = undefined
        var prevCodeSize     = 0xFF'u8

        d.huffCount[0, 256] = 1

        .optimizeHuffmanTable(0, MaxHuffSymbols0, 15, false)
        .optimizeHuffmanTable(1, MaxHuffSymbols1, 15, false)

        var numLitCodes = 286'u32
        while numLitCodes > 257
        {
            if d.huffCodeSizes[0, numLitCodes - 1] do
                break
            numLitCodes -= 1
        }

        var numDistCodes = 30'u32
        while numDistCodes > 1
        {
            if d.huffCodeSizes[1, numDistCodes - 1] do
                break
            numDistCodes -= 1
        }

        Memory.copy(&codeSizesToPack[0], &d.huffCodeSizes[0, 0], cast(u64) numLitCodes)
        Memory.copy(&codeSizesToPack[0] + numLitCodes, &d.huffCodeSizes[1, 0], cast(u64) numDistCodes)
        let totalCodeSizesToPack = numLitCodes + numDistCodes
        var numPackedCodeSizes   = 0
        var rleZCount            = 0
        var rleRepeatCount       = 0

        Memory.clear(&d.huffCount[2, 0], cast(u64) #sizeof(d.huffCount[2, 0]) * MaxHuffSymbols2)
        for i in totalCodeSizesToPack
        {
            let codeSize = codeSizesToPack[i]
            if !codeSize
            {
                .rlePrevCodeSize()
                rleZCount += 1
                if rleZCount == 138
                {
                    .rleZeroCodeSize()
                }
            }
            else
            {
                .rleZeroCodeSize()
                if codeSize != prevCodeSize
                {
                    .rlePrevCodeSize()
                    d.huffCount[2, codeSize]            = cast(u16) (d.huffCount[2, codeSize] + 1)
                    packedCodeSizes[numPackedCodeSizes] = codeSize
                    numPackedCodeSizes += 1
                }
                else
                {
                    rleRepeatCount += 1
                    if rleRepeatCount == 6
                    {
                        .rlePrevCodeSize()
                    }
                }
            }

            prevCodeSize = codeSize
        }

        if rleRepeatCount
        {
            .rlePrevCodeSize()
        }
        else
        {
            .rleZeroCodeSize()
        }

        .optimizeHuffmanTable(2, MaxHuffSymbols2, 7, false)

        .putBits(2, 2)
        .putBits(numLitCodes - 257, 5)
        .putBits(numDistCodes - 1, 5)

        var numBitLengths = 18'u32
        while numBitLengths >= 0
        {
            if d.huffCodeSizes[2, PackedCodeSizeSymsSwizzle[numBitLengths]] do
                break
            numBitLengths -= 1
        }

        numBitLengths = Math.max(4, numBitLengths + 1)
        .putBits(numBitLengths - 4, 4)
        for i in numBitLengths
        {
            .putBits(cast(u32) d.huffCodeSizes[2, PackedCodeSizeSymsSwizzle[i]], 3)
        }

        var packedCodeSizesIndex = 0
        while packedCodeSizesIndex < numPackedCodeSizes
        {
            let cde = packedCodeSizes[packedCodeSizesIndex]
            packedCodeSizesIndex += 1
            .putBits(cast(u32) d.huffCodes[2, cde], cast(u32) d.huffCodeSizes[2, cde])
            if cde >= 16
            {
                const p = [2, 3, 7]
                .putBits(cast(u32) packedCodeSizes[packedCodeSizesIndex], cast(u32) p[cde - 16])
                packedCodeSizesIndex += 1
            }
        }
    }

    mtd compressLzCodes()->bool
    {
        let d     = .state
        var flags = 1'u32
        for var lzCodes = &d.lzCodeBuf[0]; lzCodes < d.lzCodeBufAddr; flags >>= 1
        {
            if flags == 1
            {
                flags = cast(u32) (dref lzCodes) | 0x100
                lzCodes += 1
            }

            if flags & 1
            {
                let matchLen  = cast(u32) lzCodes[0]
                let matchDist = cast(u32) lzCodes[1] | (cast(u32) lzCodes[2] << 8)
                lzCodes += 3

                .putBits(cast(u32) d.huffCodes[0, LenSym[matchLen]], cast(u32) d.huffCodeSizes[0, LenSym[matchLen]])
                .putBits(matchLen & Bitmasks[LenExtra[matchLen]], cast(u32) LenExtra[matchLen])

                var sym, numExtraBits: u32
                if matchDist < 512
                {
                    sym          = SmallDistSym[matchDist]
                    numExtraBits = SmallDistExtra[matchDist]
                }
                else
                {
                    sym          = LargeDistSym[matchDist >> 8]
                    numExtraBits = LargeDistExtra[matchDist >> 8]
                }

                .putBits(cast(u32) d.huffCodes[1, sym], cast(u32) d.huffCodeSizes[1, sym])
                .putBits(matchDist & Bitmasks[numExtraBits], numExtraBits)
            }
            else
            {
                let lit = dref lzCodes
                lzCodes += 1
                .putBits(cast(u32) d.huffCodes[0, lit], cast(u32) d.huffCodeSizes[0, lit])
            }
        }

        .putBits(cast(u32) d.huffCodes[0, 256], cast(u32) d.huffCodeSizes[0, 256])
        return d.outputBufAddr < d.outputBufEndAddr
    }

    mtd compressBlock(static_block: bool)->bool
    {
        if static_block do
            .startStaticBlock()
        else do
            .startDynamicBlock()
        return .compressLzCodes()
    }

    mtd flushBlock(finish: bool)
    {
        let d = .state

        var compBlockSucceeded = false
        let useRawBlock        = ((d.compressFlags & ForceAllRawBlocks) != 0) and (d.lookaheadPos - d.lzCodeBufDictPos) <= d.dictSize

        var outputBufStart: [*] u8
        if d.result.capacity - d.result.count >= OutBufSize do
            outputBufStart = d.result.buffer + d.outBufOfs
        else do
            outputBufStart = &d.outputBuf[0]

        d.outputBufAddr    = outputBufStart
        d.outputBufEndAddr = d.outputBufAddr + OutBufSize - 16

        d.outputFlushOfs = 0

        dref d.lzFlags = cast(u8) (dref d.lzFlags >> d.numFlagsLeft)
        d.lzCodeBufAddr -= (d.numFlagsLeft == 8) ? 1 : 0

        .putBits(finish ? 1 : 0, 1)

        let savedOutputBuf = d.outputBufAddr
        let savedBitBuf    = d.bitBuffer
        let savedBitsIn    = d.bitsIn

        if !useRawBlock do
            compBlockSucceeded = .compressBlock((d.compressFlags & ForceAllStaticBlocks) or (d.totalLzBytes < 48))

        if ((useRawBlock or (d.totalLzBytes and ((d.outputBufAddr - savedOutputBuf + 1) >= d.totalLzBytes))) and
           ((d.lookaheadPos - d.lzCodeBufDictPos) <= d.dictSize))
        {
            d.outputBufAddr = savedOutputBuf
            d.bitBuffer     = savedBitBuf
            d.bitsIn        = savedBitsIn
            .putBits(0, 2)
            if d.bitsIn
            {
                .putBits(0, 8 - d.bitsIn)
            }

            for var i = 2; i != 0; i -= 1
            {
                .putBits(d.totalLzBytes & 0xFFFF, 16)
                d.totalLzBytes ^= 0xFFFF
            }

            for i in d.totalLzBytes
            {
                .putBits(cast(u32) d.dict[(d.lzCodeBufDictPos + i) & LzDictSizeMask], 8)
            }
        }
        elif !compBlockSucceeded
        {
            d.outputBufAddr = savedOutputBuf
            d.bitBuffer     = savedBitBuf
            d.bitsIn        = savedBitsIn
            discard .compressBlock(true)
        }

        if finish and d.bitsIn do
            .putBits(0, 8 - d.bitsIn)

        Memory.clear(&d.huffCount[0, 0], cast(u64) #sizeof(d.huffCount[0, 0]) * MaxHuffSymbols0)
        Memory.clear(&d.huffCount[1, 0], cast(u64) #sizeof(d.huffCount[1, 0]) * MaxHuffSymbols1)

        d.lzCodeBufAddr = &d.lzCodeBuf[0] + 1
        d.lzFlags       = &d.lzCodeBuf[0]
        d.numFlagsLeft  = 8
        d.lzCodeBufDictPos += d.totalLzBytes
        d.totalLzBytes = 0
        d.blockIndex += 1

        let n = cast(u64) (d.outputBufAddr - outputBufStart)
        if n != 0
        {
            if (outputBufStart == &d.outputBuf[0])
            {
                d.result.resize(d.result.count + n)
                Memory.copy(d.result.buffer + d.outBufOfs, d.outputBuf, cast(u64) n)
                d.outBufOfs += n
            }
            else
            {
                d.outBufOfs += n
                d.result.count += n
            }
        }
    }

    #[Swag.Overload]
    mtd compress() throw
    {
        let d          = .state
        var pSrc       = d.inSrc
        var srcBufLeft = d.srcBufLeft

        while srcBufLeft or d.lookaheadSize
        {
            if (d.lookaheadSize + d.dictSize) >= (MinMatchLen - 1)
            {
                var dstPos: u32 = (d.lookaheadPos + d.lookaheadSize) & LzDictSizeMask
                var insPos: u32 = d.lookaheadPos + d.lookaheadSize - 2
                var hash:   u32 = (cast(u32) d.dict[insPos & LzDictSizeMask] << LzHashShift)
                hash ^= d.dict[(insPos + 1) & LzDictSizeMask]

                let numBytesToProcess = cast(u32) Math.min(cast(u32) srcBufLeft, cast(u32) (MaxMatchLen - d.lookaheadSize))

                let pSrc_end = pSrc ? pSrc + numBytesToProcess : null
                srcBufLeft -= numBytesToProcess
                d.lookaheadSize += numBytesToProcess

                Debug.assert(pSrc != null)
                while pSrc != pSrc_end
                {
                    let c = dref pSrc
                    pSrc += 1
                    d.dict[dstPos] = c
                    if dstPos < (MaxMatchLen - 1) do
                        d.dict[LzDictSize + dstPos] = c
                    hash                            = ((hash << LzHashShift) ^ c) & (LzHashSize - 1)
                    d.next[insPos & LzDictSizeMask] = d.hash[hash]
                    d.hash[hash]                    = cast(u16) insPos
                    dstPos                          = (dstPos + 1) & LzDictSizeMask
                    insPos += 1
                }
            }
            else
            {
                while srcBufLeft and d.lookaheadSize < MaxMatchLen
                {
                    let c = dref pSrc
                    pSrc += 1
                    let dstPos = (d.lookaheadPos + d.lookaheadSize) & LzDictSizeMask
                    srcBufLeft -= 1
                    d.dict[dstPos] = c
                    if dstPos < (MaxMatchLen - 1) do
                        d.dict[LzDictSize + dstPos] = c
                    d.lookaheadSize += 1
                    if (d.lookaheadSize + d.dictSize) >= MinMatchLen
                    {
                        let insPos: u32 = d.lookaheadPos + (d.lookaheadSize - 1) - 2
                        var hash:   u32 = (cast(u32) d.dict[insPos & LzDictSizeMask] << (LzHashShift * 2))
                        hash = (hash ^ (cast(u32) d.dict[(insPos + 1) & LzDictSizeMask] << LzHashShift) ^ c)
                        hash &= (LzHashSize - 1)
                        d.next[insPos & LzDictSizeMask] = d.hash[hash]
                        d.hash[hash]                    = cast(u16) insPos
                    }
                }
            }

            d.dictSize = Math.min(cast(u32) (LzDictSize - d.lookaheadSize), d.dictSize)

            var lenToMove    = 1'u32
            var curMatchDist = 0'u32
            var curMatchLen  = d.savedMatchLen ? d.savedMatchLen : cast(u32) (MinMatchLen - 1)

            let curPos: u32 = d.lookaheadPos & LzDictSizeMask
            if d.compressFlags & (RleMatches | ForceAllRawBlocks)
            {
                if d.dictSize and !(d.compressFlags & ForceAllRawBlocks)
                {
                    let c = d.dict[(curPos - 1) & LzDictSizeMask]
                    curMatchLen = 0
                    while curMatchLen < d.lookaheadSize
                    {
                        if d.dict[curPos + curMatchLen] != c do
                            break
                        curMatchLen += 1
                    }
                    if curMatchLen < MinMatchLen do
                        curMatchLen = 0
                    else do
                        curMatchDist = 1
                }
            }
            else
            {
                .findMatch(d.lookaheadPos, d.dictSize, d.lookaheadSize, &curMatchDist, &curMatchLen)
            }

            if ((curMatchLen == MinMatchLen) and (curMatchDist >= 8 * 1024)) or
               (curPos == curMatchDist) or
               ((d.compressFlags & FilterMatches) and (curMatchLen <= 5))
            {
                curMatchDist, curMatchLen = 0
            }

            if d.savedMatchLen
            {
                if curMatchLen > d.savedMatchLen
                {
                    .recordLiteral(cast(u8) d.savedLit)
                    if curMatchLen >= 128
                    {
                        .recordMatch(curMatchLen, curMatchDist)
                        d.savedMatchLen = 0
                        lenToMove       = curMatchLen
                    }
                    else
                    {
                        d.savedLit       = d.dict[curPos]
                        d.savedMatchDist = curMatchDist
                        d.savedMatchLen  = curMatchLen
                    }
                }
                else
                {
                    .recordMatch(d.savedMatchLen, d.savedMatchDist)
                    lenToMove       = d.savedMatchLen - 1
                    d.savedMatchLen = 0
                }
            }
            elif !curMatchDist
            {
                .recordLiteral(d.dict[Math.min(curPos, cast(u32) #sizeof(d.dict) - 1)])
            }
            elif d.greedyParsing or (d.compressFlags & RleMatches) or (curMatchLen >= 128)
            {
                .recordMatch(curMatchLen, curMatchDist)
                lenToMove = curMatchLen
            }
            else
            {
                d.savedLit       = d.dict[Math.min(curPos, cast(u32) #sizeof(d.dict) - 1)]
                d.savedMatchDist = curMatchDist
                d.savedMatchLen  = curMatchLen
            }

            d.lookaheadPos += lenToMove
            Debug.assert(d.lookaheadSize >= lenToMove)
            d.lookaheadSize -= lenToMove
            d.dictSize = Math.min(d.dictSize + lenToMove, cast(u32) LzDictSize)

            if (d.lzCodeBufAddr > &d.lzCodeBuf[LzCodeBufSize - 8]) or
               ((d.totalLzBytes > 31 * 1024) and ((((cast(u32) (d.lzCodeBufAddr - &d.lzCodeBuf[0]) * 115) >> 7) >= d.totalLzBytes) or (d.compressFlags & ForceAllRawBlocks)))
            {
                d.inSrc      = pSrc
                d.srcBufLeft = srcBufLeft
                .flushBlock(false)
            }
        }

        d.inSrc      = pSrc
        d.srcBufLeft = srcBufLeft
    }
}

public impl Deflate
{
    // Flags controlling compression behavior
    #[Swag.EnumFlags]
    enum CompressionFlags
    {
        Zero           = 0                  // No compression flags
        ComputeAdler32                      // Compute Adler-32 checksum after compression
        Default        = ComputeAdler32     // Default flag: computes Adler-32
    }

    // Compression level to use (mapped to number of probes and strategies)
    #[Swag.Incomplete]
    enum CompressionLevel: u32
    {
        NoCompression   = 0      // Disables compression (raw storage)
        BestSpeed       = 1      // Fastest compression, less effective
        Default         = 6      // Balanced default compression
        BestCompression = 9      // High compression ratio, slower
        UberCompression = 10     // Maximum compression effort
    }

    // Strategy for compression to influence algorithm behavior
    enum CompressionStrategy
    {
        Default         // Default compression strategy
        Filtered        // Filter out potentially poor matches
        HuffmanOnly     // Use only Huffman coding, no match search
        Rle             // Run-length encoding
        Fixed           // Use fixed Huffman codes
    }

    mtd opDrop()
    {
        Memory.delete(.state)
    }

    // Initialize the compressor
    // Can be called multiple times
    mtd init(level = CompressionLevel.Default, strategy = CompressionStrategy.Default, flags = CompressionFlags.Default)
    {
        if !.state do
            .state = Memory.new'Compressor()
        else do
            @init(.state, 1)
        .setup(createCompFlagsFromZipParams(level, strategy))
        .initFlags = flags
    }

    // Compress the source buffer
    #[Swag.Overload]
    mtd compress(result: *Array'u8, src: const [..] u8) throw
    {
        if @countof(src) > Swag.U32.Max do
            throw UnsupportedFormatError{"source buffer too big"}

        Debug.assert(.state != null, "'init' not called")

        .state.result     = result
        .state.inBuf      = @dataof(src)
        .state.inSrc      = @dataof(src)
        .state.srcBufLeft = @countof(src)
        .state.outBufOfs  = result.count

        .compress()
        .flushBlock(true)
        Debug.assert(!.state.lookaheadSize and !.state.srcBufLeft)

        if .initFlags.has(.ComputeAdler32)
        {
            .adler.init()
            .adler.update(src)
        }
    }
}
