/*
    :LICENCE
    This code is based on the miniz library (https://github.com/richgel999/miniz)
    The original code has been modified for Swag
    See LICENCE.md for the corresponding licence.
*/

#global namespace Compress
#global #[Swag.Safety("overflow", false)]

#[Swag.Opaque]
public struct Deflate
{
    stream: Stream
    state:  tdefl_compressor
}

struct Stream
{
    next_in:    const *u8
    avail_in:   uint
    total_in:   uint
    next_out:   *u8
    avail_out:  uint
    total_out:  uint
    adler:      u32
}

struct tdefl_sym_freq
{
    m_key, m_sym_index: u16
}

const TDEFL_MAX_SUPPORTED_HUFF_CODESIZE = 32

const s_tdefl_packed_code_size_syms_swizzle: [] u8 = @[
    16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15
]

const mz_bitmasks: [17] u32 = @[
    0x0000, 0x0001, 0x0003, 0x0007, 0x000F, 0x001F, 0x003F, 0x007F, 0x00FF, 0x01FF, 0x03FF, 0x07FF, 0x0FFF, 0x1FFF, 0x3FFF, 0x7FFF, 0xFFFF
]

const s_tdefl_len_extra: [256] u8 = @[
    0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 0
]

const s_tdefl_len_sym: [256] u16 = @[
    257, 258, 259, 260, 261, 262, 263, 264, 265, 265, 266, 266, 267, 267, 268, 268, 269, 269, 269, 269, 270, 270, 270, 270, 271, 271, 271, 271, 272, 272, 272, 272,
    273, 273, 273, 273, 273, 273, 273, 273, 274, 274, 274, 274, 274, 274, 274, 274, 275, 275, 275, 275, 275, 275, 275, 275, 276, 276, 276, 276, 276, 276, 276, 276,
    277, 277, 277, 277, 277, 277, 277, 277, 277, 277, 277, 277, 277, 277, 277, 277, 278, 278, 278, 278, 278, 278, 278, 278, 278, 278, 278, 278, 278, 278, 278, 278,
    279, 279, 279, 279, 279, 279, 279, 279, 279, 279, 279, 279, 279, 279, 279, 279, 280, 280, 280, 280, 280, 280, 280, 280, 280, 280, 280, 280, 280, 280, 280, 280,
    281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281, 281,
    282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282, 282,
    283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283, 283,
    284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 284, 285
]

const s_tdefl_small_dist_sym: [512] u8 = @[
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

const s_tdefl_small_dist_extra: [512] u8 = @[
    0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
    6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
    6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7
]

const s_tdefl_large_dist_extra: [128] u8 = @[
    0, 0, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
    12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
    13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13
]

const s_tdefl_large_dist_sym: [128] u8 = @[
    0, 0, 18, 19, 20, 20, 21, 21, 22, 22, 22, 22, 23, 23, 23, 23, 24, 24, 24, 24, 24, 24, 24, 24, 25, 25, 25, 25, 25, 25, 25, 25, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26,
    26, 26, 26, 26, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
    28, 28, 28, 28, 28, 28, 28, 28, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29
]

enum tdefl_status
{
    TDEFL_STATUS_BAD_PARAM = -2
    TDEFL_STATUS_PUT_BUF_FAILED = -1
    TDEFL_STATUS_OKAY = 0
    TDEFL_STATUS_DONE = 1
}

enum tdefl_flush
{
    TDEFL_NO_FLUSH = 0
    TDEFL_SYNC_FLUSH = 2
    TDEFL_FULL_FLUSH = 3
    TDEFL_FINISH = 4
}

const MZ_NO_FLUSH = 0
const MZ_PARTIAL_FLUSH = 1
const MZ_SYNC_FLUSH = 2
const MZ_FULL_FLUSH = 3
const MZ_FINISH = 4
const MZ_BLOCK = 5

const TDEFL_MAX_HUFF_TABLES = 3
const TDEFL_MAX_HUFF_SYMBOLS_0 = 288'u32
const TDEFL_MAX_HUFF_SYMBOLS_1 = 32'u32
const TDEFL_MAX_HUFF_SYMBOLS_2 = 19'u32
const TDEFL_LZ_DICT_SIZE = 32768
const TDEFL_LZ_DICT_SIZE_MASK = TDEFL_LZ_DICT_SIZE - 1
const TDEFL_MIN_MATCH_LEN = 3
const TDEFL_MAX_MATCH_LEN = 258

const TDEFL_LZ_CODE_BUF_SIZE = 64 * 1024
const TDEFL_OUT_BUF_SIZE = (TDEFL_LZ_CODE_BUF_SIZE * 13) / 10
const TDEFL_MAX_HUFF_SYMBOLS = 288
const TDEFL_LZ_HASH_BITS = 15
const TDEFL_LEVEL1_HASH_SIZE_MASK = 4095
const TDEFL_LZ_HASH_SHIFT = (TDEFL_LZ_HASH_BITS + 2) / 3
const TDEFL_LZ_HASH_SIZE = 1 << TDEFL_LZ_HASH_BITS

struct tdefl_compressor
{
    m_flags:            u32
    m_max_probes:       [2] u32
    m_greedy_parsing:   bool
    m_adler32:          u32
    m_lookahead_pos:    u32
    m_lookahead_size:   u32
    m_dict_size:        u32
    m_pLZ_code_buf:     *u8
    m_pLZ_flags:        *u8
    m_pOutput_buf:      *u8
    m_pOutput_buf_end:  *u8
    m_num_flags_left:   u32
    m_total_lz_bytes:   u32
    m_lz_code_buf_dict_pos:   u32
    m_bits_in:          u32
    m_bit_buffer:       u32
    m_saved_match_dist:   u32
    m_saved_match_len:   u32
    m_saved_lit:   u32
    m_output_flush_ofs:   u32
    m_output_flush_remaining:   u32
    m_finished:   u32
    m_block_index:   u32
    m_wants_to_finish:   u32
    m_pIn_buf:      const *u8
    m_pOut_buf:   *u8
    m_pIn_buf_size: *uint
    m_pOut_buf_size: *uint
    m_flush:    tdefl_flush
    m_pSrc: const *u8
    m_src_buf_left: uint
    m_out_buf_ofs: uint
    m_dict:             [TDEFL_LZ_DICT_SIZE + TDEFL_MAX_MATCH_LEN - 1] u8
    m_huff_count:       [TDEFL_MAX_HUFF_TABLES, TDEFL_MAX_HUFF_SYMBOLS] u16
    m_huff_codes:       [TDEFL_MAX_HUFF_TABLES, TDEFL_MAX_HUFF_SYMBOLS] u16
    m_huff_code_sizes:  [TDEFL_MAX_HUFF_TABLES, TDEFL_MAX_HUFF_SYMBOLS] u8
    m_lz_code_buf:      [TDEFL_LZ_CODE_BUF_SIZE] u8
    m_next:             [TDEFL_LZ_DICT_SIZE] u16
    m_hash:             [TDEFL_LZ_HASH_SIZE] u16
    m_output_buf:       [TDEFL_OUT_BUF_SIZE] u8
    result:             *Array'u8
}

const TDEFL_WRITE_ZLIB_HEADER = 0x01000
const TDEFL_COMPUTE_ADLER32 = 0x02000
const TDEFL_GREEDY_PARSING_FLAG = 0x04000
const TDEFL_NONDETERMINISTIC_PARSING_FLAG = 0x08000
const TDEFL_RLE_MATCHES = 0x10000
const TDEFL_FILTER_MATCHES = 0x20000
const TDEFL_FORCE_ALL_STATIC_BLOCKS = 0x40000
const TDEFL_FORCE_ALL_RAW_BLOCKS = 0x80000

const TDEFL_HUFFMAN_ONLY = 0
const TDEFL_DEFAULT_MAX_PROBES = 128
const TDEFL_MAX_PROBES_MASK = 0xFFF

const s_tdefl_num_probes: [11] u32 = @[ 0, 1, 6, 32, 16, 32, 128, 256, 512, 768, 1500 ]

impl Deflate
{
    func tdefl_create_comp_flags_from_zip_params(level: u32, window_bits: s32, strategy: CompressionStrategy)->u32
    {
        comp_flags := 0'u32
        if level != MZ_DEFAULT_COMPRESSION
            comp_flags = s_tdefl_num_probes[Math.min(10, cast(s32) level)]
        else
            comp_flags = s_tdefl_num_probes[MZ_DEFAULT_LEVEL]
        if level <= 3
            comp_flags |= TDEFL_GREEDY_PARSING_FLAG
        if window_bits > 0
            comp_flags |= TDEFL_WRITE_ZLIB_HEADER

        if level == MZ_NO_COMPRESSION
            comp_flags |= TDEFL_FORCE_ALL_RAW_BLOCKS
        else if strategy == .MZ_FILTERED
            comp_flags |= TDEFL_FILTER_MATCHES
        else if strategy ==. MZ_HUFFMAN_ONLY
            comp_flags &= ~TDEFL_MAX_PROBES_MASK
        else if strategy == .MZ_FIXED
            comp_flags |= TDEFL_FORCE_ALL_STATIC_BLOCKS
        else if strategy == .MZ_RLE
            comp_flags |= TDEFL_RLE_MATCHES

        return comp_flags
    }

    mtd mz_deflateInit2(level: u32, method: s32, window_bits: s32, mem_level: s32, strategy: CompressionStrategy) throw
    {
        comp_flags := TDEFL_COMPUTE_ADLER32 | tdefl_create_comp_flags_from_zip_params(level, window_bits, strategy)
        if (method != MZ_DEFLATED or mem_level < 1 or mem_level > 9 or (window_bits != MZ_DEFAULT_WINDOW_BITS and -window_bits != MZ_DEFAULT_WINDOW_BITS))
            throw "MZ_PARAM_ERROR"
        tdefl_init(comp_flags)
    }

    mtd tdefl_init(flags: u32)
    {
        d := &state
        @init(d)
        d.m_flags = flags
        d.m_max_probes[0] = 1 + ((flags & 0xFFF) + 2) / 3
        d.m_greedy_parsing = (flags & TDEFL_GREEDY_PARSING_FLAG) != 0
        d.m_max_probes[1] = 1 + (((flags & 0xFFF) >> 2) + 2) / 3
        d.m_pLZ_code_buf = &d.m_lz_code_buf[1]
        d.m_pLZ_flags = &d.m_lz_code_buf[0]
        d.m_num_flags_left = 8
        d.m_pOutput_buf = d.m_output_buf
        d.m_pOutput_buf_end = d.m_output_buf
        d.m_adler32 = 1
        d.m_flush = .TDEFL_NO_FLUSH
    }

    mtd tdefl_flush_output_buffer()->tdefl_status
    {
        d := &state
        if d.m_pIn_buf_size
            dref d.m_pIn_buf_size = cast(uint) (d.m_pSrc - cast(const *u8) d.m_pIn_buf)

        if d.m_pOut_buf_size
        {
            n := Math.min(cast(u32) (dref d.m_pOut_buf_size - d.m_out_buf_ofs), cast(u32) d.m_output_flush_remaining)

            d.result.resize(d.result.count + n)
            d.m_pOut_buf = d.result.buffer

            Memory.copy(d.m_pOut_buf + d.m_out_buf_ofs, &d.m_output_buf[0] + d.m_output_flush_ofs, cast(uint) n)
            d.m_output_flush_ofs += n
            d.m_output_flush_remaining -= n
            d.m_out_buf_ofs += n

            dref d.m_pOut_buf_size = d.m_out_buf_ofs
        }

        return d.m_finished and !d.m_output_flush_remaining ? .TDEFL_STATUS_DONE : .TDEFL_STATUS_OKAY
    }

    mtd tdefl_record_literal(lit: u8)
    {
        d := &state

        d.m_total_lz_bytes += 1
        dref d.m_pLZ_code_buf = lit
        d.m_pLZ_code_buf += 1

        dref d.m_pLZ_flags = cast(u8) (dref d.m_pLZ_flags >>,safe 1)
        d.m_num_flags_left -= 1

        if d.m_num_flags_left
        {
            d.m_num_flags_left = 8
            d.m_pLZ_flags = d.m_pLZ_code_buf
            d.m_pLZ_code_buf += 1
        }

        d.m_huff_count[0, lit] += 1
    }

    mtd tdefl_record_match(match_len: u32, md: u32)
    {
        match_dist := md
        d := &state

        d.m_total_lz_bytes += match_len

        d.m_pLZ_code_buf[0] = cast(u8) (match_len - TDEFL_MIN_MATCH_LEN)

        match_dist -= 1
        d.m_pLZ_code_buf[1] = cast(u8) (match_dist & 0xFF)
        d.m_pLZ_code_buf[2] = cast(u8) (match_dist >>,safe 8)
        d.m_pLZ_code_buf += 3

        dref d.m_pLZ_flags = cast(u8) ((dref d.m_pLZ_flags >>,safe 1) | 0x80)
        d.m_num_flags_left -= 1
        if d.m_num_flags_left == 0
        {
            d.m_num_flags_left = 8
            d.m_pLZ_flags = d.m_pLZ_code_buf
            d.m_pLZ_code_buf += 1
        }

        s0 := s_tdefl_small_dist_sym[match_dist & 511]
        s1 := s_tdefl_large_dist_sym[(match_dist >>,safe 8) & 127]
        d.m_huff_count[1, (match_dist < 512) ? s0 : s1] += 1
        d.m_huff_count[0, s_tdefl_len_sym[match_len - TDEFL_MIN_MATCH_LEN]] += 1
    }

    mtd tdefl_find_match(lookahead_pos, max_dist, max_match_len: u32, pMatch_dist, pMatch_len: *u32)
    {
        d := &state

        pos := lookahead_pos & TDEFL_LZ_DICT_SIZE_MASK
        match_len := dref pMatch_len
        probe_pos := pos

        num_probes_left := d.m_max_probes[match_len >= 32 ? 1 : 0]
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
                func TDEFL_PROBE()
                {
                    next_probe_pos = d.m_next[probe_pos]
                    if !next_probe_pos
                        return
                    dist = cast(u16) (lookahead_pos - next_probe_pos)
                    if dist > max_dist
                        return
                    probe_pos = next_probe_pos & TDEFL_LZ_DICT_SIZE_MASK
                    if (d.m_dict[probe_pos + match_len] == c0) and (d.m_dict[probe_pos + match_len - 1] == c1)
                        break
                }

                TDEFL_PROBE()
                TDEFL_PROBE()
                TDEFL_PROBE()
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
    mtd TDEFL_PUT_BITS(b, l: u32)
    {
        d := &state
        bits := b
        len  := l

        d.m_bit_buffer |= (bits << d.m_bits_in)
        d.m_bits_in += len
        while d.m_bits_in >= 8
        {
            if d.m_pOutput_buf < d.m_pOutput_buf_end
            {
                dref d.m_pOutput_buf = cast(u8) d.m_bit_buffer
                d.m_pOutput_buf += 1
            }

            d.m_bit_buffer >>= 8
            d.m_bits_in -= 8
        }
    }

    mtd tdefl_radix_sort_syms(num_syms: u32, pSyms0, pSyms1: *tdefl_sym_freq)->*tdefl_sym_freq
    {
        total_passes := 2
        var hist: [256 * 2] u32

        pCur_syms := pSyms0
        pNew_syms := pSyms1
        for i := 0; i < num_syms; i += 1
        {
            freq := pSyms0[i].m_key
            hist[freq & 0xFF] += 1
            hist[256 + ((freq >> 8) & 0xFF)] += 1
        }

        while (total_passes > 1) and (num_syms == hist[(total_passes - 1) * 256])
            total_passes -= 1

        pass_shift := 0
        for pass := 0; pass < total_passes; pass += 1
        {
            pHist := &hist[pass << 8]
            var offsets: [256] u32
            cur_ofs := 0

            for i := 0; i < 256; i += 1
            {
                offsets[i] = cur_ofs
                cur_ofs += pHist[i]
            }

            for i := 0; i < num_syms; i += 1
            {
                ptr := &offsets[(pCur_syms[i].m_key >> pass_shift) & 0xFF]
                pNew_syms[dref ptr] = pCur_syms[i]
                dref ptr += 1
            }

            swap(&pCur_syms, &pNew_syms)
            pass_shift += 8
        }

        return pCur_syms;
    }

    mtd tdefl_calculate_minimum_redundancy(A: *tdefl_sym_freq, n: u32)
    {
        if n == 0
            return
        elif n == 1
        {
            A[0].m_key = 1
            return
        }

        A[0].m_key += A[1].m_key
        root := 0
        leaf := 2
        for next := 1; next < n - 1; next += 1
        {
            if leaf >= n or A[root].m_key < A[leaf].m_key
            {
                A[next].m_key = A[root].m_key
                A[root].m_key = cast(u16) next
                root += 1
            }
            else
            {
                A[next].m_key = A[leaf].m_key
                leaf += 1
            }

            if leaf >= n or (root < next and A[root].m_key < A[leaf].m_key)
            {
                A[next].m_key = cast(u16) (A[next].m_key + A[root].m_key)
                A[root].m_key = cast(u16) next
                root += 1
            }
            else
            {
                A[next].m_key = cast(u16) (A[next].m_key + A[leaf].m_key)
                leaf += 1
            }
        }

        A[n - 2].m_key = 0
        for next := n - 3; cast(s32) next >= 0; next -= 1
            A[next].m_key = cast(u16) (A[A[next].m_key].m_key + 1)

        avbl := 1
        used := 0
        dpth := 0
        root = n - 2
        next := n - 1
        while avbl > 0
        {
            while root >= 0 and cast(s32) A[root].m_key == dpth
            {
                used += 1
                root -= 1
            }

            while avbl > used
            {
                A[next].m_key = cast(u16) dpth
                next -= 1
                avbl -= 1
            }

            avbl = 2 * used
            dpth += 1
            used = 0
        }
    }

    mtd tdefl_huffman_enforce_max_code_size(pNum_codes: *s32, code_list_len, max_code_size: u32)
    {
        total := 0'u32
        if code_list_len <= 1
            return

        for i := max_code_size + 1; i <= TDEFL_MAX_SUPPORTED_HUFF_CODESIZE; i += 1
            pNum_codes[max_code_size] += pNum_codes[i]
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

    mtd tdefl_optimize_huffman_table(table_num, table_len, code_size_limit: u32, static_table: bool)
    {
        d := &state

        var num_codes: [1 + TDEFL_MAX_SUPPORTED_HUFF_CODESIZE] s32
        var next_code: [TDEFL_MAX_SUPPORTED_HUFF_CODESIZE + 1] u32

        if static_table
        {
            for i := 0; i < table_len; i += 1
            {
                num_codes[d.m_huff_code_sizes[table_num, i]] += 1
            }
        }
        else
        {
            var syms0: [TDEFL_MAX_HUFF_SYMBOLS] tdefl_sym_freq
            var syms1: [TDEFL_MAX_HUFF_SYMBOLS] tdefl_sym_freq

            num_used_syms := 0'u32
            pSym_count := &d.m_huff_count[table_num, 0]

            for i := 0; i < table_len; i += 1
            {
                if pSym_count[i]
                {
                    syms0[num_used_syms].m_key = cast(u16) pSym_count[i]
                    syms0[num_used_syms].m_sym_index = cast(u16) i
                    num_used_syms += 1
                }
            }

            pSyms := tdefl_radix_sort_syms(num_used_syms, syms0, syms1)
            tdefl_calculate_minimum_redundancy(pSyms, num_used_syms)

            for i := 0; i < num_used_syms; i += 1
            {
                num_codes[pSyms[i].m_key] += 1
            }

            tdefl_huffman_enforce_max_code_size(num_codes, num_used_syms, code_size_limit)

            Memory.clear(&d.m_huff_code_sizes[table_num], @sizeof(d.m_huff_code_sizes[table_num]))
            Memory.clear(&d.m_huff_codes[table_num], @sizeof(d.m_huff_codes[table_num]))

            j := num_used_syms
            for i := 1; i <= code_size_limit; i += 1
            {
                for l := num_codes[i]; l > 0; l -= 1
                {
                    j -= 1
                    d.m_huff_code_sizes[table_num, pSyms[j].m_sym_index] = cast(u8) i
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

    mtd tdefl_start_static_block()
    {
        d := &state
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

        tdefl_optimize_huffman_table(0, 288, 15, true)
        tdefl_optimize_huffman_table(1, 32, 15, true)

        TDEFL_PUT_BITS(1, 2)
    }

    #[Swag.Mixin]
    func TDEFL_RLE_PREV_CODE_SIZE()
    {
        if rle_repeat_count
        {
            d := &state
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
    mtd TDEFL_RLE_ZERO_CODE_SIZE()
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

    mtd tdefl_start_dynamic_block()
    {
        d := &state

        var code_sizes_to_pack: [TDEFL_MAX_HUFF_SYMBOLS_0 + TDEFL_MAX_HUFF_SYMBOLS_1] u8
        var packed_code_sizes:  [TDEFL_MAX_HUFF_SYMBOLS_0 + TDEFL_MAX_HUFF_SYMBOLS_1] u8
        prev_code_size := 0xFF'u8

        d.m_huff_count[0, 256] = 1;

        tdefl_optimize_huffman_table(0, TDEFL_MAX_HUFF_SYMBOLS_0, 15, false)
        tdefl_optimize_huffman_table(1, TDEFL_MAX_HUFF_SYMBOLS_1, 15, false)

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

        Memory.clear(&d.m_huff_count[2, 0], cast(uint) @sizeof(d.m_huff_count[2, 0]) * TDEFL_MAX_HUFF_SYMBOLS_2)
        for i := 0; i < total_code_sizes_to_pack; i += 1
        {
            code_size := code_sizes_to_pack[i]
            if !code_size
            {
                TDEFL_RLE_PREV_CODE_SIZE()
                rle_z_count += 1
                if rle_z_count == 138
                {
                    TDEFL_RLE_ZERO_CODE_SIZE()
                }
            }
            else
            {
                TDEFL_RLE_ZERO_CODE_SIZE()
                if code_size != prev_code_size
                {
                    TDEFL_RLE_PREV_CODE_SIZE()
                    d.m_huff_count[2, code_size] = cast(u16) (d.m_huff_count[2, code_size] + 1)
                    packed_code_sizes[num_packed_code_sizes] = code_size
                    num_packed_code_sizes += 1
                }
                else
                {
                    rle_repeat_count += 1
                    if rle_repeat_count == 6
                    {
                        TDEFL_RLE_PREV_CODE_SIZE()
                    }
                }
            }

            prev_code_size = code_size
        }

        if rle_repeat_count
        {
            TDEFL_RLE_PREV_CODE_SIZE()
        }
        else
        {
            TDEFL_RLE_ZERO_CODE_SIZE()
        }

        tdefl_optimize_huffman_table(2, TDEFL_MAX_HUFF_SYMBOLS_2, 7, false)

        TDEFL_PUT_BITS(2, 2)
        TDEFL_PUT_BITS(num_lit_codes - 257, 5)
        TDEFL_PUT_BITS(num_dist_codes - 1, 5)

        num_bit_lengths := 18'u32
        while num_bit_lengths >= 0
        {
            if d.m_huff_code_sizes[2, s_tdefl_packed_code_size_syms_swizzle[num_bit_lengths]]
                break
            num_bit_lengths -= 1
        }

        num_bit_lengths = Math.max(4, num_bit_lengths + 1)
        TDEFL_PUT_BITS(num_bit_lengths - 4, 4)
        for i := 0; i < num_bit_lengths; i += 1
        {
            TDEFL_PUT_BITS(cast(u32) d.m_huff_code_sizes[2, s_tdefl_packed_code_size_syms_swizzle[i]], 3)
        }

        packed_code_sizes_index := 0
        while packed_code_sizes_index < num_packed_code_sizes
        {
            cde := packed_code_sizes[packed_code_sizes_index]
            packed_code_sizes_index += 1
            TDEFL_PUT_BITS(cast(u32) d.m_huff_codes[2, cde], cast(u32) d.m_huff_code_sizes[2, cde])
            if cde >= 16
            {
                const p = @[2, 3, 7]
                TDEFL_PUT_BITS(cast(u32) packed_code_sizes[packed_code_sizes_index], cast(u32) p[cde - 16])
                packed_code_sizes_index += 1
            }
        }
    }

    mtd tdefl_compress_lz_codes()->bool
    {
        d := &state
        flags := 1'u32
        for pLZ_codes := &d.m_lz_code_buf[0]; pLZ_codes < d.m_pLZ_code_buf; flags >>= 1
        {
            if flags == 1
            {
                flags = cast(u32) (dref pLZ_codes) | 0x100
                pLZ_codes += 1
            }

            if flags & 1
            {
                match_len  := cast(u32) pLZ_codes[0]
                match_dist := cast(u32) (pLZ_codes[1] | (pLZ_codes[2] << 8))
                pLZ_codes += 3

                TDEFL_PUT_BITS(cast(u32) d.m_huff_codes[0, s_tdefl_len_sym[match_len]], cast(u32) d.m_huff_code_sizes[0, s_tdefl_len_sym[match_len]])
                TDEFL_PUT_BITS(match_len & mz_bitmasks[s_tdefl_len_extra[match_len]], cast(u32) s_tdefl_len_extra[match_len])

                var sym, num_extra_bits: u32
                if match_dist < 512
                {
                    sym = s_tdefl_small_dist_sym[match_dist]
                    num_extra_bits = s_tdefl_small_dist_extra[match_dist]
                }
                else
                {
                    sym = s_tdefl_large_dist_sym[match_dist >> 8]
                    num_extra_bits = s_tdefl_large_dist_extra[match_dist >> 8]
                }

                TDEFL_PUT_BITS(cast(u32) d.m_huff_codes[1, sym], cast(u32) d.m_huff_code_sizes[1, sym])
                TDEFL_PUT_BITS(match_dist & mz_bitmasks[num_extra_bits], num_extra_bits)
            }
            else
            {
                lit := dref pLZ_codes
                pLZ_codes += 1
                TDEFL_PUT_BITS(cast(u32) d.m_huff_codes[0, lit], cast(u32) d.m_huff_code_sizes[0, lit])
            }
        }

        TDEFL_PUT_BITS(cast(u32) d.m_huff_codes[0, 256], cast(u32) d.m_huff_code_sizes[0, 256])
        return d.m_pOutput_buf < d.m_pOutput_buf_end
    }

    mtd tdefl_compress_block(static_block: bool)->bool
    {
        if static_block
            tdefl_start_static_block()
        else
            tdefl_start_dynamic_block()
        return tdefl_compress_lz_codes()
    }

    mtd tdefl_flush_block(flush: tdefl_flush)->s32
    {
        d := &state

        comp_block_succeeded := false
        use_raw_block := ((d.m_flags & TDEFL_FORCE_ALL_RAW_BLOCKS) != 0) and (d.m_lookahead_pos - d.m_lz_code_buf_dict_pos) <= d.m_dict_size
        pOutput_buf_start := (((dref d.m_pOut_buf_size - d.m_out_buf_ofs) >= TDEFL_OUT_BUF_SIZE)) ? (cast(*u8) d.m_pOut_buf + d.m_out_buf_ofs) : &d.m_output_buf[0]

        d.m_pOutput_buf = pOutput_buf_start
        d.m_pOutput_buf_end = d.m_pOutput_buf + TDEFL_OUT_BUF_SIZE - 16

        d.m_output_flush_ofs = 0
        d.m_output_flush_remaining = 0

        dref d.m_pLZ_flags = cast(u8) (dref d.m_pLZ_flags >> d.m_num_flags_left)
        d.m_pLZ_code_buf -= (d.m_num_flags_left == 8) ? 1 : 0

        if (d.m_flags & TDEFL_WRITE_ZLIB_HEADER) and !d.m_block_index
        {
            flevel := 3'u8
            n := cast(u32) @sizeof(s_tdefl_num_probes) / @sizeof(u32)

            i := 0
            while i < n
            {
                if s_tdefl_num_probes[i] == (d.m_flags & 0xFFF)
                    break
                i += 1
            }

            if i < 2
                flevel = 0
            elif i < 6
                flevel = 1
            elif i == 6
                flevel = 2

            const cmf = 0x78'u32
            header := (cmf << 8) | (flevel << 6)
            header += 31 - (header % 31)
            flg := header & 0xFF

            TDEFL_PUT_BITS(cmf, 8)
            TDEFL_PUT_BITS(flg, 8)
        }

        TDEFL_PUT_BITS(flush == .TDEFL_FINISH ? 1 : 0, 1)

        pSaved_output_buf := d.m_pOutput_buf
        saved_bit_buf := d.m_bit_buffer
        saved_bits_in := d.m_bits_in

        if !use_raw_block
            comp_block_succeeded = tdefl_compress_block((d.m_flags & TDEFL_FORCE_ALL_STATIC_BLOCKS) or (d.m_total_lz_bytes < 48))

        if ((use_raw_block or (d.m_total_lz_bytes and ((d.m_pOutput_buf - pSaved_output_buf + 1) >= d.m_total_lz_bytes))) and
            ((d.m_lookahead_pos - d.m_lz_code_buf_dict_pos) <= d.m_dict_size))
        {
            d.m_pOutput_buf = pSaved_output_buf
            d.m_bit_buffer = saved_bit_buf
            d.m_bits_in = saved_bits_in
            TDEFL_PUT_BITS(0, 2)
            if d.m_bits_in
            {
                TDEFL_PUT_BITS(0, 8 - d.m_bits_in)
            }

            for i := 2; i != 0; i -= 1
            {
                TDEFL_PUT_BITS(d.m_total_lz_bytes & 0xFFFF, 16)
                d.m_total_lz_bytes ^= 0xFFFF
            }

            for i := 0; i < d.m_total_lz_bytes; i += 1
            {
                TDEFL_PUT_BITS(cast(u32) d.m_dict[(d.m_lz_code_buf_dict_pos + i) & TDEFL_LZ_DICT_SIZE_MASK], 8)
            }
        }
        elif !comp_block_succeeded
        {
            d.m_pOutput_buf = pSaved_output_buf
            d.m_bit_buffer = saved_bit_buf
            d.m_bits_in = saved_bits_in
            discard tdefl_compress_block(true)
        }

        if flush
        {
            if flush == .TDEFL_FINISH
            {
                if d.m_bits_in
                {
                    TDEFL_PUT_BITS(0, 8 - d.m_bits_in)
                }
                if d.m_flags & TDEFL_WRITE_ZLIB_HEADER
                {
                    a := d.m_adler32
                    for i := 0; i < 4; i += 1
                    {
                        TDEFL_PUT_BITS((a >> 24) & 0xFF, 8)
                        a <<= 8
                    }
                }
            }
            else
            {
                z := 0'u32
                TDEFL_PUT_BITS(0, 3)
                if d.m_bits_in
                {
                    TDEFL_PUT_BITS(0, 8 - d.m_bits_in)
                }

                for i := 2; i != 0; i -= 1
                {
                    TDEFL_PUT_BITS(z & 0xFFFF, 16)
                    z ^= 0xFFFF
                }
            }
        }

        Memory.clear(&d.m_huff_count[0, 0], cast(uint) @sizeof(d.m_huff_count[0, 0]) * TDEFL_MAX_HUFF_SYMBOLS_0)
        Memory.clear(&d.m_huff_count[1, 0], cast(uint) @sizeof(d.m_huff_count[1, 0]) * TDEFL_MAX_HUFF_SYMBOLS_1)

        d.m_pLZ_code_buf = &d.m_lz_code_buf[0] + 1
        d.m_pLZ_flags = &d.m_lz_code_buf[0]
        d.m_num_flags_left = 8
        d.m_lz_code_buf_dict_pos += d.m_total_lz_bytes
        d.m_total_lz_bytes = 0
        d.m_block_index += 1

        n := cast(s32) (d.m_pOutput_buf - pOutput_buf_start)
        if n != 0
        {
            if pOutput_buf_start == &d.m_output_buf[0]
            {
                bytes_to_copy := cast(s32) Math.min(cast(uint) n, cast(uint) (dref d.m_pOut_buf_size - d.m_out_buf_ofs))
                Memory.copy(cast(*u8) d.m_pOut_buf + d.m_out_buf_ofs, d.m_output_buf, cast(uint) bytes_to_copy)
                d.m_out_buf_ofs += bytes_to_copy
                n -= bytes_to_copy
                if n != 0
                {
                    d.m_output_flush_ofs = bytes_to_copy
                    d.m_output_flush_remaining = n
                }
            }
            else
            {
                d.m_out_buf_ofs += n
            }
        }

        return d.m_output_flush_remaining
    }

    mtd tdefl_compress_normal()->bool throw
    {
        d := &state
        pSrc := d.m_pSrc
        src_buf_left := d.m_src_buf_left
        flush := d.m_flush

        while src_buf_left or (flush and d.m_lookahead_size)
        {
            if (d.m_lookahead_size + d.m_dict_size) >= (TDEFL_MIN_MATCH_LEN - 1)
            {
                var dst_pos: u32 = (d.m_lookahead_pos + d.m_lookahead_size) & TDEFL_LZ_DICT_SIZE_MASK
                var ins_pos: u32 = d.m_lookahead_pos + d.m_lookahead_size - 2
                var hash: u32 = (d.m_dict[ins_pos & TDEFL_LZ_DICT_SIZE_MASK] << TDEFL_LZ_HASH_SHIFT) ^ d.m_dict[(ins_pos + 1) & TDEFL_LZ_DICT_SIZE_MASK]
                num_bytes_to_process := cast(u32) Math.min(cast(u32) src_buf_left, cast(u32) (TDEFL_MAX_MATCH_LEN - d.m_lookahead_size))

                pSrc_end := pSrc ? pSrc + num_bytes_to_process : null
                src_buf_left -= num_bytes_to_process
                d.m_lookahead_size += num_bytes_to_process

                while pSrc != pSrc_end
                {
                    c := dref pSrc
                    pSrc += 1
                    d.m_dict[dst_pos] = c;
                    if dst_pos < (TDEFL_MAX_MATCH_LEN - 1)
                        d.m_dict[TDEFL_LZ_DICT_SIZE + dst_pos] = c
                    hash = ((hash << TDEFL_LZ_HASH_SHIFT) ^ c) & (TDEFL_LZ_HASH_SIZE - 1)
                    d.m_next[ins_pos & TDEFL_LZ_DICT_SIZE_MASK] = d.m_hash[hash]
                    d.m_hash[hash] = cast(u16) ins_pos
                    dst_pos = (dst_pos + 1) & TDEFL_LZ_DICT_SIZE_MASK
                    ins_pos += 1
                }
            }
            else
            {
                while src_buf_left and d.m_lookahead_size < TDEFL_MAX_MATCH_LEN
                {
                    c := dref pSrc
                    pSrc += 1
                    dst_pos := (d.m_lookahead_pos + d.m_lookahead_size) & TDEFL_LZ_DICT_SIZE_MASK
                    src_buf_left -= 1
                    d.m_dict[dst_pos] = c
                    if dst_pos < (TDEFL_MAX_MATCH_LEN - 1)
                        d.m_dict[TDEFL_LZ_DICT_SIZE + dst_pos] = c
                    d.m_lookahead_size += 1
                    if (d.m_lookahead_size + d.m_dict_size) >= TDEFL_MIN_MATCH_LEN
                    {
                        var ins_pos: u32 = d.m_lookahead_pos + (d.m_lookahead_size - 1) - 2
                        var hash: u32 = ((cast(u32) d.m_dict[ins_pos & TDEFL_LZ_DICT_SIZE_MASK] << (TDEFL_LZ_HASH_SHIFT * 2)) ^
                                         (cast(u32) d.m_dict[(ins_pos + 1) & TDEFL_LZ_DICT_SIZE_MASK] << TDEFL_LZ_HASH_SHIFT) ^ c) & (TDEFL_LZ_HASH_SIZE - 1)
                        d.m_next[ins_pos & TDEFL_LZ_DICT_SIZE_MASK] = d.m_hash[hash]
                        d.m_hash[hash] = cast(u16) ins_pos
                    }
                }
            }

            d.m_dict_size = Math.min(cast(u32) (TDEFL_LZ_DICT_SIZE - d.m_lookahead_size), d.m_dict_size)
            if flush == 0 and (d.m_lookahead_size < TDEFL_MAX_MATCH_LEN)
                break

            len_to_move := 1'u32
            cur_match_dist := 0'u32
            cur_match_len := d.m_saved_match_len ? d.m_saved_match_len : cast(u32) (TDEFL_MIN_MATCH_LEN - 1)
            var cur_pos: u32 = d.m_lookahead_pos & TDEFL_LZ_DICT_SIZE_MASK
            if d.m_flags & (TDEFL_RLE_MATCHES | TDEFL_FORCE_ALL_RAW_BLOCKS)
            {
                if (d.m_dict_size) and !(d.m_flags & TDEFL_FORCE_ALL_RAW_BLOCKS)
                {
                    c := d.m_dict[(cur_pos - 1) & TDEFL_LZ_DICT_SIZE_MASK]
                    cur_match_len = 0
                    while cur_match_len < d.m_lookahead_size
                    {
                        if d.m_dict[cur_pos + cur_match_len] != c
                            break
                        cur_match_len += 1
                    }
                    if cur_match_len < TDEFL_MIN_MATCH_LEN
                        cur_match_len = 0
                    else
                        cur_match_dist = 1
                }
            }
            else
            {
                tdefl_find_match(d.m_lookahead_pos, d.m_dict_size, d.m_lookahead_size, &cur_match_dist, &cur_match_len)
            }

            if  ((cur_match_len == TDEFL_MIN_MATCH_LEN) and (cur_match_dist >= 8 * 1024)) or
                (cur_pos == cur_match_dist) or
                ((d.m_flags & TDEFL_FILTER_MATCHES) and (cur_match_len <= 5))
            {
                cur_match_dist, cur_match_len = 0
            }

            if d.m_saved_match_len
            {
                if cur_match_len > d.m_saved_match_len
                {
                    tdefl_record_literal(cast(u8) d.m_saved_lit)
                    if cur_match_len >= 128
                    {
                        tdefl_record_match(cur_match_len, cur_match_dist)
                        d.m_saved_match_len = 0
                        len_to_move = cur_match_len
                    }
                    else
                    {
                        d.m_saved_lit = d.m_dict[cur_pos]
                        d.m_saved_match_dist = cur_match_dist
                        d.m_saved_match_len = cur_match_len
                    }
                }
                else
                {
                    tdefl_record_match(d.m_saved_match_len, d.m_saved_match_dist)
                    len_to_move = d.m_saved_match_len - 1;
                    d.m_saved_match_len = 0;
                }
            }
            elif !cur_match_dist
            {
                tdefl_record_literal(d.m_dict[Math.min(cur_pos, cast(u32) @sizeof(d.m_dict) - 1)])
            }
            elif d.m_greedy_parsing or (d.m_flags & TDEFL_RLE_MATCHES) or (cur_match_len >= 128)
            {
                tdefl_record_match(cur_match_len, cur_match_dist)
                len_to_move = cur_match_len
            }
            else
            {
                d.m_saved_lit = d.m_dict[Math.min(cur_pos, cast(u32) @sizeof(d.m_dict) - 1)]
                d.m_saved_match_dist = cur_match_dist
                d.m_saved_match_len = cur_match_len
            }

            d.m_lookahead_pos += len_to_move
            Debug.assert(d.m_lookahead_size >= len_to_move)
            d.m_lookahead_size -= len_to_move
            d.m_dict_size = Math.min(d.m_dict_size + len_to_move, cast(u32) TDEFL_LZ_DICT_SIZE)

            if (d.m_pLZ_code_buf > &d.m_lz_code_buf[TDEFL_LZ_CODE_BUF_SIZE - 8]) or
                ((d.m_total_lz_bytes > 31 * 1024) and ((((cast(u32) (d.m_pLZ_code_buf - &d.m_lz_code_buf[0]) * 115) >> 7) >= d.m_total_lz_bytes) or (d.m_flags & TDEFL_FORCE_ALL_RAW_BLOCKS)))
            {
                d.m_pSrc = pSrc
                d.m_src_buf_left = src_buf_left
                n := tdefl_flush_block(.TDEFL_NO_FLUSH)
                if n
                {
                    return (n < 0) ? false : true
                }
            }
        }

        d.m_pSrc = pSrc
        d.m_src_buf_left = src_buf_left
        return true
    }

    mtd tdefl_compress(pIn_buf: const *u8, pIn_buf_size: *uint, pOut_buf: *u8, pOut_buf_size: *uint, flush: tdefl_flush)->tdefl_status throw
    {
        d := &state
        d.m_pIn_buf = pIn_buf
        d.m_pIn_buf_size = pIn_buf_size
        d.m_pOut_buf = pOut_buf
        d.m_pOut_buf_size = pOut_buf_size
        d.m_pSrc = cast(const *u8) pIn_buf
        d.m_src_buf_left = pIn_buf_size ? dref pIn_buf_size : 0
        d.m_flush = flush

        d.m_wants_to_finish |= (flush == .TDEFL_FINISH) ? 1 : 0
        if d.m_output_flush_remaining or d.m_finished
            return tdefl_flush_output_buffer()

        discard tdefl_compress_normal()

        //if ((d.m_flags & (TDEFL_WRITE_ZLIB_HEADER | TDEFL_COMPUTE_ADLER32)) && (pIn_buf))
        //    d.m_adler32 = (mz_uint32)mz_adler32(d.m_adler32, (const mz_uint8 *)pIn_buf, d.m_pSrc - (const mz_uint8 *)pIn_buf);

        if flush and !d.m_lookahead_size and !d.m_src_buf_left and !d.m_output_flush_remaining
        {
            discard tdefl_flush_block(flush)
            d.m_finished = flush == .TDEFL_FINISH ? 1 : 0
            if flush == .TDEFL_FULL_FLUSH
            {
                Memory.clear(&d.m_hash, @sizeof(d.m_hash))
                Memory.clear(&d.m_next, @sizeof(d.m_next))
                d.m_dict_size = 0
            }
        }

        return tdefl_flush_output_buffer()
    }

    mtd mz_deflate(fl: s32) throw
    {
        if !stream.avail_out
            throw "MZ_BUF_ERROR"

        flush := fl
        if flush == MZ_PARTIAL_FLUSH
            flush = MZ_SYNC_FLUSH

        //if (((tdefl_compressor *)stream.state)->m_prev_return_status == TDEFL_STATUS_DONE)
        //    return (flush == MZ_FINISH) ? MZ_STREAM_END : MZ_BUF_ERROR;

        orig_total_in  := stream.total_in
        orig_total_out := stream.total_out
        loop
        {
            in_bytes  := stream.avail_in
            out_bytes := stream.avail_out

            defl_status := tdefl_compress(stream.next_in, &in_bytes, stream.next_out, &out_bytes, cast(tdefl_flush) flush)
            stream.next_in  += in_bytes
            stream.avail_in -= in_bytes
            stream.total_in += in_bytes
            stream.adler = state.m_adler32

            stream.next_out  += out_bytes
            stream.avail_out -= out_bytes
            stream.total_out += out_bytes

            if defl_status < 0
                throw "MZ_STREAM_ERROR"

            if defl_status == .TDEFL_STATUS_DONE
                break
            if !stream.avail_out
                break
            if !stream.avail_in and flush != MZ_FINISH
            {
                if flush or (stream.total_in != orig_total_in) or (stream.total_out != orig_total_out)
                    break
                throw "MZ_BUF_ERROR"
            }
        }
    }
}

public impl Deflate
{
    const MZ_NO_COMPRESSION = 0
    const MZ_BEST_SPEED = 1
    const MZ_BEST_COMPRESSION = 9
    const MZ_UBER_COMPRESSION = 10
    const MZ_DEFAULT_LEVEL = 6
    const MZ_DEFAULT_COMPRESSION = Swag.U32.Max

    enum CompressionStrategy
    {
        MZ_DEFAULT_STRATEGY = 0
        MZ_FILTERED = 1
        MZ_HUFFMAN_ONLY = 2
        MZ_RLE = 3
        MZ_FIXED = 4
    }

    const MZ_DEFLATED = 8
    const MZ_DEFAULT_WINDOW_BITS = 15

    // Initialize the stream
    mtd init(src: const [..] u8, level: u32 = MZ_DEFAULT_COMPRESSION) throw
    {
        if @countof(src) > Swag.U32.Max
            throw "overflow"

        stream.next_in  = @dataof(src)
        stream.avail_in = cast(u32) @countof(src)

        mz_deflateInit2(level, MZ_DEFLATED, MZ_DEFAULT_WINDOW_BITS, 9, .MZ_DEFAULT_STRATEGY)
    }

    // Compress the associated stream
    mtd compress()->Array'u8 throw
    {
        var result: retval

        state.result = &result
        result.reserve(1024)
        stream.next_out  = result.buffer
        stream.avail_out = cast(u32) result.capacity
        mz_deflate(MZ_FINISH)

        return result
    }
}
