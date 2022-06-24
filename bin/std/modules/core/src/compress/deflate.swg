/*
    :LICENCE
    This code is based on the miniz library (https://github.com/richgel999/miniz)
    The original code has been modified for Swag
    See LICENCE.md for the corresponding licence.
*/

#global namespace Compress
#global #[Swag.Safety("overflow", false)]

using namespace DeflatePrivate {

const PackedCodeSizeSymsSwizzle: [] u8 = @[
    16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15
]

const Bitmasks: [17] u32 = @[
    0x0000, 0x0001, 0x0003, 0x0007, 0x000F, 0x001F, 0x003F, 0x007F, 0x00FF, 0x01FF, 0x03FF, 0x07FF, 0x0FFF, 0x1FFF, 0x3FFF, 0x7FFF, 0xFFFF
]

const LenExtra: [256] u8 = @[
    0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 0
]

const LenSym: [256] u16 = @[
    257, 258, 259, 260, 261, 262, 263, 264, 265, 265, 266, 266, 267, 267, 268, 268, 269, 269, 269, 269, 270, 270, 270, 270, 271, 271, 271, 271, 272, 272, 272, 272,
    273, 273, 273, 273, 273, 273, 273, 273, 274, 274, 274, 274, 274, 274, 274, 274, 275, 275, 275, 275, 275, 275, 275, 275, 276, 276, 276, 276, 276, 276, 276, 276,
    277, 277, 277, 277, 277, 277, 277, 277, 277, 277, 277, 277, 277, 277, 277, 277, 278, 278, 278, 278, 278, 278, 278, 278, 278, 278, 278, 278, 278, 278, 278, 278,
    279, 279, 279, 279, 279, 279, 279, 279, 279, 279, 279, 279, 279, 279, 279, 279, 280, 280, 280, 280, 280, 280, 280, 280, 280, 280, 280, 280, 280, 280, 280, 280,
    281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281,
    282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282,
    283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283,
    284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 285
]

const SmallDistSym: [512] u8 = @[
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
    17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17
]

const SmallDistExtra: [512] u8 = @[
    0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
    6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
    6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7
]

const LargeDistExtra: [128] u8 = @[
    0, 0, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
    12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
    13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13
]

const LargeDistSym: [128] u8 = @[
    0, 0, 18, 19, 20, 20, 21, 21, 22, 22, 22, 22, 23, 23, 23, 23, 24, 24, 24, 24, 24, 24, 24, 24, 25, 25, 25, 25, 25, 25, 25, 25, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26,
    26, 26, 26, 26, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
    28, 28, 28, 28, 28, 28, 28, 28, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29
]

const NumProbes: [11] u32 = @[
    0, 1, 6, 32, 16, 32, 128, 256, 512, 768, 1500
]

const MaxSupportedHuffCodeSize  = 32
const MaxHuffTables             = 3
const MaxHuffSymbols0           = 288'u32
const MaxHuffSymbols1           = 32'u32
const MaxHuffSymbols2           = 19'u32
const LzDictSize                = 32768
const LzDictSizeMask            = LzDictSize - 1
const MinMatchLen               = 3
const MaxMatchLen               = 258

const LzCodeBufSize    = 24 * 1024
const LzHashBits       = 12
//const LzCodeBufSize  = 64 * 1024
//const LzHashBits     = 15

const OutBufSize          = (LzCodeBufSize * 13) / 10
const MaxHuffSymbols      = 288
const Level1HashSizeMask  = 4095
const LzHashShift         = (LzHashBits + 2) / 3
const LzHashSize          = 1 << LzHashBits

const GreedyParsing             = 0x04000
const NonDeterministicParsing   = 0x08000
const RleMatches                = 0x10000
const FilterMatches             = 0x20000
const ForceAllStaticBlocks      = 0x40000
const ForceAllRawBlocks         = 0x80000

const HuffmanOnly       = 0
const DefaultMaxProbes  = 128
const MaxProbesMask     = 0xFFF

struct SymFreq
{
    key:      u16
    symIndex: u16
}

struct Compressor
{
    m_flags:            u32
    maxProbes:          [2] u32
    greedyParsing:      bool
    lookaheadPos:       u32
    lookaheadSize:      u32
    dictSize:           u32
    lzCodeBuf:          *u8
    lzFlags:            *u8
    outputBuf:          *u8
    outputBufEnd:       *u8
    numFlagsLeft:       u32
    totalLzBytes:       u32
    lzCodeBufDictPos:   u32
    bitsIn:             u32
    bitBuffer:          u32
    savedMatchDist:     u32
    savedMatchLen:      u32
    savedLit:           u32
    outputFlushOfs:     u32
    blockIndex:         u32
    inBuf:              const *u8
    inSrc:              const *u8
    srcBufLeft:         uint
    outBufOfs:          uint
    m_dict:             [LzDictSize + MaxMatchLen - 1] u8
    m_huff_count:       [MaxHuffTables, MaxHuffSymbols] u16
    m_huff_codes:       [MaxHuffTables, MaxHuffSymbols] u16
    m_huff_code_sizes:  [MaxHuffTables, MaxHuffSymbols] u8
    m_lz_code_buf:      [LzCodeBufSize] u8
    m_next:             [LzDictSize] u16
    m_hash:             [LzHashSize] u16
    m_output_buf:       [OutBufSize] u8
    result:             *Array'u8
}
}

#[Swag.Opaque]
public struct Deflate
{
    state:      *Compressor
    adler:      Hash.Adler32
    initFlags:  CompressionFlags = Default
}

impl Deflate
{
    func createCompFlagsFromZipParams(level: CompressionLevel, strategy: CompressionStrategy)->u32
    {
        compFlags := 0'u32
        compFlags = NumProbes[Math.min(10'u32, level)]
        if level <= 3
            compFlags |= GreedyParsing

        if level == .NoCompression
            compFlags |= ForceAllRawBlocks
        elif strategy == .Filtered
            compFlags |= FilterMatches
        elif strategy ==. HuffmanOnly
            compFlags &= ~MaxProbesMask
        elif strategy == .Fixed
            compFlags |= ForceAllStaticBlocks
        elif strategy == .Rle
            compFlags |= RleMatches

        return compFlags
    }

    mtd setup(flags: u32)
    {
        d := state
        d.m_flags = flags
        d.maxProbes[0] = 1 + ((flags & 0xFFF) + 2) / 3
        d.greedyParsing = (flags & GreedyParsing) != 0
        d.maxProbes[1] = 1 + (((flags & 0xFFF) >> 2) + 2) / 3
        d.lzCodeBuf = &d.m_lz_code_buf[1]
        d.lzFlags = &d.m_lz_code_buf[0]
        d.numFlagsLeft = 8
        d.outputBuf = d.m_output_buf
        d.outputBufEnd = d.m_output_buf
    }

    mtd recordLiteral(lit: u8)
    {
        d := state

        d.totalLzBytes += 1
        dref d.lzCodeBuf = lit
        d.lzCodeBuf += 1
        dref d.lzFlags = cast(u8) (dref d.lzFlags >> 1)

        d.numFlagsLeft -= 1
        if d.numFlagsLeft == 0
        {
            d.numFlagsLeft = 8
            d.lzFlags = d.lzCodeBuf
            d.lzCodeBuf += 1
        }

        d.m_huff_count[0, lit] += 1
    }

    mtd recordMatch(match_len: u32, md: u32)
    {
        match_dist := md
        d := state

        d.totalLzBytes += match_len

        d.lzCodeBuf[0] = cast(u8) (match_len - MinMatchLen)

        match_dist -= 1
        d.lzCodeBuf[1] = cast(u8) (match_dist & 0xFF)
        d.lzCodeBuf[2] = cast(u8) (match_dist >> 8)
        d.lzCodeBuf += 3

        dref d.lzFlags = cast(u8) ((dref d.lzFlags >> 1) | 0x80)
        d.numFlagsLeft -= 1
        if d.numFlagsLeft == 0
        {
            d.numFlagsLeft = 8
            d.lzFlags = d.lzCodeBuf
            d.lzCodeBuf += 1
        }

        s0 := SmallDistSym[match_dist & 511]
        s1 := LargeDistSym[(match_dist >> 8) & 127]
        d.m_huff_count[1, (match_dist < 512) ? s0 : s1] += 1
        d.m_huff_count[0, LenSym[match_len - MinMatchLen]] += 1
    }

    mtd findMatch(lookahead_pos, max_dist, max_match_len: u32, pMatch_dist, pMatch_len: *u32)
    {
        d := state

        pos := lookahead_pos & LzDictSizeMask
        match_len := dref pMatch_len
        probe_pos := pos

        num_probes_left := d.maxProbes[match_len >= 32 ? 1 : 0]
        s  := &d.m_dict[0] + pos
        c0 := d.m_dict[pos + match_len]
        c1 := d.m_dict[pos + match_len - 1]

        if max_match_len <= match_len
            return

        loop
        {
            var next_probe_pos: u32
            var dist: u16

            loop
            {
                num_probes_left -= 1
                if num_probes_left == 0
                    return

                #[Swag.Mixin]
                func probe()
                {
                    next_probe_pos = d.m_next[probe_pos]
                    if !next_probe_pos
                        return
                    dist = cast(u16) (lookahead_pos - next_probe_pos)
                    if dist > max_dist
                        return
                    probe_pos = next_probe_pos & LzDictSizeMask
                    if (d.m_dict[probe_pos + match_len] == c0) and (d.m_dict[probe_pos + match_len - 1] == c1)
                        break
                }

                probe()
                probe()
                probe()
            }

            if !dist
                break

            p := s
            q := &d.m_dict[0] + probe_pos
            var probe_len: u32
            while probe_len < max_match_len
            {
                pp := dref p
                qq := dref q
                p, q += 1
                if pp != qq
                    break
                probe_len += 1
            }

            if probe_len > match_len
            {
                dref pMatch_dist = dist
                dref pMatch_len = probe_len
                match_len = probe_len
                if match_len == max_match_len
                    return
                c0 = d.m_dict[pos + match_len]
                c1 = d.m_dict[pos + match_len - 1]
            }
        }
    }

    //#[Swag.Macro]
    mtd putBits(b, l: u32)
    {
        d := state
        bits := b
        len  := l

        d.bitBuffer |= (bits << d.bitsIn)
        d.bitsIn += len
        while d.bitsIn >= 8
        {
            if d.outputBuf < d.outputBufEnd
            {
                dref d.outputBuf = cast(u8) d.bitBuffer
                d.outputBuf += 1
            }

            d.bitBuffer >>= 8
            d.bitsIn -= 8
        }
    }

    mtd radixSortSyms(numSyms: u32, pSyms0, pSyms1: *SymFreq)->*SymFreq
    {
        var hist: [256 * 2] u32
        loop i: numSyms
        {
            freq := pSyms0[i].key
            hist[freq & 0xFF] += 1
            hist[256 + ((freq >> 8) & 0xFF)] += 1
        }

        totalPasses := 2
        while (totalPasses > 1) and (numSyms == hist[(totalPasses - 1) * 256])
            totalPasses -= 1

        curSym    := pSyms0
        newSym    := pSyms1
        passShift := 0
        loop pass: totalPasses
        {
            pHist  := &hist[pass << 8]
            curOfs := 0

            var offsets: [256] u32 = ?
            loop i: 256
            {
                offsets[i] = curOfs
                curOfs += pHist[i]
            }

            loop i: numSyms
            {
                ptr := &offsets[(curSym[i].key >> passShift) & 0xFF]
                newSym[dref ptr] = curSym[i]
                dref ptr += 1
            }

            swap(&curSym, &newSym)
            passShift += 8
        }

        return curSym
    }

    mtd calculateMinimumRedundancy(symFreq: *SymFreq, n: u32)
    {
        if n == 0
            return

        if n == 1
        {
            symFreq[0].key = 1
            return
        }

        symFreq[0].key += symFreq[1].key
        root := 0
        leaf := 2
        for next := 1; next < n - 1; next += 1
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
        for next := n - 3; cast(s32) next >= 0; next -= 1
            symFreq[next].key = cast(u16) (symFreq[symFreq[next].key].key + 1)

        avbl := 1
        used := 0
        dpth := 0
        root = n - 2
        next := n - 1
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

    mtd huffmanEnforceMaxCodeSize(pNum_codes: *s32, code_list_len, max_code_size: u32)
    {
        if code_list_len <= 1
            return

        for i := max_code_size + 1; i <= MaxSupportedHuffCodeSize; i += 1
            pNum_codes[max_code_size] += pNum_codes[i]

        total := 0'u32
        for i := cast(s32) max_code_size; i > 0; i -= 1
            total += ((cast(u32) pNum_codes[i]) << (max_code_size - i))

        while total != (1'u32 << max_code_size)
        {
            pNum_codes[max_code_size] -= 1
            for i := cast(s32) max_code_size - 1; i > 0; i -= 1
            {
                if pNum_codes[i]
                {
                    pNum_codes[i] -= 1
                    pNum_codes[i + 1] += 2
                    break
                }
            }

            total -= 1
        }
    }

    mtd optimizeHuffmanTable(table_num, table_len, code_size_limit: u32, static_table: bool)
    {
        d := state

        var num_codes: [1 + MaxSupportedHuffCodeSize] s32
        var next_code: [MaxSupportedHuffCodeSize + 1] u32

        if static_table
        {
            for i := 0; i < table_len; i += 1
            {
                num_codes[d.m_huff_code_sizes[table_num, i]] += 1
            }
        }
        else
        {
            var syms0: [MaxHuffSymbols] SymFreq
            var syms1: [MaxHuffSymbols] SymFreq

            num_used_syms := 0'u32
            pSym_count := &d.m_huff_count[table_num, 0]

            for i := 0; i < table_len; i += 1
            {
                if pSym_count[i]
                {
                    syms0[num_used_syms].key = cast(u16) pSym_count[i]
                    syms0[num_used_syms].symIndex = cast(u16) i
                    num_used_syms += 1
                }
            }

            pSyms := radixSortSyms(num_used_syms, syms0, syms1)
            calculateMinimumRedundancy(pSyms, num_used_syms)

            for i := 0; i < num_used_syms; i += 1
            {
                num_codes[pSyms[i].key] += 1
            }

            huffmanEnforceMaxCodeSize(num_codes, num_used_syms, code_size_limit)

            Memory.clear(&d.m_huff_code_sizes[table_num], @sizeof(d.m_huff_code_sizes[table_num]))
            Memory.clear(&d.m_huff_codes[table_num], @sizeof(d.m_huff_codes[table_num]))

            for { i := 1; j := num_used_syms; } i <= code_size_limit; i += 1
            {
                for l := num_codes[i]; l > 0; l -= 1
                {
                    j -= 1
                    d.m_huff_code_sizes[table_num, pSyms[j].symIndex] = cast(u8) i
                }
            }
        }

        next_code[1] = 0
        j := 0
        for i := 2; i <= code_size_limit; i += 1
        {
            j = (j + num_codes[i - 1]) << 1
            next_code[i] = j
        }

        for i := 0; i < table_len; i += 1
        {
            rev_code := 0'u32
            code_size := d.m_huff_code_sizes[table_num, i]
            if code_size == 0
                continue
            cde := next_code[code_size]
            next_code[code_size] += 1
            for l := code_size; l > 0; l -= 1
            {
                rev_code = (rev_code << 1) | (cde & 1)
                cde >>= 1
            }

            d.m_huff_codes[table_num, i] = cast(u16) rev_code
        }
    }

    mtd startStaticBlock()
    {
        d := state
        p := &d.m_huff_code_sizes[0, 0]

        i := 0
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

        Memory.set(d.m_huff_code_sizes[1], 5, 32)

        optimizeHuffmanTable(0, 288, 15, true)
        optimizeHuffmanTable(1, 32, 15, true)

        putBits(1, 2)
    }

    #[Swag.Mixin]
    func rlePrevCodeSize()
    {
        if rle_repeat_count
        {
            d := state
            if rle_repeat_count < 3
            {
                d.m_huff_count[2, prev_code_size] = cast(u16) (d.m_huff_count[2, prev_code_size] + rle_repeat_count)
                while rle_repeat_count
                {
                    packed_code_sizes[num_packed_code_sizes] = prev_code_size
                    num_packed_code_sizes += 1
                    rle_repeat_count -= 1
                }
            }
            else
            {
                d.m_huff_count[2, 16] = cast(u16) (d.m_huff_count[2, 16] + 1)
                packed_code_sizes[num_packed_code_sizes] = 16
                num_packed_code_sizes += 1
                packed_code_sizes[num_packed_code_sizes] = cast(u8) (rle_repeat_count - 3)
                num_packed_code_sizes += 1
            }

            rle_repeat_count = 0
        }
    }

    #[Swag.Mixin]
    mtd rleZeroCodeSize()
    {
        if rle_z_count
        {
            if rle_z_count < 3
            {
                d.m_huff_count[2, 0] = cast(u16) (d.m_huff_count[2, 0] + rle_z_count)
                while rle_z_count
                {
                    packed_code_sizes[num_packed_code_sizes] = 0
                    num_packed_code_sizes += 1
                    rle_z_count -= 1
                }
            }
            elif rle_z_count <= 10
            {
                d.m_huff_count[2, 17] = cast(u16) (d.m_huff_count[2, 17] + 1)
                packed_code_sizes[num_packed_code_sizes] = 17
                num_packed_code_sizes += 1
                packed_code_sizes[num_packed_code_sizes] = cast(u8) (rle_z_count - 3)
                num_packed_code_sizes += 1
            }
            else
            {
                d.m_huff_count[2, 18] = cast(u16) (d.m_huff_count[2, 18] + 1)
                packed_code_sizes[num_packed_code_sizes] = 18
                num_packed_code_sizes += 1
                packed_code_sizes[num_packed_code_sizes] = cast(u8) (rle_z_count - 11)
                num_packed_code_sizes += 1

            }

            rle_z_count = 0
        }
    }

    mtd startDynamicBlock()
    {
        d := state

        var code_sizes_to_pack: [MaxHuffSymbols0 + MaxHuffSymbols1] u8 = ?
        var packed_code_sizes:  [MaxHuffSymbols0 + MaxHuffSymbols1] u8 = ?
        prev_code_size := 0xFF'u8

        d.m_huff_count[0, 256] = 1;

        optimizeHuffmanTable(0, MaxHuffSymbols0, 15, false)
        optimizeHuffmanTable(1, MaxHuffSymbols1, 15, false)

        num_lit_codes := 286'u32
        while num_lit_codes > 257
        {
            if d.m_huff_code_sizes[0, num_lit_codes - 1]
                break
            num_lit_codes -= 1
        }

        num_dist_codes := 30'u32
        while num_dist_codes > 1
        {
            if d.m_huff_code_sizes[1, num_dist_codes - 1]
                break
            num_dist_codes -= 1
        }

        Memory.copy(&code_sizes_to_pack[0], &d.m_huff_code_sizes[0, 0], cast(uint) num_lit_codes)
        Memory.copy(&code_sizes_to_pack[0] + num_lit_codes, &d.m_huff_code_sizes[1, 0], cast(uint) num_dist_codes)
        total_code_sizes_to_pack := num_lit_codes + num_dist_codes
        num_packed_code_sizes := 0
        rle_z_count := 0
        rle_repeat_count := 0

        Memory.clear(&d.m_huff_count[2, 0], cast(uint) @sizeof(d.m_huff_count[2, 0]) * MaxHuffSymbols2)
        for i := 0; i < total_code_sizes_to_pack; i += 1
        {
            code_size := code_sizes_to_pack[i]
            if !code_size
            {
                rlePrevCodeSize()
                rle_z_count += 1
                if rle_z_count == 138
                {
                    rleZeroCodeSize()
                }
            }
            else
            {
                rleZeroCodeSize()
                if code_size != prev_code_size
                {
                    rlePrevCodeSize()
                    d.m_huff_count[2, code_size] = cast(u16) (d.m_huff_count[2, code_size] + 1)
                    packed_code_sizes[num_packed_code_sizes] = code_size
                    num_packed_code_sizes += 1
                }
                else
                {
                    rle_repeat_count += 1
                    if rle_repeat_count == 6
                    {
                        rlePrevCodeSize()
                    }
                }
            }

            prev_code_size = code_size
        }

        if rle_repeat_count
        {
            rlePrevCodeSize()
        }
        else
        {
            rleZeroCodeSize()
        }

        optimizeHuffmanTable(2, MaxHuffSymbols2, 7, false)

        putBits(2, 2)
        putBits(num_lit_codes - 257, 5)
        putBits(num_dist_codes - 1, 5)

        num_bit_lengths := 18'u32
        while num_bit_lengths >= 0
        {
            if d.m_huff_code_sizes[2, PackedCodeSizeSymsSwizzle[num_bit_lengths]]
                break
            num_bit_lengths -= 1
        }

        num_bit_lengths = Math.max(4, num_bit_lengths + 1)
        putBits(num_bit_lengths - 4, 4)
        for i := 0; i < num_bit_lengths; i += 1
        {
            putBits(cast(u32) d.m_huff_code_sizes[2, PackedCodeSizeSymsSwizzle[i]], 3)
        }

        packed_code_sizes_index := 0
        while packed_code_sizes_index < num_packed_code_sizes
        {
            cde := packed_code_sizes[packed_code_sizes_index]
            packed_code_sizes_index += 1
            putBits(cast(u32) d.m_huff_codes[2, cde], cast(u32) d.m_huff_code_sizes[2, cde])
            if cde >= 16
            {
                const p = @[2, 3, 7]
                putBits(cast(u32) packed_code_sizes[packed_code_sizes_index], cast(u32) p[cde - 16])
                packed_code_sizes_index += 1
            }
        }
    }

    mtd compressLzCodes()->bool
    {
        d := state
        flags := 1'u32
        for pLZ_codes := &d.m_lz_code_buf[0]; pLZ_codes < d.lzCodeBuf; flags >>= 1
        {
            if flags == 1
            {
                flags = cast(u32) (dref pLZ_codes) | 0x100
                pLZ_codes += 1
            }

            if flags & 1
            {
                match_len  := cast(u32) pLZ_codes[0]
                match_dist := cast(u32) pLZ_codes[1] | (cast(u32) pLZ_codes[2] << 8)
                pLZ_codes += 3

                putBits(cast(u32) d.m_huff_codes[0, LenSym[match_len]], cast(u32) d.m_huff_code_sizes[0, LenSym[match_len]])
                putBits(match_len & Bitmasks[LenExtra[match_len]], cast(u32) LenExtra[match_len])

                var sym, num_extra_bits: u32
                if match_dist < 512
                {
                    sym = SmallDistSym[match_dist]
                    num_extra_bits = SmallDistExtra[match_dist]
                }
                else
                {
                    sym = LargeDistSym[match_dist >> 8]
                    num_extra_bits = LargeDistExtra[match_dist >> 8]
                }

                putBits(cast(u32) d.m_huff_codes[1, sym], cast(u32) d.m_huff_code_sizes[1, sym])
                putBits(match_dist & Bitmasks[num_extra_bits], num_extra_bits)
            }
            else
            {
                lit := dref pLZ_codes
                pLZ_codes += 1
                putBits(cast(u32) d.m_huff_codes[0, lit], cast(u32) d.m_huff_code_sizes[0, lit])
            }
        }

        putBits(cast(u32) d.m_huff_codes[0, 256], cast(u32) d.m_huff_code_sizes[0, 256])
        return d.outputBuf < d.outputBufEnd
    }

    mtd compressBlock(static_block: bool)->bool
    {
        if static_block
            startStaticBlock()
        else
            startDynamicBlock()
        return compressLzCodes()
    }

    mtd flushBlock(finish: bool)
    {
        d := state

        comp_block_succeeded := false
        use_raw_block := ((d.m_flags & ForceAllRawBlocks) != 0) and (d.lookaheadPos - d.lzCodeBufDictPos) <= d.dictSize

        var pOutput_buf_start: *u8
        if d.result.capacity - d.result.count >= OutBufSize
            pOutput_buf_start = d.result.buffer + d.outBufOfs
        else
            pOutput_buf_start = &d.m_output_buf[0]

        d.outputBuf = pOutput_buf_start
        d.outputBufEnd = d.outputBuf + OutBufSize - 16

        d.outputFlushOfs = 0

        dref d.lzFlags = cast(u8) (dref d.lzFlags >> d.numFlagsLeft)
        d.lzCodeBuf -= (d.numFlagsLeft == 8) ? 1 : 0

        putBits(finish ? 1 : 0, 1)

        pSaved_output_buf := d.outputBuf
        saved_bit_buf := d.bitBuffer
        saved_bits_in := d.bitsIn

        if !use_raw_block
            comp_block_succeeded = compressBlock((d.m_flags & ForceAllStaticBlocks) or (d.totalLzBytes < 48))

        if ((use_raw_block or (d.totalLzBytes and ((d.outputBuf - pSaved_output_buf + 1) >= d.totalLzBytes))) and
            ((d.lookaheadPos - d.lzCodeBufDictPos) <= d.dictSize))
        {
            d.outputBuf = pSaved_output_buf
            d.bitBuffer = saved_bit_buf
            d.bitsIn = saved_bits_in
            putBits(0, 2)
            if d.bitsIn
            {
                putBits(0, 8 - d.bitsIn)
            }

            for i := 2; i != 0; i -= 1
            {
                putBits(d.totalLzBytes & 0xFFFF, 16)
                d.totalLzBytes ^= 0xFFFF
            }

            for i := 0; i < d.totalLzBytes; i += 1
            {
                putBits(cast(u32) d.m_dict[(d.lzCodeBufDictPos + i) & LzDictSizeMask], 8)
            }
        }
        elif !comp_block_succeeded
        {
            d.outputBuf = pSaved_output_buf
            d.bitBuffer = saved_bit_buf
            d.bitsIn = saved_bits_in
            discard compressBlock(true)
        }

        if finish and d.bitsIn
            putBits(0, 8 - d.bitsIn)

        Memory.clear(&d.m_huff_count[0, 0], cast(uint) @sizeof(d.m_huff_count[0, 0]) * MaxHuffSymbols0)
        Memory.clear(&d.m_huff_count[1, 0], cast(uint) @sizeof(d.m_huff_count[1, 0]) * MaxHuffSymbols1)

        d.lzCodeBuf = &d.m_lz_code_buf[0] + 1
        d.lzFlags = &d.m_lz_code_buf[0]
        d.numFlagsLeft = 8
        d.lzCodeBufDictPos += d.totalLzBytes
        d.totalLzBytes = 0
        d.blockIndex += 1

        n := cast(uint) (d.outputBuf - pOutput_buf_start)
        if n != 0
        {
            if (pOutput_buf_start == &d.m_output_buf[0])
            {
                d.result.resize(d.result.count + n)
                Memory.copy(d.result.buffer + d.outBufOfs, d.m_output_buf, cast(uint) n)
                d.outBufOfs += n
            }
            else
            {
                d.outBufOfs += n
                d.result.count += n
            }
        }
    }

    mtd compress() throw
    {
        d := state
        pSrc := d.inSrc
        src_buf_left := d.srcBufLeft

        while src_buf_left or d.lookaheadSize
        {
            if (d.lookaheadSize + d.dictSize) >= (MinMatchLen - 1)
            {
                var dst_pos: u32 = (d.lookaheadPos + d.lookaheadSize) & LzDictSizeMask
                var ins_pos: u32 = d.lookaheadPos + d.lookaheadSize - 2
                var hash:    u32 = (cast(u32) d.m_dict[ins_pos & LzDictSizeMask] << LzHashShift)
                hash ^= d.m_dict[(ins_pos + 1) & LzDictSizeMask]

                num_bytes_to_process := cast(u32) Math.min(cast(u32) src_buf_left, cast(u32) (MaxMatchLen - d.lookaheadSize))

                pSrc_end := pSrc ? pSrc + num_bytes_to_process : null
                src_buf_left -= num_bytes_to_process
                d.lookaheadSize += num_bytes_to_process

                while pSrc != pSrc_end
                {
                    c := dref pSrc
                    pSrc += 1
                    d.m_dict[dst_pos] = c;
                    if dst_pos < (MaxMatchLen - 1)
                        d.m_dict[LzDictSize + dst_pos] = c
                    hash = ((hash << LzHashShift) ^ c) & (LzHashSize - 1)
                    d.m_next[ins_pos & LzDictSizeMask] = d.m_hash[hash]
                    d.m_hash[hash] = cast(u16) ins_pos
                    dst_pos = (dst_pos + 1) & LzDictSizeMask
                    ins_pos += 1
                }
            }
            else
            {
                while src_buf_left and d.lookaheadSize < MaxMatchLen
                {
                    c := dref pSrc
                    pSrc += 1
                    dst_pos := (d.lookaheadPos + d.lookaheadSize) & LzDictSizeMask
                    src_buf_left -= 1
                    d.m_dict[dst_pos] = c
                    if dst_pos < (MaxMatchLen - 1)
                        d.m_dict[LzDictSize + dst_pos] = c
                    d.lookaheadSize += 1
                    if (d.lookaheadSize + d.dictSize) >= MinMatchLen
                    {
                        var ins_pos: u32 = d.lookaheadPos + (d.lookaheadSize - 1) - 2
                        var hash: u32 = (cast(u32) d.m_dict[ins_pos & LzDictSizeMask] << (LzHashShift * 2))
                        hash = (hash ^ (cast(u32) d.m_dict[(ins_pos + 1) & LzDictSizeMask] << LzHashShift) ^ c)
                        hash &= (LzHashSize - 1)
                        d.m_next[ins_pos & LzDictSizeMask] = d.m_hash[hash]
                        d.m_hash[hash] = cast(u16) ins_pos
                    }
                }
            }

            d.dictSize = Math.min(cast(u32) (LzDictSize - d.lookaheadSize), d.dictSize)

            len_to_move := 1'u32
            cur_match_dist := 0'u32
            cur_match_len := d.savedMatchLen ? d.savedMatchLen : cast(u32) (MinMatchLen - 1)
            var cur_pos: u32 = d.lookaheadPos & LzDictSizeMask
            if d.m_flags & (RleMatches | ForceAllRawBlocks)
            {
                if (d.dictSize) and !(d.m_flags & ForceAllRawBlocks)
                {
                    c := d.m_dict[(cur_pos - 1) & LzDictSizeMask]
                    cur_match_len = 0
                    while cur_match_len < d.lookaheadSize
                    {
                        if d.m_dict[cur_pos + cur_match_len] != c
                            break
                        cur_match_len += 1
                    }
                    if cur_match_len < MinMatchLen
                        cur_match_len = 0
                    else
                        cur_match_dist = 1
                }
            }
            else
            {
                findMatch(d.lookaheadPos, d.dictSize, d.lookaheadSize, &cur_match_dist, &cur_match_len)
            }

            if  ((cur_match_len == MinMatchLen) and (cur_match_dist >= 8 * 1024)) or
                (cur_pos == cur_match_dist) or
                ((d.m_flags & FilterMatches) and (cur_match_len <= 5))
            {
                cur_match_dist, cur_match_len = 0
            }

            if d.savedMatchLen
            {
                if cur_match_len > d.savedMatchLen
                {
                    recordLiteral(cast(u8) d.savedLit)
                    if cur_match_len >= 128
                    {
                        recordMatch(cur_match_len, cur_match_dist)
                        d.savedMatchLen = 0
                        len_to_move = cur_match_len
                    }
                    else
                    {
                        d.savedLit = d.m_dict[cur_pos]
                        d.savedMatchDist = cur_match_dist
                        d.savedMatchLen = cur_match_len
                    }
                }
                else
                {
                    recordMatch(d.savedMatchLen, d.savedMatchDist)
                    len_to_move = d.savedMatchLen - 1;
                    d.savedMatchLen = 0;
                }
            }
            elif !cur_match_dist
            {
                recordLiteral(d.m_dict[Math.min(cur_pos, cast(u32) @sizeof(d.m_dict) - 1)])
            }
            elif d.greedyParsing or (d.m_flags & RleMatches) or (cur_match_len >= 128)
            {
                recordMatch(cur_match_len, cur_match_dist)
                len_to_move = cur_match_len
            }
            else
            {
                d.savedLit = d.m_dict[Math.min(cur_pos, cast(u32) @sizeof(d.m_dict) - 1)]
                d.savedMatchDist = cur_match_dist
                d.savedMatchLen = cur_match_len
            }

            d.lookaheadPos += len_to_move
            Debug.assert(d.lookaheadSize >= len_to_move)
            d.lookaheadSize -= len_to_move
            d.dictSize = Math.min(d.dictSize + len_to_move, cast(u32) LzDictSize)

            if (d.lzCodeBuf > &d.m_lz_code_buf[LzCodeBufSize - 8]) or
                ((d.totalLzBytes > 31 * 1024) and ((((cast(u32) (d.lzCodeBuf - &d.m_lz_code_buf[0]) * 115) >> 7) >= d.totalLzBytes) or (d.m_flags & ForceAllRawBlocks)))
            {
                d.inSrc = pSrc
                d.srcBufLeft = src_buf_left
                flushBlock(false)
            }
        }

        d.inSrc = pSrc
        d.srcBufLeft = src_buf_left
    }
}

public impl Deflate
{
    #[Swag.EnumFlags]
    enum CompressionFlags
    {
        Zero = 0
        ComputeAdler32
        Default = ComputeAdler32
    }

    #[Swag.Incomplete]
    enum CompressionLevel : u32
    {
        NoCompression   = 0
        BestSpeed       = 1
        Default         = 6
        BestCompression = 9
        UberCompression = 10
    }

    enum CompressionStrategy
    {
        Default
        Filtered
        HuffmanOnly
        Rle
        Fixed
    }

    mtd opDrop()
    {
        Memory.delete(state)
    }

    // Initialize the compressor
    // Can be called multiple times
    mtd init(level = CompressionLevel.Default, strategy = CompressionStrategy.Default, flags = CompressionFlags.Default)
    {
        if !state
            state = Memory.new'Compressor()
        else
            @init(state)
        setup(createCompFlagsFromZipParams(level, strategy))
        initFlags = flags
    }

    // Compress the source buffer
    mtd compress(result: *Array'u8, src: const [..] u8) throw
    {
        if @countof(src) > Swag.U32.Max
            throw "source buffer too big"

        Debug.assert(state  != null, "'init' not called")

        state.result         = result
        state.inBuf      = @dataof(src)
        state.inSrc         = @dataof(src)
        state.srcBufLeft = @countof(src)
        state.outBufOfs  = result.count

        compress()
        flushBlock(true)
        Debug.assert(!state.lookaheadSize and !state.srcBufLeft)

        if initFlags & .ComputeAdler32
        {
            adler.init()
            adler.update(src)
        }
    }
}
