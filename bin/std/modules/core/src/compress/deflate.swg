/*
    :LICENCE
    This code is based on the miniz library (https://github.com/richgel999/miniz)
    The original code has been modified for Swag
    See LICENCE.md for the corresponding licence.
*/

#global namespace Compress

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
}

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
const TDEFL_MAX_HUFF_SYMBOLS_0 = 288
const TDEFL_MAX_HUFF_SYMBOLS_1 = 32
const TDEFL_MAX_HUFF_SYMBOLS_2 = 19
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

    mtd tdefl_compress_normal() throw
    {
        d := &state
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

        tdefl_compress_normal()

        //if ((d->m_flags & (TDEFL_WRITE_ZLIB_HEADER | TDEFL_COMPUTE_ADLER32)) && (pIn_buf))
        //    d->m_adler32 = (mz_uint32)mz_adler32(d->m_adler32, (const mz_uint8 *)pIn_buf, d->m_pSrc - (const mz_uint8 *)pIn_buf);

        if flush and !d.m_lookahead_size and !d.m_src_buf_left and !d.m_output_flush_remaining
        {
            //if (tdefl_flush_block(d, flush) < 0)
           //     return d->m_prev_return_status;
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
            //stream.adler = tdefl_get_adler32()

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

        result.resize(1024)
        stream.next_out  = result.buffer
        stream.avail_out = cast(u32) result.capacity
        mz_deflate(MZ_FINISH)

        return result
    }
}
