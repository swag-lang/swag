#global namespace jpg
using swag, core

const InBufSize         = 4096
const MaxHeight         = 8192
const MaxWidth          = 8192
const MaxComponents     = 4
const MaxQuantTables    = 4
const MaxHuffTables     = 8
const MaxBocksPerMcu    = 10
const MaxBlocksPerRow   = 6144
const MaxCompsInScan    = 4
const GrayScale         = 0
const YH1V1             = 1
const YH2V1             = 2
const YH1V2             = 3
const YH2V2             = 4
const Failed            = -1
const Done              = 1
const Okay              = 0
const ConstBits         = 13'u32
const Pass1Bits         = 2'u32
const Fix_0_298631336   = 2446
const Fix_0_390180644   = 3196
const Fix_0_541196100   = 4433
const Fix_0_765366865   = 6270
const Fix_0_899976223   = 7373
const Fix_1_175875602   = 9633
const Fix_1_501321110   = 12299
const Fix_1_847759065   = 15137
const Fix_1_961570560   = 16069
const Fix_2_053119869   = 16819
const Fix_2_562915447   = 20995
const Fix_3_072711026   = 25172

public struct HuffTables
{
    lookUp:    [256] s32
    codeSize:  [256] u8
    tree:      [512] s32
}

public struct CoeffBuf
{
    pdata:     Array'u8
    blockNumX: s32
    blockNumY: s32
    blockLenX: s32
    blockLenY: s32
    blockSize: s32
}

public struct Decoder
{
    stream:             const *u8
    inBufOfs:           *u8
    pSampleBuf:         *u8

    imageSizeX:         s32
    imageSizeY:         s32
    streamLen:          s32
    streamSeek:         s32
    bit_buf:            u32
    bits_left:          s32
    inBufLeft:          s32
    tem_flag:           s32
    eofReached:         s32
    totalBytesRead:     s32
    comps_in_frame:     s32
    restart_interval:   s32
    scan_type:          s32
    max_blocks_per_mcu: s32
    max_mcu_x_size:     s32
    max_mcu_y_size:     s32
    blocks_per_mcu:     s32
    max_mcus_per_row:   s32
    max_mcus_per_col:   s32
    max_blocks_per_row: s32
    total_lines_left:   s32
    mcu_lines_left:     s32
    spectral_start:     s32
    spectral_end:       s32
    successive_low:     s32
    successive_high:    s32
    mcus_per_row:       s32
    mcus_per_col:       s32
    eob_run:            s32
    restarts_left:      s32
    next_restart_num:   s32
    comps_in_scan:      s32

    dest_bytes_per_pixel:           s32
    real_dest_bytes_per_scan_line:  s32
    dest_bytes_per_scan_line:       s32

    accept_meta:        bool
    progressive_flag:   bool

    in_buf:             [InBufSize + 128] u8
    compSampH:        [MaxComponents] s32
    compSampV:        [MaxComponents] s32
    compQuant:         [MaxComponents] s32
    compIdent:         [MaxComponents] s32
    blockMaxZagSet:  [MaxBlocksPerRow] s32
    block_seg:          [MaxBlocksPerRow] *s16
    comp_list:          [MaxCompsInScan] s32
    comp_dc_tab:        [MaxComponents] s32
    comp_ac_tab:        [MaxComponents] s32
    comp_h_blocks:      [MaxComponents] s32
    comp_v_blocks:      [MaxComponents] s32
    mcu_org:            [MaxBocksPerMcu] s32
    h:                  [MaxHuffTables] HuffTables
    dc_huff_seg:        [MaxBocksPerMcu] *HuffTables
    ac_huff_seg:        [MaxBocksPerMcu] *HuffTables
    block_y_mcu:        [MaxComponents] s32
    component:          [MaxBocksPerMcu] *u32
    last_dc_val:        [MaxComponents] u32
    dc_coeffs:          [MaxComponents] CoeffBuf
    ac_coeffs:          [MaxComponents] CoeffBuf
    temp_block:         [64] s16

    crr:                [256] s32
    cbb:                [256] s32
    crg:                [256] s32
    cbg:                [256] s32

    huff_num:           [MaxHuffTables] Array'u8
    huff_val:           [MaxHuffTables] Array'u8
    quant:              [MaxQuantTables] Array's16
    scan_line_0:        Array'u8
    scan_line_1:        Array'u8
    q_buf:              Array'u8
    sample_buf:         Array'u8
}

enum Marker
{
    M_SOF0  = 0xC0
    M_SOF1  = 0xC1
    M_SOF2  = 0xC2
    M_SOF3  = 0xC3
    M_SOF5  = 0xC5
    M_SOF6  = 0xC6
    M_SOF7  = 0xC7
    M_JPG   = 0xC8
    M_SOF9  = 0xC9
    M_SOF10 = 0xCA
    M_SOF11 = 0xCB
    M_SOF13 = 0xCD
    M_SOF14 = 0xCE
    M_SOF15 = 0xCF
    M_DHT   = 0xC4
    M_DAC   = 0xCC
    M_RST0  = 0xD0
    M_RST1  = 0xD1
    M_RST2  = 0xD2
    M_RST3  = 0xD3
    M_RST4  = 0xD4
    M_RST5  = 0xD5
    M_RST6  = 0xD6
    M_RST7  = 0xD7
    M_SOI   = 0xD8
    M_EOI   = 0xD9
    M_SOS   = 0xDA
    M_DQT   = 0xDB
    M_DNL   = 0xDC
    M_DRI   = 0xDD
    M_DHP   = 0xDE
    M_EXP   = 0xDF
    M_APP0  = 0xE0
    M_APP1  = 0xE1
    M_APP15 = 0xEF
    M_JPG0  = 0xF0
    M_JPG13 = 0xFD
    M_COM   = 0xFE
    M_TEM   = 0x01
    M_ERROR = 0x100
}

const extend_off: [16] s32 = @[
    0,
    ((-1) << 1) + 1,
    ((-1) << 2) + 1,
    ((-1) << 3) + 1,
    ((-1) << 4) + 1,
    ((-1) << 5) + 1,
    ((-1) << 6) + 1,
    ((-1) << 7) + 1,
    ((-1) << 8) + 1,
    ((-1) << 9) + 1,
    ((-1) << 10) + 1,
    ((-1) << 11) + 1,
    ((-1) << 12) + 1,
    ((-1) << 13) + 1,
    ((-1) << 14) + 1,
    ((-1) << 15) + 1
]

const extend_test: [16] s32 = @[
    0, 0x0001, 0x0002, 0x0004, 0x0008, 0x0010, 0x0020,
    0x0040, 0x0080, 0x0100, 0x0200, 0x0400, 0x0800,
    0x1000, 0x2000, 0x4000
]

const zag: [64] u32 = @[
    0, 1, 8, 16, 9, 2, 3, 10, 17, 24, 32, 25, 18, 11, 4, 5,
    12, 19, 26, 33, 40, 48, 41, 34, 27, 20, 13, 6, 7, 14, 21,
    28, 35, 42, 49, 56, 57, 50, 43, 36, 29, 22, 15, 23, 30, 37,
    44, 51, 58, 59, 52, 45, 38, 31, 39, 46, 53, 60, 61, 54, 47,
    55, 62, 63,
]

#[inline, safety("of", false)]
func jpeg_descale(x: s32, n: u32) => (x + (1's32 << (n - 1))) >> n

#[inline, safety("of", false)]
func jpeg_rol(i, j: u32)->u32 => cast(u32) ((i << j) | (i >> (32 - j)))

#[inline, safety("of", false)]
func jpeg_clamp(i: u32) => cast(u8) ((i &0xFFFFFF00) ? (((~i) >> 31) & 0xFF) : i)

#[inline, safety("of", false)]
func jpeg_clampd(i: s32)->s32 => (i &0xFF00) ? (((~i) >> 15) & 0xFF) : i

impl Decoder
{
    func init(using self, buffer: const [..] u8) throw
    {
        @init(self)
        stream = @dataof(buffer)
        streamLen = cast(s32) @countof(buffer)

        inBufOfs = in_buf
        prepareStreamBuffer()
        bits_left = 16
        discard get_bits_1(16)
        discard get_bits_1(16)
        loop i: MaxBlocksPerRow blockMaxZagSet[i] = 64

        try locate_sof_marker()
    }

    func start(using self) throw
    {
        try init_frame()
        if progressive_flag
            try init_progressive()
        else
            try init_sequential()
    }

    func doit(using self, buf: *u8) throw
    {
        var scan_line_ofs: *void
        var scan_line_len: u32

        // Gray scale (1BPP)
        if comps_in_frame == 1
        {
            pdest := buf
            pdest += cast(uint) ((imageSizeY - 1) * imageSizeX * 3)
            while true
            {
                if try decode(&scan_line_ofs, &scan_line_len)
                    break

                pdest1 := pdest
                psrc := cast(*u8) scan_line_ofs
                loop imageSizeX
                {
                    pdest1[0] = :psrc
                    pdest1[1] = :psrc
                    pdest1[2] = :psrc
                    pdest1 += 3
                    psrc += 1
                }

                pdest -= cast(uint) (imageSizeX * 3)
            }
        }

        // 3 BPP
        else
        {
            pdest := buf
            pdest += cast(uint) ((imageSizeY - 1) * imageSizeX * 3)
            while true
            {
                if try decode(&scan_line_ofs, &scan_line_len)
                    break
                memory.copy(pdest, scan_line_ofs, cast(uint) imageSizeX * 3)
                pdest -= cast(uint) (imageSizeX * 3)
            }
        }
    }

    func coeff_buf_getp(_cb: CoeffBuf, _block_x, _block_y: s32)->*s16
    {
        debug.assert(_block_x < _cb.blockNumX)
        debug.assert(_block_y < _cb.blockNumY)
        return cast(*s16) (_cb.pdata.buffer + _block_x * _cb.blockSize + _block_y * (_cb.blockSize * _cb.blockNumX))
    }

    func load_next_row(using self) throw
    {
        var block_x_mcu: [MaxComponents] s32
        row_block := 0
        loop mcu_row: mcus_per_row
        {
            block_x_mcu_ofs := 0
            block_y_mcu_ofs := 0
            loop mcu_block: blocks_per_mcu
            {
                component_id := mcu_org[mcu_block]

                p   := block_seg[row_block]
                q   := quant[compQuant[component_id]]
                pAC := coeff_buf_getp(ac_coeffs[component_id], block_x_mcu[component_id] + block_x_mcu_ofs, block_y_mcu[component_id] + block_y_mcu_ofs)
                pDC := coeff_buf_getp(dc_coeffs[component_id], block_x_mcu[component_id] + block_x_mcu_ofs, block_y_mcu[component_id] + block_y_mcu_ofs)

                p[0] = pDC[0]
                memory.copy(&p[1], &pAC[1], 63'uint * @sizeof(s16))

                i := 63
                while i > 0
                {
                    if p[zag[i]]
                        break
                    i -= 1
                }

                while i >= 0
                {
                    if p[zag[i]]
                        p[zag[i]] *= q[i]
                    i -= 1
                }

                row_block += 1
                if comps_in_scan == 1
                    block_x_mcu[component_id] += 1
                else
                {
                    block_x_mcu_ofs += 1
                    if block_x_mcu_ofs == compSampH[component_id]
                    {
                        block_x_mcu_ofs = 0
                        block_y_mcu_ofs += 1
                        if block_y_mcu_ofs == compSampV[component_id]
                        {
                            block_y_mcu_ofs = 0
                            block_x_mcu[component_id] += compSampH[component_id]
                        }
                    }
                }
            }
        }

        if comps_in_scan == 1
            block_y_mcu[comp_list[0]] += 1
        else
        {
            loop component_num: comps_in_scan
            {
                component_id := comp_list[component_num]
                block_y_mcu[component_id] += compSampV[component_id]
            }
        }
    }

    func process_restart(using self) throw
    {
        i := 1536
        while i > 0
        {
            if get_char() == 0xFF
                break
            i -= 1
        }

        if i == 0
            throw "bad restart marker"

        c := 0'u32
        while i > 0
        {
            c = get_char()
            if c != 0xFF
                break;
            i -= 1
        }

        if i == 0 throw "bad restart marker"
        if c != (next_restart_num + cast(u32) Marker.M_RST0) throw "bad restart marker"

        memory.clear(&last_dc_val, cast(uint) comps_in_frame * @sizeof(u32))

        eob_run          = 0
        restarts_left    = restart_interval
        next_restart_num = (next_restart_num + 1) & 7
        bits_left        = 16

        discard get_bits_2(16)
        discard get_bits_2(16)
    }

    func huff_decode(using self, _ph: *HuffTables)->s32
    {
        symbol := _ph.lookUp[(bit_buf >>,safe 8) & 0xFF]
        if symbol < 0
        {
            discard get_bits_2(8)
            while true
            {
                symbol = _ph.tree[~symbol + (1 - get_bits_2(1))]
                if symbol >= 0
                    break
            }
        }
        else
        {
            discard get_bits_2(cast(s32) _ph.codeSize[symbol])
        }

        return symbol
    }

    func decode_next_row(using self) throw
    {
        row_block := 0
        loop mcu_row: mcus_per_row
        {
            if restart_interval && (restarts_left == 0)
                try process_restart()

            loop mcu_block: blocks_per_mcu
            {
                component_id := mcu_org[mcu_block]
                p            := block_seg[row_block]
                q            := quant[compQuant[component_id]].buffer

                s := huff_decode(&h[comp_dc_tab[component_id]])
                if s != 0
                {
                    r := cast(s32) get_bits_2(s)
                    s = r < extend_test[s] ? r + extend_off[s] : r
                }

                s += bitcast(s32) last_dc_val[component_id]
                last_dc_val[component_id] = bitcast(u32) s

                p[0]         = cast(s16) (s * q[0])
                prev_num_set := blockMaxZagSet[row_block]
                ph           := &h[comp_ac_tab[component_id]]

                k := 1
                while k < 64
                {
                    s = huff_decode(ph)
                    r := s >>,safe 4
                    s &= 0x0F

                    if s
                    {
                        if r
                        {
                            if (k + r) > 63 throw "decode error"
                            if k < prev_num_set
                            {
                                n := math.min(r, prev_num_set - k)
                                kt := k
                                while n
                                {
                                    p[zag[kt]] = 0;
                                    n -= 1
                                    kt += 1
                                }
                            }

                            k += r
                        }

                        r = cast(s32) get_bits_2(s)
                        if s > 15
                            throw "decode error"

                        s = (r < extend_test[s] ? r + extend_off[s] : r)
                        p[zag[k]] = cast(s16)(s * q[k])
                    }
                    else
                    {
                        if (r == 15)
                        {
                            if (k + 15) > 63
                                throw "decode error"

                            if k < prev_num_set
                            {
                                n := math.min(16'u32, cast(u32) (prev_num_set - k))
                                kt := k
                                while n
                                {
                                    p[zag[kt]] = 0;
                                    n -= 1
                                    kt += 1
                                }
                            }

                            k += 15
                        }
                        else
                        {
                            break
                        }
                    }

                    k += 1
                }

                if k < prev_num_set
                {
                    kt := k
                    while kt < prev_num_set
                    {
                        p[zag[kt]] = 0
                        kt += 1
                    }
                }

                blockMaxZagSet[row_block] = k
                row_block += 1
            }

            restarts_left -= 1
        }
    }

    func idct(using self, _data: *s16, buf: *u8)
    {
        _pdst_ptr := buf
        dataptr := _data;
        for rowctr := 8 - 1; rowctr >= 0; rowctr -= 1
        {
            if (dataptr[1] | dataptr[2] | dataptr[3] | dataptr[4] | dataptr[5] | dataptr[6] | dataptr[7]) == 0
            {
                dcval := cast(s16) (dataptr[0] << Pass1Bits)
                dataptr[0] = dcval
                dataptr[1] = dcval
                dataptr[2] = dcval
                dataptr[3] = dcval
                dataptr[4] = dcval
                dataptr[5] = dcval
                dataptr[6] = dcval
                dataptr[7] = dcval
                dataptr += 8
                continue;
            }

            z2 := cast(s32) dataptr[2]
            z3 := cast(s32) dataptr[6]

            z1   := (z2 + z3) * Fix_0_541196100
            tmp2 := z1 + (z3 * -Fix_1_847759065)
            tmp3 := z1 + (z2 * Fix_0_765366865)

            tmp0 := (cast(s32) dataptr[0] + cast(s32) dataptr[4]) << ConstBits
            tmp1 := (cast(s32) dataptr[0] - cast(s32) dataptr[4]) << ConstBits

            tmp10 := tmp0 + tmp3
            tmp13 := tmp0 - tmp3
            tmp11 := tmp1 + tmp2
            tmp12 := tmp1 - tmp2

            tmp0 = cast(s32) dataptr[7]
            tmp1 = cast(s32) dataptr[5]
            tmp2 = cast(s32) dataptr[3]
            tmp3 = cast(s32) dataptr[1]

            z1 = tmp0 + tmp3
            z2 = tmp1 + tmp2
            z3 = tmp0 + tmp2
            z4 := tmp1 + tmp3
            z5 := (z3 + z4) * Fix_1_175875602

            tmp0 = (tmp0 * Fix_0_298631336)
            tmp1 = (tmp1 * Fix_2_053119869)
            tmp2 = (tmp2 * Fix_3_072711026)
            tmp3 = (tmp3 * Fix_1_501321110)
            z1   = (z1 * -Fix_0_899976223)
            z2   = (z2 * -Fix_2_562915447)
            z3   = (z3 * -Fix_1_961570560)
            z4   = (z4 * -Fix_0_390180644)

            z3 += z5
            z4 += z5

            tmp0 += z1 + z3
            tmp1 += z2 + z4
            tmp2 += z2 + z3
            tmp3 += z1 + z4

            dataptr[0] = cast(s16) jpeg_descale(tmp10 + tmp3, ConstBits - Pass1Bits)
            dataptr[7] = cast(s16) jpeg_descale(tmp10 - tmp3, ConstBits - Pass1Bits)
            dataptr[1] = cast(s16) jpeg_descale(tmp11 + tmp2, ConstBits - Pass1Bits)
            dataptr[6] = cast(s16) jpeg_descale(tmp11 - tmp2, ConstBits - Pass1Bits)
            dataptr[2] = cast(s16) jpeg_descale(tmp12 + tmp1, ConstBits - Pass1Bits)
            dataptr[5] = cast(s16) jpeg_descale(tmp12 - tmp1, ConstBits - Pass1Bits)
            dataptr[3] = cast(s16) jpeg_descale(tmp13 + tmp0, ConstBits - Pass1Bits)
            dataptr[4] = cast(s16) jpeg_descale(tmp13 - tmp0, ConstBits - Pass1Bits)

            dataptr += 8
        }

        dataptr = _data
        for rowctr := 8 - 1; rowctr >= 0; rowctr -= 1
        {
            if (dataptr[8 * 1] | dataptr[8 * 2] | dataptr[8 * 3] | dataptr[8 * 4] | dataptr[8 * 5] | dataptr[8 * 6] | dataptr[8 * 7]) == 0
            {
                dcval := cast(s16) jpeg_descale(cast(s32) dataptr[0], Pass1Bits + 3)
                dcval +=,safe 128
                dcval = math.clamp(dcval, 0's16, 255's16)

                _pdst_ptr[8 * 0] = cast(u8) dcval
                _pdst_ptr[8 * 1] = cast(u8) dcval
                _pdst_ptr[8 * 2] = cast(u8) dcval
                _pdst_ptr[8 * 3] = cast(u8) dcval
                _pdst_ptr[8 * 4] = cast(u8) dcval
                _pdst_ptr[8 * 5] = cast(u8) dcval
                _pdst_ptr[8 * 6] = cast(u8) dcval
                _pdst_ptr[8 * 7] = cast(u8) dcval

                dataptr += 1
                _pdst_ptr += 1
                continue
            }

            z2 := cast(s32) dataptr[8 * 2]
            z3 := cast(s32) dataptr[8 * 6]

            z1   := (z2 + z3) * Fix_0_541196100
            tmp2 := z1 + (z3 * -Fix_1_847759065)
            tmp3 := z1 + (z2 * Fix_0_765366865)

            tmp0 := (cast(s32) dataptr[8 * 0] + cast(s32) dataptr[8 * 4]) << ConstBits
            tmp1 := (cast(s32) dataptr[8 * 0] - cast(s32) dataptr[8 * 4]) << ConstBits

            tmp10 := tmp0 + tmp3
            tmp13 := tmp0 - tmp3
            tmp11 := tmp1 + tmp2
            tmp12 := tmp1 - tmp2

            tmp0 = cast(s32) dataptr[8 * 7]
            tmp1 = cast(s32) dataptr[8 * 5]
            tmp2 = cast(s32) dataptr[8 * 3]
            tmp3 = cast(s32) dataptr[8 * 1]

            z1 = tmp0 + tmp3
            z2 = tmp1 + tmp2
            z3 = tmp0 + tmp2
            z4 := tmp1 + tmp3
            z5 := (z3 + z4) * Fix_1_175875602

            tmp0 = (tmp0 * Fix_0_298631336)
            tmp1 = (tmp1 * Fix_2_053119869)
            tmp2 = (tmp2 * Fix_3_072711026)
            tmp3 = (tmp3 * Fix_1_501321110)
            z1   = (z1 * -Fix_0_899976223)
            z2   = (z2 * -Fix_2_562915447)
            z3   = (z3 * -Fix_1_961570560)
            z4   = (z4 * -Fix_0_390180644)

            z3 += z5
            z4 += z5

            tmp0 += z1 + z3
            tmp1 += z2 + z4
            tmp2 += z2 + z3
            tmp3 += z1 + z4

            i := cast(s16) jpeg_descale(tmp10 + tmp3, ConstBits + Pass1Bits + 3) + 128
            i = jpeg_clampd(i)
            _pdst_ptr[8 * 0] = cast(u8) i

            i = cast(s16) jpeg_descale(tmp10 - tmp3, ConstBits + Pass1Bits + 3) + 128
            i = jpeg_clampd(i)
            _pdst_ptr[8 * 7] = cast(u8) i

            i = cast(s16) jpeg_descale(tmp11 + tmp2, ConstBits + Pass1Bits + 3) + 128
            i = jpeg_clampd(i)
            _pdst_ptr[8 * 1] = cast(u8) i

            i = cast(s16) jpeg_descale(tmp11 - tmp2, ConstBits + Pass1Bits + 3) + 128
            i = jpeg_clampd(i)
            _pdst_ptr[8 * 6] = cast(u8) i

            i = cast(s16) jpeg_descale(tmp12 + tmp1, ConstBits + Pass1Bits + 3) + 128
            i = jpeg_clampd(i)
            _pdst_ptr[8 * 2] = cast(u8) i

            i = cast(s16) jpeg_descale(tmp12 - tmp1, ConstBits + Pass1Bits + 3) + 128
            i = jpeg_clampd(i)
            _pdst_ptr[8 * 5] = cast(u8) i

            i = cast(s16) jpeg_descale(tmp13 + tmp0, ConstBits + Pass1Bits + 3) + 128
            i = jpeg_clampd(i)
            _pdst_ptr[8 * 3] = cast(u8) i

            i = cast(s16) jpeg_descale(tmp13 - tmp0, ConstBits + Pass1Bits + 3) + 128
            i = jpeg_clampd(i)
            _pdst_ptr[8 * 4] = cast(u8) i

            dataptr += 1
            _pdst_ptr += 1
        }
    }

    func transform_row(using self)
    {
        psrc_ptr := block_seg[0]
        pdst_ptr := pSampleBuf

        for i := max_blocks_per_row; i > 0; i -= 1
        {
            memory.copy(temp_block, psrc_ptr, 64'uint * @sizeof(s16))
            idct(temp_block, pdst_ptr)
            psrc_ptr += 64
            pdst_ptr += 64
        }
    }

    func find_eoi(using self) throw
    {
        if !progressive_flag
        {
            bits_left = 16
            discard get_bits_1(16)
            discard get_bits_1(16)
            discard try process_markers()
        }

        totalBytesRead -= inBufLeft
    }

    func decode(using self, scan_line_ofs: **void, scan_line_len: *u32)->s32 throw
    {
        if total_lines_left == 0
            return Done

        if mcu_lines_left == 0
        {
            if progressive_flag
                try load_next_row()
            else
                try decode_next_row()
            if total_lines_left <= max_mcu_y_size
                try find_eoi()
            transform_row()
            mcu_lines_left = max_mcu_y_size
        }

        switch scan_type
        {
        case YH2V2:
            if (mcu_lines_left & 1) == 0
            {
                H2V2Convert()
                :scan_line_ofs = scan_line_0.buffer
            }
            else
                :scan_line_ofs = scan_line_1.buffer

        case YH2V1:
            H2V1Convert()
            :scan_line_ofs = scan_line_0.buffer

        case YH1V2:
            if (mcu_lines_left & 1) == 0
            {
                H1V2Convert()
                :scan_line_ofs = scan_line_0.buffer
            }
            else
                :scan_line_ofs = scan_line_1.buffer

        case YH1V1:
            H1V1Convert()
            :scan_line_ofs = scan_line_0.buffer

        case GrayScale:
            GrayConvert()
            :scan_line_ofs = scan_line_0.buffer
        }

        :scan_line_len = cast(u32) real_dest_bytes_per_scan_line
        mcu_lines_left -= 1
        total_lines_left -= 1

        return Okay
    }

    func GrayConvert(using self)
    {
        row := max_mcu_y_size - mcu_lines_left
        d := scan_line_0.buffer
        s := pSampleBuf + row * 8
        for i := max_mcus_per_row; i > 0; i -= 1
        {
            d[0] = s[0]
            d[1] = s[1]
            d[2] = s[2]
            d[3] = s[3]
            d[4] = s[4]
            d[5] = s[5]
            d[6] = s[6]
            d[7] = s[7]

            s += 64
            d += 8
        }
    }

    func H1V1Convert(using self)
    {
        row := max_mcu_y_size - mcu_lines_left
        d := scan_line_0.buffer
        s := pSampleBuf + row * 8
        for i := max_mcus_per_row; i > 0; i -= 1
        {
            loop j: 8
            {
                y  := s[j]
                cb := s[64 + j]
                cr := s[128 + j]

                d[2] = jpeg_clamp(y +,safe crr[cr])
                d[1] = jpeg_clamp(y +,safe ((crg[cr] + cbg[cb]) >>,safe 16))
                d[0] = jpeg_clamp(y +,safe cbb[cb])
                d += 3
            }

            s += 64 * 3
        }
    }

    func H1V2Convert(using self)
    {
        row := max_mcu_y_size - mcu_lines_left
        d0 := scan_line_0.buffer
        d1 := scan_line_1.buffer

        var y: *u8
        if row < 8
            y = pSampleBuf + row * 8
        else
            y = pSampleBuf + 64 * 1 + (row & 7) * 8

        c := pSampleBuf + 64 * 2 + (row >>,safe 1) * 8
        for i := max_mcus_per_row; i > 0; i -= 1
        {
            loop j: 8
            {
                cb := c[0 + j]
                cr := c[64 + j]
                rc := crr[cr]
                gc := ((crg[cr] + cbg[cb]) >>,safe 16)
                bc := cbb[cb]
                yy := y[j]

                d0[2] = jpeg_clamp(yy +,safe rc)
                d0[1] = jpeg_clamp(yy +,safe gc)
                d0[0] = jpeg_clamp(yy +,safe bc)

                yy    = y[8 + j]
                d1[2] = jpeg_clamp(yy +,safe rc)
                d1[1] = jpeg_clamp(yy +,safe gc)
                d1[0] = jpeg_clamp(yy +,safe bc)

                d0 += 3
                d1 += 3
            }

            y += 64 * 4
            c += 64 * 4
        }
    }

    func H2V1Convert(using self)
    {
        row := max_mcu_y_size - mcu_lines_left
        d0 := scan_line_0.buffer
        y := pSampleBuf + row * 8
        c := pSampleBuf + 2 * 64 + row * 8

        for i := max_mcus_per_row; i > 0; i -= 1
        {
            loop l: 2
            {
                loop j: 4
                {
                    cb := c[0]
                    cr := c[64]
                    rc := crr[cr]
                    gc := (crg[cr] + cbg[cb]) >>,safe 16
                    bc := cbb[cb]
                    yy := y[j << 1]

                    d0[2] = jpeg_clamp(yy +,safe rc)
                    d0[1] = jpeg_clamp(yy +,safe gc)
                    d0[0] = jpeg_clamp(yy +,safe bc)

                    yy    = y[(j << 1) + 1]
                    d0[5] = jpeg_clamp(yy +,safe rc)
                    d0[4] = jpeg_clamp(yy +,safe gc)
                    d0[3] = jpeg_clamp(yy +,safe bc)

                    d0 += 6

                    c += 1
                }

                y += 64
            }

            y += 64 * 4 - 64 * 2
            c += 64 * 4 - 8
        }
    }

    func H2V2Convert(using self)
    {
        row := max_mcu_y_size - mcu_lines_left
        d0 := scan_line_0.buffer
        d1 := scan_line_1.buffer

        var y: *u8
        if (row < 8)
            y = pSampleBuf + row * 8
        else
            y = pSampleBuf + 64 * 2 + (row & 7) * 8
        c := pSampleBuf + 64 * 4 + (row >> 1) * 8

        for i := max_mcus_per_row; i > 0; i -= 1
        {
            loop l: 2
            {
                for j := 0; j < 8; j += 2
                {
                    cb := c[0]
                    cr := c[64]
                    rc := crr[cr]
                    gc := (crg[cr] + cbg[cb]) >>,safe 16
                    bc := cbb[cb]
                    yy := y[j]

                    d0[2] = jpeg_clamp(yy +,safe rc)
                    d0[1] = jpeg_clamp(yy +,safe gc)
                    d0[0] = jpeg_clamp(yy +,safe bc)

                    yy    = y[j + 1]
                    d0[5] = jpeg_clamp(yy +,safe rc)
                    d0[4] = jpeg_clamp(yy +,safe gc)
                    d0[3] = jpeg_clamp(yy +,safe bc)

                    yy    = y[j + 8]
                    d1[2] = jpeg_clamp(yy +,safe rc)
                    d1[1] = jpeg_clamp(yy +,safe gc)
                    d1[0] = jpeg_clamp(yy +,safe bc)

                    yy    = y[j + 8 + 1]
                    d1[5] = jpeg_clamp(yy +,safe rc)
                    d1[4] = jpeg_clamp(yy +,safe gc)
                    d1[3] = jpeg_clamp(yy +,safe bc)

                    d0 += 6
                    d1 += 6

                    c += 1
                }

                y += 64
            }

            y += 64 * 6 - 64 * 2
            c += 64 * 6 - 8
        }
    }

    func init_frame(using self) throw
    {
        if comps_in_frame == 1
        {
            scan_type          = GrayScale
            max_blocks_per_mcu = 1
            max_mcu_x_size     = 8
            max_mcu_y_size     = 8
        }
        else if comps_in_frame == 3
        {
            if (compSampH[1] != 1) || (compSampV[1] != 1) || (compSampH[2] != 1) || (compSampV[2] != 1)
                throw "unsupported samp factors"

            if (compSampH[0] == 1) && (compSampV[0] == 1)
            {
                scan_type          = YH1V1
                max_blocks_per_mcu = 3
                max_mcu_x_size     = 8
                max_mcu_y_size     = 8
            }
            else if (compSampH[0] == 2) && (compSampV[0] == 1)
            {
                scan_type          = YH2V1
                max_blocks_per_mcu = 4
                max_mcu_x_size     = 16
                max_mcu_y_size     = 8
            }
            else if (compSampH[0] == 1) && (compSampV[0] == 2)
            {
                scan_type          = YH1V2
                max_blocks_per_mcu = 4
                max_mcu_x_size     = 8
                max_mcu_y_size     = 16
            }
            else if (compSampH[0] == 2) && (compSampV[0] == 2)
            {
                scan_type          = YH2V2
                max_blocks_per_mcu = 6
                max_mcu_x_size     = 16
                max_mcu_y_size     = 16
            }
            else
                throw "unsupported samp factors"
        }
        else
            throw "unsupported color space"

        max_mcus_per_row = (imageSizeX + (max_mcu_x_size - 1)) / max_mcu_x_size
        max_mcus_per_col = (imageSizeY + (max_mcu_y_size - 1)) / max_mcu_y_size

        if scan_type == GrayScale
            dest_bytes_per_pixel = 1
        else
            dest_bytes_per_pixel = 4

        dest_bytes_per_scan_line      = ((imageSizeX + 15) & 0xFFF0) * dest_bytes_per_pixel
        real_dest_bytes_per_scan_line = (imageSizeX * dest_bytes_per_pixel)

        scan_line_0.resize(cast(uint) dest_bytes_per_scan_line + 8)
        memory.clear(scan_line_0.buffer, cast(uint) dest_bytes_per_scan_line)

        scan_line_1.resize(cast(uint) dest_bytes_per_scan_line + 8)
        memory.clear(scan_line_1.buffer, cast(uint) dest_bytes_per_scan_line)

        max_blocks_per_row = max_mcus_per_row * max_blocks_per_mcu
        if max_blocks_per_row > MaxBlocksPerRow
            throw "assertion error"

        q_buf.resize(cast(uint) max_blocks_per_row * 64 * @sizeof(s16) + 8)
        loop i: max_blocks_per_row
            block_seg[i] = cast(*s16) (q_buf.buffer + i * 64 * @sizeof(s16))
        loop i: max_blocks_per_row
            blockMaxZagSet[i] = 64

        sample_buf.resize(cast(uint) max_blocks_per_row * 64 + 8)
        pSampleBuf = sample_buf.buffer

        total_lines_left = imageSizeY
        mcu_lines_left   = 0
        create_look_ups();
    }

    func create_look_ups(using self)
    {
        var fix: [4] s32

        fix[0] = cast(s32) ((1.40200 / 2) * (1'u32 << 16) + 0.5)
        fix[1] = cast(s32) ((1.77200 / 2) * (1'u32 << 16) + 0.5)
        fix[2] = cast(s32) -((0.71414 / 2) * (1'u32 << 16) + 0.5)
        fix[3] = cast(s32) -((0.34414 / 2) * (1'u32 << 16) + 0.5)

        loop i: 255
        {
            k := (i * 2) - 256
            crr[i] = (fix[0] * k + (1's32 << (16 - 1))) >>,safe 16
            cbb[i] = (fix[1] * k + (1's32 << (16 - 1))) >>,safe 16
            crg[i] = (fix[2] * k)
            cbg[i] = (fix[3] * k + (1's32 << (16 - 1)))
        }
    }

    func init_scan(using self)->bool throw
    {
        res := try locate_sos_marker()
        if !res return false

        calc_mcu_block_order()
        try check_huff_tables()
        try check_quant_tables()

        memory.clear(last_dc_val, cast(uint) comps_in_frame * @sizeof(u32))

        eob_run = 0
        if restart_interval
        {
            restarts_left    = restart_interval
            next_restart_num = 0
        }

        fix_in_buffer()
        return true
    }

    func stuff_char(using self, q: u8)
    {
        inBufOfs -= 1
        :inBufOfs = q
        inBufLeft += 1
    }

    func fix_in_buffer(using self)
    {
        if bits_left == 16
            stuff_char(cast(u8) ((bit_buf >>,safe 16) & 0xFF))
        if bits_left >= 8
            stuff_char(cast(u8) ((bit_buf >>,safe 24) & 0xFF))

        stuff_char(cast(u8) (bit_buf & 0xFF))
        stuff_char(cast(u8) ((bit_buf >>,safe 8) & 0xFF))
        bits_left = 16

        discard get_bits_2(16)
        discard get_bits_2(16)
    }

    func check_quant_tables(using self) throw
    {
        loop i: comps_in_scan
        {
            if quant[compQuant[comp_list[i]]].count == 0
                throw "undefined quant table"
        }
    }

    func make_huff_table(using self, _index: s32, _hs: *HuffTables)
    {
        var huffsize: [257] u8
        var huffcode: [257] u32

        p := 0
        for l := 1; l <= 16; l += 1
        {
            for i := 1; i <= huff_num[_index].buffer[l]; i += 1
            {
                huffsize[p] = cast(u8) l
                p += 1
            }
        }

        huffsize[p] = 0
        lastp := p
        cde := 0'u32
        si := huffsize[0]
        p = 0

        while huffsize[p]
        {
            while huffsize[p] == si
            {
                huffcode[p] = cde
                p, cde += 1
            }

            cde <<=,safe 1
            si += 1
        }

        memory.clear(_hs.lookUp, @sizeof(_hs.lookUp))
        memory.clear(_hs.tree, @sizeof(_hs.tree))
        memory.clear(_hs.codeSize, @sizeof(_hs.codeSize))

        nextfreeentry := -1's32
        p = 0

        while p < lastp
        {
            i := huff_val[_index].buffer[p]
            cde = huffcode[p];
            codeSize := huffsize[p]
            _hs.codeSize[i] = cast(u8) codeSize
            if codeSize <= 8
            {
                cde <<= (8 - codeSize)
                for l := 1 << (8 - codeSize); l > 0; l -= 1
                {
                    _hs.lookUp[cde] = cast(s32) i
                    cde += 1
                }
            }
            else
            {
                subtree := (cde >>,safe (codeSize - 8)) & 0xFF
                currententry := _hs.lookUp[subtree]
                if currententry == 0
                {
                    _hs.lookUp[subtree] = nextfreeentry
                    currententry = nextfreeentry
                    nextfreeentry -= 2
                }

                cde <<= (16 - (codeSize - 8))
                for l := codeSize; l > 9; l -= 1
                {
                    if (cde & 0x8000) == 0
                        currententry -= 1

                    if _hs.tree[-currententry - 1] == 0
                    {
                        _hs.tree[-currententry - 1] = nextfreeentry
                        currententry = nextfreeentry
                        nextfreeentry -= 2
                    }
                    else
                        currententry = _hs.tree[-currententry - 1]

                    cde <<= 1
                }

                if (cde & 0x8000) == 0
                    currententry -= 1
                _hs.tree[-currententry - 1] = cast(s32) i
            }

            p += 1
        }
    }

    func check_huff_tables(using self) throw
    {
        loop i: comps_in_scan
        {
            if (spectral_start == 0) && (huff_num[comp_dc_tab[comp_list[i]]].count == 0)
                throw "undefined huff table"

            if (spectral_end > 0) && (huff_num[comp_ac_tab[comp_list[i]]].count == 0)
                throw "undefined huff table"
        }

        loop i: MaxHuffTables
        {
            if huff_num[i].count
            {
                make_huff_table(i, &h[i])
            }
        }

        loop i: blocks_per_mcu
        {
            dc_huff_seg[i] = &h[comp_dc_tab[mcu_org[i]]]
            ac_huff_seg[i] = &h[comp_ac_tab[mcu_org[i]]]
            component[i]   = &last_dc_val[mcu_org[i]]
        }
    }

    func calc_mcu_block_order(using self)
    {
        max_h_samp := 0
        max_v_samp := 0

        loop cid: comps_in_frame
        {
            if compSampH[cid] > max_h_samp
                max_h_samp = compSampH[cid]
            if compSampV[cid] > max_v_samp
                max_v_samp = compSampV[cid]
        }

        loop cid: comps_in_frame
        {
            comp_h_blocks[cid] = ((((imageSizeX * compSampH[cid]) + (max_h_samp - 1)) / max_h_samp) + 7) / 8
            comp_v_blocks[cid] = ((((imageSizeY * compSampV[cid]) + (max_v_samp - 1)) / max_v_samp) + 7) / 8
        }

        if (comps_in_scan == 1)
        {
            mcus_per_row = comp_h_blocks[comp_list[0]]
            mcus_per_col = comp_v_blocks[comp_list[0]]
        }
        else
        {
            mcus_per_row = (((imageSizeX + 7) / 8) + (max_h_samp - 1)) / max_h_samp
            mcus_per_col = (((imageSizeY + 7) / 8) + (max_v_samp - 1)) / max_v_samp
        }

        if comps_in_scan == 1
        {
            mcu_org[0] = comp_list[0]
            blocks_per_mcu = 1
        }
        else
        {
            blocks_per_mcu = 0
            loop component_num: comps_in_scan
            {
                cid := comp_list[component_num]
                num_blocks := compSampH[cid] * compSampV[cid]
                while num_blocks
                {
                    mcu_org[blocks_per_mcu] = cid
                    blocks_per_mcu += 1
                    num_blocks -= 1
                }
            }
        }
    }

    func coeff_buf_open(_block_num_x, _block_num_y, _block_len_x, _block_len_y: s32)->CoeffBuf
    {
        var result: retval
        result.blockNumX = _block_num_x;
        result.blockNumY = _block_num_y;
        result.blockLenX = _block_len_x;
        result.blockLenY = _block_len_y;
        result.blockSize  = (_block_len_x * _block_len_y) * @sizeof(s16)
        result.pdata.resize(cast(uint) result.blockSize * _block_num_x * _block_num_y)
        return result
    }

    #[swag.safety("of", false)]
    func decode_block_dc_first(using self, _component_id: s32, _block_x, _block_y: s32) throw
    {
        p := coeff_buf_getp(dc_coeffs[_component_id], _block_x, _block_y)
        s := huff_decode(&h[comp_dc_tab[_component_id]])
        if s != 0
        {
            r := get_bits_2(s)
            s = cast(s32) (r < extend_test[s] ? r + extend_off[s] : r)
        }

        s += cast(s32) last_dc_val[_component_id]
        last_dc_val[_component_id] = cast(u32) s
        p[0] = cast(s16) (s << cast(u32) successive_low)
    }

    #[swag.safety("of", false)]
    func decode_block_dc_refine(using self, _component_id: s32, _block_x, _block_y: s32) throw
    {
        if get_bits_2(1)
        {
            p := coeff_buf_getp(dc_coeffs[_component_id], _block_x, _block_y)
            p[0] |= (1 << cast(u32) successive_low)
        }
    }

    #[swag.safety("of", false)]
    func decode_block_ac_first(using self, _component_id: s32, _block_x, _block_y: s32) throw
    {
        if eob_run
        {
            eob_run -= 1
            return
        }

        p := coeff_buf_getp(ac_coeffs[_component_id], _block_x, _block_y)

        for k := spectral_start; k <= spectral_end; k += 1
        {
            s := huff_decode(&h[comp_ac_tab[_component_id]])
            r := s >> 4
            s &= 0x0F;

            if s
            {
                k += r
                if k > 63 throw "decode error"
                r = cast(s32) get_bits_2(s)
                if s > 15 throw "decode error"

                s = (r < extend_test[s] ? r + extend_off[s] : r)
                p[zag[k]] = cast(s16) (s << cast(u32) successive_low)
            }
            else
            {
                if r == 15
                {
                    k += 15
                    if k > 63
                        throw "decode error"
                }
                else
                {
                    eob_run = 1 << cast(u32) r
                    if r
                        eob_run += cast(s32) get_bits_2(r)
                    eob_run -= 1
                    break
                }
            }
        }
    }

    #[swag.safety("of", false)]
    func decode_block_ac_refine(using self, _component_id: s32, _block_x, _block_y: s32) throw
    {
        p1 := 1 << cast(u32) successive_low
        m1 := (-1) << cast(u32) successive_low
        p := coeff_buf_getp(ac_coeffs[_component_id], _block_x, _block_y)
        k := spectral_start
        if eob_run == 0
        {
            if spectral_end >= 64 throw "decode error"

            while k <= spectral_end
            {
                s := huff_decode(&h[comp_ac_tab[_component_id]])
                r := s >> 4
                s &= 15

                if s
                {
                    if s != 1 throw "decode error"
                    if get_bits_2(1)
                        s = p1
                    else
                        s = m1
                }
                else
                {
                    if r != 15
                    {
                        eob_run = 1 << cast(u32) r
                        if (r)
                            eob_run += cast(s32) get_bits_2(r)
                        break
                    }
                }

                while true
                {
                    this_coef := p + zag[k]
                    if :this_coef != 0
                    {
                        if get_bits_2(1)
                        {
                            if (:this_coef & cast(s16) p1) == 0
                            {
                                if :this_coef >= 0
                                    :this_coef += cast(s16) p1
                                else
                                    :this_coef += cast(s16) m1
                            }
                        }
                    }
                    else
                    {
                        r -= 1
                        if r < 0
                            break
                    }

                    k += 1
                    if k > spectral_end
                        break
                }

                if s && (k < 64)
                {
                    p[zag[k]] = cast(s16) s
                }

                k += 1
            }
        }

        if eob_run > 0
        {
            if spectral_end >= 64 throw "decode error"
            while k <= spectral_end
            {
                this_coef := p + zag[k]
                if :this_coef != 0
                {
                    if get_bits_2(1)
                    {
                        if (:this_coef & cast(s16) p1) == 0
                        {
                            if :this_coef >= 0
                                :this_coef += cast(s16) p1
                            else
                                :this_coef += cast(s16) m1
                        }
                    }
                }

                k += 1
            }

            eob_run -= 1
        }
    }

    func decode_scan(using self, fn: func(*Decoder, s32, s32, s32) throw) throw
    {
        var block_x_mcut: [MaxComponents] s32
        var block_y_mcut: [MaxComponents] s32

        loop mcu_col: mcus_per_col
        {
            memory.clear(block_x_mcut, @sizeof(block_x_mcut))
            loop mcu_row: mcus_per_row
            {
                block_x_mcu_ofs := 0
                block_y_mcu_ofs := 0

                if restart_interval && (restarts_left == 0)
                    try process_restart()

                loop mcu_block: blocks_per_mcu
                {
                    component_id := mcu_org[mcu_block]
                    try fn(self, component_id, block_x_mcut[component_id] + block_x_mcu_ofs, block_y_mcut[component_id] + block_y_mcu_ofs)

                    if comps_in_scan == 1
                        block_x_mcut[component_id] += 1
                    else
                    {
                        block_x_mcu_ofs += 1
                        if block_x_mcu_ofs == compSampH[component_id]
                        {
                            block_x_mcu_ofs = 0
                            block_y_mcu_ofs += 1
                            if block_y_mcu_ofs == compSampV[component_id]
                            {
                                block_y_mcu_ofs = 0
                                block_x_mcut[component_id] += compSampH[component_id]
                            }
                        }
                    }
                }

                restarts_left -= 1
            }

            if comps_in_scan == 1
                block_y_mcut[comp_list[0]] += 1
            else
            {
                loop component_num: comps_in_scan
                {
                    component_id := comp_list[component_num]
                    block_y_mcut[component_id] += compSampV[component_id]
                }
            }
        }
    }

    func init_progressive(using self) throw
    {
        //decode_block_func decode_block_func;
        if comps_in_frame == 4 throw "unsupported color space"

        loop i: comps_in_frame
        {
            dc_coeffs[i] = coeff_buf_open(max_mcus_per_row * compSampH[i], max_mcus_per_col * compSampV[i], 1, 1)
            ac_coeffs[i] = coeff_buf_open(max_mcus_per_row * compSampH[i], max_mcus_per_col * compSampV[i], 8, 8)
        }

        while true
        {
            res := try init_scan()
            if (!res) break

            dc_only_scan    := (spectral_start == 0)
            refinement_scan := (successive_high != 0)

            if (spectral_start > spectral_end) || (spectral_end > 63)
                throw "bad sos spectral"

            if dc_only_scan
            {
                if spectral_end
                    throw "bad sos spectral"
            }
            else if comps_in_scan != 1
            {
                throw "bad sos spectral"
            }

            if refinement_scan && (successive_low != successive_high - 1)
                throw "bad sos successive"

            var decode_block_func: func(*Decoder, s32, s32, s32) throw
            if dc_only_scan
            {
                if refinement_scan
                    decode_block_func = &decode_block_dc_refine
                else
                    decode_block_func = &decode_block_dc_first
            }
            else
            {
                if refinement_scan
                    decode_block_func = &decode_block_ac_refine
                else
                    decode_block_func = &decode_block_ac_first
            }

            try decode_scan(decode_block_func)

            bits_left = 16

            discard get_bits_1(16)
            discard get_bits_1(16)
        }

        comps_in_scan = comps_in_frame
        loop i: comps_in_frame
            comp_list[i] = i
        calc_mcu_block_order()
    }

    func init_sequential(using self) throw
    {
        res := try init_scan()
        if !res throw "unexpected marker"
    }

    #[swag.safety("of", false)]
    func get_bits_1(using self, num_bits: s32)->u32
    {
        i := bit_buf >> cast(u32) ((16 - num_bits)) & ((1'u32 << cast(u32) num_bits) - 1)
        bits_left -= num_bits
        if bits_left <= 0
        {
            bit_buf = jpeg_rol(bit_buf, cast(u32) (num_bits + bits_left))
            c1      := get_char()
            c2      := get_char()
            bit_buf = (bit_buf & 0xFFFF) | (cast(u32) c1 << 24) | (cast(u32) c2 << 16)
            bit_buf = jpeg_rol(bit_buf, cast(u32) -bits_left)
            bits_left += 16
        }
        else
            bit_buf = jpeg_rol(bit_buf, cast(u32) num_bits)

        return i
    }

    #[swag.safety("of", false)]
    func get_bits_2(using self, _numbits: s32)->u32
    {
        i := (bit_buf >> cast(u32) (16 - _numbits)) & ((1'u32 << cast(u32) _numbits) - 1)
        bits_left -= _numbits
        if bits_left <= 0
        {
            bit_buf = jpeg_rol(bit_buf, cast(u32) (_numbits + bits_left))
            c1      := get_octet();
            c2      := get_octet();
            bit_buf = (bit_buf & 0xFFFF) | ((cast(u32) c1) << 24) | ((cast(u32) c2) << 16)
            bit_buf = jpeg_rol(bit_buf, cast(u32) -bits_left)
            bits_left += 16
        }
        else
            bit_buf = jpeg_rol(bit_buf, cast(u32) _numbits)

        return i
    }

    func get_octet(using self)->u8
    {
        var padding_flag: bool
        c := get_char(&padding_flag)
        if c == 0xFF
        {
            if padding_flag
                return 0xFF

            c = get_char(&padding_flag)
            if padding_flag
            {
                stuff_char(0xFF)
                return 0xFF
            }

            if c == 0x00
                return 0xFF

            stuff_char(cast(u8) c)
            stuff_char(0xFF)
            return 0xFF
        }

        return cast(u8) c
    }

    func get_char(using self, paddingFlag: *bool)->u32
    {
        if !inBufLeft
        {
            prepareStreamBuffer()
            if !inBufLeft
            {
                :paddingFlag = true
                t := tem_flag
                tem_flag ^= 1
                if t return 0xD9
                return 0xFF
            }
        }

        :paddingFlag = false
        c := :inBufOfs
        inBufOfs += 1
        inBufLeft -= 1
        return c
    }

    func get_char(using self)->u32
    {
        if !inBufLeft
        {
            prepareStreamBuffer()
            if !inBufLeft
            {
                t := tem_flag
                tem_flag ^= 1
                if t return 0xD9
                return 0xFF
            }
        }

        c := inBufOfs[0]
        inBufOfs += 1
        inBufLeft -= 1
        return c
    }

    func next_marker(using self)->Marker
    {
        bytes := 0
        var c: u32

        while true
        {
            while true
            {
                bytes += 1
                c = get_bits_1(8)
                if c == 0xFF break
            }

            while true
            {
                c = get_bits_1(8)
                if c != 0xFF break
            }

            if c break
        }

        return cast(Marker) c
    }

    func process_markers(using self)->Marker throw
    {
        while true
        {
            c := next_marker()
            switch c
            {
            case M_SOF0, M_SOF1, M_SOF2, M_SOF3, M_SOF5, M_SOF6,
                 M_SOF7, M_SOF9, M_SOF10, M_SOF11, M_SOF13, M_SOF14,
                 M_SOF15, M_SOI, M_EOI, M_SOS:
                return c
            case M_JPG, M_RST0, M_RST1, M_RST2, M_RST3, M_RST4,
                 M_RST5, M_RST6, M_RST7, M_TEM:
                throw "unexpected marker"
            case M_DAC:
                throw "no arithmetic support"
            case M_DHT:
                try read_dht_marker()
            case M_DQT:
                try read_dqt_marker()
            case M_DRI:
                try read_dri_marker()
            default:
                try read_variable_marker(c)
            }
        }

        return Marker.M_ERROR
    }

    func read_variable_marker(using self, marker: Marker) throw
    {
        left := get_bits_1(16)
        if left < 2 throw "bad variable marker"
        left -= 2

        @assert(!accept_meta)
        while left
        {
            discard get_bits_1(8)
            left -= 1
        }
    }

    func read_dht_marker(using self) throw
    {
        var huff_numt: [17] u8
        var huff_valt: [256] u8

        left := get_bits_1(16)
        if left < 2 throw "bad dht marker"
        left -= 2

        while left
        {
            index := get_bits_1(8);
            count := 0
            huff_numt[0] = 0
            for i := 1; i <= 16; i += 1
            {
                huff_numt[i] = cast(u8) get_bits_1(8)
                count += cast(s32) huff_numt[i]
            }

            if count > 255 throw "bad dht counts"
            loop i: count
            {
                huff_valt[i] = cast(u8) get_bits_1(8)
            }

            i := 1 + 16 + count
            if left < i throw "bad dht marker"

            left -= cast(u32) i
            if (index & 0x10) > 0x10 throw "bad dht index"

            index = (index & 0x0F) + ((index & 0x10) >> 4) * (MaxHuffTables >> 1)
            if index >= MaxHuffTables throw "bad dht index"

            huff_num[index].resize(17)
            huff_val[index].resize(256)
            memory.copy(huff_num[index].buffer, huff_numt, 17)
            memory.copy(huff_val[index].buffer, huff_valt, 256)
        }
    }

    func read_dqt_marker(using self) throw
    {
        left := get_bits_1(16)
        if left < 2 throw "bad dqt marker"
        left -= 2

        while left
        {
            n    := get_bits_1(8)
            prec := n >>,safe 4
            n &= 0x0F
            if n >= MaxQuantTables throw "bad dqt table"

            quant[n].resize(64)
            loop i: 64
            {
                temp := get_bits_1(8)
                if prec temp = (temp << 8) + get_bits_1(8)
                quant[n].buffer[i] = cast(s16) temp
            }

            i := 64'u32 + 1
            if prec i += 64
            if left < i throw "bad dqt length"
            left -= i
        }
    }

    func read_dri_marker(using self) throw
    {
        if get_bits_1(16) != 4 throw "bad dri length"
        restart_interval = cast(s32) get_bits_1(16)
    }

    func read_sof_marker(using self) throw
    {
        left := get_bits_1(16)
        if get_bits_1(8) != 8 throw "bad precision"

        imageSizeY = cast(s32) get_bits_1(16)
        imageSizeX = cast(s32) get_bits_1(16)

        if (imageSizeY < 1) || (imageSizeY > MaxHeight) throw "bad height"
        if (imageSizeX < 1) || (imageSizeX > MaxWidth) throw "bad width"

        comps_in_frame = cast(s32) get_bits_1(8)
        if comps_in_frame > MaxComponents throw "too many components"
        if left != (comps_in_frame * 3 + 8) throw "bad sqf length"

        loop i: comps_in_frame
        {
            compIdent[i]  = cast(s32) get_bits_1(8)
            compSampH[i] = cast(s32) get_bits_1(4)
            compSampV[i] = cast(s32) get_bits_1(4)
            compQuant[i]  = cast(s32) get_bits_1(8)
        }
    }

    func read_sos_marker(using self) throw
    {
        left := get_bits_1(16)
        n    := get_bits_1(8)
        comps_in_scan = cast(s32) n
        left -= 3

        if (left != cast(u32)(n * 2 + 3)) || (n < 1) || (n > MaxCompsInScan)
            throw "bad sos length"

        loop i: n
        {
            cc := get_bits_1(8)
            c  := get_bits_1(8)
            left -= 2

            ci := 0
            while ci < comps_in_frame
            {
                if (cc == compIdent[ci])
                    break;
                ci += 1
            }

            if ci >= comps_in_frame
                throw "bad sos comp id"

            comp_list[i]    = ci
            comp_dc_tab[ci] = cast(s32) (c >>,safe 4) & 15
            comp_ac_tab[ci] = cast(s32) (c & 15) + (MaxHuffTables >> 1)
        }

        spectral_start  = cast(s32) get_bits_1(8)
        spectral_end    = cast(s32) get_bits_1(8)
        successive_high = cast(s32) get_bits_1(4)
        successive_low  = cast(s32) get_bits_1(4)

        if !progressive_flag
        {
            spectral_start = 0
            spectral_end   = 63
        }

        left -= 3
        while left
        {
            discard get_bits_1(8)
            left -= 1
        }
    }

    func locate_sos_marker(using self)->bool throw
    {
        c := try process_markers()
        if c == .M_EOI
            return false
        else if c != .M_SOS
            throw "unexpected marker"
        try read_sos_marker()
        return true
    }

    func locate_soi_marker(using self) throw
    {
        lastchar := get_bits_1(8)
        thischar := get_bits_1(8)
        if (lastchar == 0xFF) && (thischar == Marker.M_SOI)
            return
        bytesleft := 512
        while true
        {
            bytesleft -= 1
            if bytesleft == 0 throw "not a jpeg"
            lastchar = thischar
            thischar = get_bits_1(8)
            if (lastchar == 0xFF) && (thischar == Marker.M_SOI)
                break
        }

        thischar = (bit_buf >> 8) & 0xFF
        if thischar != 0xFF throw "not a jpeg"
    }

    func locate_sof_marker(using self) throw
    {
        try locate_soi_marker()
        c := try process_markers()
        switch c
        {
        case M_SOF2:
            progressive_flag = true
            try read_sof_marker()
        case M_SOF0, M_SOF1:
            try read_sof_marker()
        case M_SOF9:
            throw "no arithmetic support"
        default:
            throw "unsupported marker"
        }
    }

    func word_clear(p: *void, c: u16, n: u32)
    {
        ps := cast(*u16) p
        loop n
        {
            :ps = c
            ps += 1
        }
    }

    func prepareStreamBuffer(using self)
    {
        inBufLeft = 0
        inBufOfs = in_buf
        if eofReached return

        while true
        {
            size := InBufSize - inBufLeft
            if size + streamSeek > streamLen
            {
                size     = streamLen - streamSeek
                eofReached = 1
            }

            bytes_read := size
            memory.copy(&in_buf[0] + inBufLeft, stream + streamSeek, cast(uint) size)
            streamSeek += size
            inBufLeft += bytes_read
            if inBufLeft >= InBufSize || eofReached
                break
        }

        totalBytesRead += inBufLeft
        word_clear(inBufOfs + inBufLeft, 0xD9FF, 64)
    }
}

public impl IDecoder for Decoder
{
    func canDecode(using self, fileName: string)->bool
    {
        ext := path.getExtensionLowerCase(fileName)
        return ext == ".jpg" || ext == ".jpeg"
    }

    func decode(using self, buffer: const [..] u8)->Image throw
    {
        try
        {
            init(buffer)
            start()

            var result: retval = create(imageSizeX, imageSizeY, PixelFormat.BGR8)
            doit(result.pixels)
        }

        return result
    }
}