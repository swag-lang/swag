#global namespace jpg
using swag, core

const INBUFSIZE         = 4096
const MAX_HEIGHT        = 8192
const MAX_WIDTH         = 8192
const MAXCOMPONENTS     = 4
const MAXQUANTTABLES    = 4
const MAXHUFFTABLES     = 8
const MAXBLOCKSPERMCU   = 10
const MAXBLOCKSPERROW   = 6144
const MAXCOMPSINSCAN    = 4
const GRAYSCALE         = 0
const YH1V1             = 1
const YH2V1             = 2
const YH1V2             = 3
const YH2V2             = 4
const FAILED            = -1
const DONE              = 1
const OKAY              = 0

public struct huff_tables_t
{
    look_up:    [256] s32
    code_size:  [256] u8
    tree:       [512] s32
}

public struct Decoder
{
    stream:             const *u8
    in_buf_ofs:         *u8
    psample_buf:        *u8

    image_x_size:       s32
    image_y_size:       s32
    streamlen:          s32
    streamseek:         s32
    bit_buf:            u32
    bits_left:          s32
    in_buf_left:        s32
    tem_flag:           s32
    eof_flag:           s32
    total_bytes_read:   s32
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

    in_buf:             [INBUFSIZE + 128] u8
    comp_h_samp:        [MAXCOMPONENTS] s32
    comp_v_samp:        [MAXCOMPONENTS] s32
    comp_quant:         [MAXCOMPONENTS] s32
    comp_ident:         [MAXCOMPONENTS] s32
    block_max_zag_set:  [MAXBLOCKSPERROW] s32
    block_seg:          [MAXBLOCKSPERROW] *s16
    comp_list:          [MAXCOMPSINSCAN] s32
    comp_dc_tab:        [MAXCOMPONENTS] s32
    comp_ac_tab:        [MAXCOMPONENTS] s32
    comp_h_blocks:      [MAXCOMPONENTS] s32
    comp_v_blocks:      [MAXCOMPONENTS] s32
    mcu_org:            [MAXBLOCKSPERMCU] s32
    h:                  [MAXHUFFTABLES] huff_tables_t
    dc_huff_seg:        [MAXBLOCKSPERMCU] *huff_tables_t
    ac_huff_seg:        [MAXBLOCKSPERMCU] *huff_tables_t
    component:          [MAXBLOCKSPERMCU] *u32
    last_dc_val:        [MAXCOMPONENTS] u32
    temp_block:         [64] s16

    crr:                [256] s32
    cbb:                [256] s32
    crg:                [256] s32
    cbg:                [256] s32

    huff_num:           [MAXHUFFTABLES] Array'u8
    huff_val:           [MAXHUFFTABLES] Array'u8
    quant:              [MAXQUANTTABLES] Array's16
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

#[inline, safety("of", false)]
func jpeg_rol(i, j: u32)->u32 => cast(u32) ((i << j) | (i >> (32 - j)))

impl Decoder
{
    func decode_init(using self, buffer: const [..] u8) throw
    {
        @init(self)
        stream = @dataof(buffer)
        streamlen = cast(s32) @countof(buffer)

        in_buf_ofs = in_buf
        prep_in_buffer()
        bits_left = 16
        discard get_bits_1(16)
        discard get_bits_1(16)
        loop i: MAXBLOCKSPERROW block_max_zag_set[i] = 64

        try locate_sof_marker()
    }

    func decode_start(using self) throw
    {
        try init_frame()
        if progressive_flag
            init_progressive()
        else
            try init_sequential()
    }

    func decode_process(using self, buf: *u8) throw
    {
        var scan_line_ofs: *void
        var scan_line_len: u32

        // Gray scale (1BPP)
        if comps_in_frame == 1
        {
            pdest := buf
            pdest += cast(uint) ((image_y_size - 1) * image_x_size * 3)
            while true
            {
                if try decode(&scan_line_ofs, &scan_line_len)
                    break

                pdest1 := pdest
                psrc := cast(*u8) scan_line_ofs
                loop image_x_size
                {
                    pdest1[0] = :psrc
                    pdest1[1] = :psrc
                    pdest1[2] = :psrc
                    pdest1 += 3
                    psrc += 1
                }

                pdest -= cast(uint) (image_x_size * 3)
            }
        }

        // 3 BPP
        else
        {
            pdest := buf
            pdest += cast(uint) ((image_y_size - 1) * image_x_size * 3)
            while true
            {
                if try decode(&scan_line_ofs, &scan_line_len)
                    break
                memory.copy(pdest, scan_line_ofs, cast(uint) image_x_size * 3)
                pdest -= cast(uint) (image_x_size * 3)
            }
        }
    }

    func load_next_row(using self)
    {

    }

    func decode_next_row(using self)
    {

    }

    func idct(using self, _data: *s16, _pdst_ptr: *u8)
    {
    }

    func transform_row(using self)
    {
        psrc_ptr := block_seg[0]
        pdst_ptr := psample_buf

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

        total_bytes_read -= in_buf_left
    }

    func decode(using self, scan_line_ofs: **void, scan_line_len: *u32)->s32 throw
    {
        if total_lines_left == 0
            return DONE

        if mcu_lines_left == 0
        {
            if progressive_flag
                load_next_row()
            else
                decode_next_row()
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

        case GRAYSCALE:
            GrayConvert()
            :scan_line_ofs = scan_line_0.buffer
        }

        :scan_line_len = cast(u32) real_dest_bytes_per_scan_line
        mcu_lines_left -= 1
        total_lines_left -= 1

        //return OKAY
        return DONE
    }

    func GrayConvert(using self)
    {
    }

    func H1V1Convert(using self)
    {
    }

    func H1V2Convert(using self)
    {
    }

    func H2V1Convert(using self)
    {
    }

    func H2V2Convert(using self)
    {
    }

    func init_frame(using self) throw
    {
        if comps_in_frame == 1
        {
            scan_type          = GRAYSCALE
            max_blocks_per_mcu = 1
            max_mcu_x_size     = 8
            max_mcu_y_size     = 8
        }
        else if comps_in_frame == 3
        {
            if (comp_h_samp[1] != 1) || (comp_v_samp[1] != 1) || (comp_h_samp[2] != 1) || (comp_v_samp[2] != 1)
                throw "unsupported samp factors"

            if (comp_h_samp[0] == 1) && (comp_v_samp[0] == 1)
            {
                scan_type          = YH1V1
                max_blocks_per_mcu = 3
                max_mcu_x_size     = 8
                max_mcu_y_size     = 8
            }
            else if (comp_h_samp[0] == 2) && (comp_v_samp[0] == 1)
            {
                scan_type          = YH2V1
                max_blocks_per_mcu = 4
                max_mcu_x_size     = 16
                max_mcu_y_size     = 8
            }
            else if (comp_h_samp[0] == 1) && (comp_v_samp[0] == 2)
            {
                scan_type          = YH1V2
                max_blocks_per_mcu = 4
                max_mcu_x_size     = 8
                max_mcu_y_size     = 16
            }
            else if (comp_h_samp[0] == 2) && (comp_v_samp[0] == 2)
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

        max_mcus_per_row = (image_x_size + (max_mcu_x_size - 1)) / max_mcu_x_size
        max_mcus_per_col = (image_y_size + (max_mcu_y_size - 1)) / max_mcu_y_size

        if scan_type == GRAYSCALE
            dest_bytes_per_pixel = 1
        else
            dest_bytes_per_pixel = 4

        dest_bytes_per_scan_line      = ((image_x_size + 15) & 0xFFF0) * dest_bytes_per_pixel
        real_dest_bytes_per_scan_line = (image_x_size * dest_bytes_per_pixel)

        scan_line_0.resize(cast(uint) dest_bytes_per_scan_line + 8)
        memory.clear(scan_line_0.buffer, cast(uint) dest_bytes_per_scan_line)

        scan_line_1.resize(cast(uint) dest_bytes_per_scan_line + 8)
        memory.clear(scan_line_1.buffer, cast(uint) dest_bytes_per_scan_line)

        max_blocks_per_row = max_mcus_per_row * max_blocks_per_mcu
        if max_blocks_per_row > MAXBLOCKSPERROW
            throw "assertion error"

        q_buf.resize(cast(uint) max_blocks_per_row * 64 * @sizeof(s16) + 8)
        loop i: max_blocks_per_row
            block_seg[i] = cast(*s16) (q_buf.buffer + i * 64 * @sizeof(s16))
        loop i: max_blocks_per_row
            block_max_zag_set[i] = 64

        sample_buf.resize(cast(uint) max_blocks_per_row * 64 + 8)
        psample_buf = sample_buf.buffer

        total_lines_left = image_y_size
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
        in_buf_ofs -= 1
        :in_buf_ofs = q
        in_buf_left += 1
    }

    func fix_in_buffer(using self)
    {
        if bits_left == 16
            stuff_char(cast(u8) (bit_buf >>,safe 16) & 0xFF)
        if bits_left >= 8
            stuff_char(cast(u8) (bit_buf >>,safe 24) & 0xFF)

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
            if quant[comp_quant[comp_list[i]]].count == 0
                throw "undefined quant table"
        }
    }

    func make_huff_table(using self, _index: s32, _hs: *huff_tables_t)
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

        memory.clear(_hs.look_up, @sizeof(_hs.look_up))
        memory.clear(_hs.tree, @sizeof(_hs.tree))
        memory.clear(_hs.code_size, @sizeof(_hs.code_size))

        nextfreeentry := -1's32
        p = 0

        while p < lastp
        {
            i := huff_val[_index].buffer[p]
            cde = huffcode[p];
            code_size := huffsize[p]
            _hs.code_size[i] = cast(u8) code_size
            if code_size <= 8
            {
                cde <<= (8 - code_size)
                for l := 1 << (8 - code_size); l > 0; l -= 1
                {
                    _hs.look_up[cde] = cast(s32) i
                    cde += 1
                }
            }
            else
            {
                subtree := (cde >>,safe (code_size - 8)) & 0xFF
                currententry := _hs.look_up[subtree]
                if currententry == 0
                {
                    _hs.look_up[subtree] = nextfreeentry
                    currententry = nextfreeentry
                    nextfreeentry -= 2
                }

                cde <<= (16 - (code_size - 8))
                for l := code_size; l > 9; l -= 1
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

        loop i: MAXHUFFTABLES
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
            if comp_h_samp[cid] > max_h_samp
                max_h_samp = comp_h_samp[cid]
            if comp_v_samp[cid] > max_v_samp
                max_v_samp = comp_v_samp[cid]
        }

        loop cid: comps_in_frame
        {
            comp_h_blocks[cid] = ((((image_x_size * comp_h_samp[cid]) + (max_h_samp - 1)) / max_h_samp) + 7) / 8
            comp_v_blocks[cid] = ((((image_y_size * comp_v_samp[cid]) + (max_v_samp - 1)) / max_v_samp) + 7) / 8
        }

        if (comps_in_scan == 1)
        {
            mcus_per_row = comp_h_blocks[comp_list[0]]
            mcus_per_col = comp_v_blocks[comp_list[0]]
        }
        else
        {
            mcus_per_row = (((image_x_size + 7) / 8) + (max_h_samp - 1)) / max_h_samp
            mcus_per_col = (((image_y_size + 7) / 8) + (max_v_samp - 1)) / max_v_samp
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
                num_blocks := comp_h_samp[cid] * comp_v_samp[cid]
                while num_blocks
                {
                    mcu_org[blocks_per_mcu] = cid
                    blocks_per_mcu += 1
                    num_blocks -= 1
                }
            }
        }
    }

    func init_progressive(using self)
    {
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
        if !in_buf_left
        {
            prep_in_buffer()
            if !in_buf_left
            {
                :paddingFlag = true
                t := tem_flag
                tem_flag ^= 1
                if t return 0xD9
                return 0xFF
            }
        }

        :paddingFlag = false
        c := :in_buf_ofs
        in_buf_ofs += 1
        in_buf_left -= 1
        return c
    }

    func get_char(using self)->u32
    {
        if !in_buf_left
        {
            prep_in_buffer()
            if !in_buf_left
            {
                t := tem_flag
                tem_flag ^= 1
                if t return 0xD9
                return 0xFF
            }
        }

        c := in_buf_ofs[0]
        in_buf_ofs += 1
        in_buf_left -= 1
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

            index = (index & 0x0F) + ((index & 0x10) >> 4) * (MAXHUFFTABLES >> 1)
            if index >= MAXHUFFTABLES throw "bad dht index"

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
            if n >= MAXQUANTTABLES throw "bad dqt table"

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

        image_y_size = cast(s32) get_bits_1(16)
        image_x_size = cast(s32) get_bits_1(16)

        if (image_y_size < 1) || (image_y_size > MAX_HEIGHT) throw "bad height"
        if (image_x_size < 1) || (image_x_size > MAX_WIDTH) throw "bad width"

        comps_in_frame = cast(s32) get_bits_1(8)
        if comps_in_frame > MAXCOMPONENTS throw "too many components"
        if left != (comps_in_frame * 3 + 8) throw "bad sqf length"

        loop i: comps_in_frame
        {
            comp_ident[i]  = cast(s32) get_bits_1(8)
            comp_h_samp[i] = cast(s32) get_bits_1(4)
            comp_v_samp[i] = cast(s32) get_bits_1(4)
            comp_quant[i]  = cast(s32) get_bits_1(8)
        }
    }

    func read_sos_marker(using self) throw
    {
        left := get_bits_1(16)
        n    := get_bits_1(8)
        comps_in_scan = cast(s32) n
        left -= 3

        if (left != cast(u32)(n * 2 + 3)) || (n < 1) || (n > MAXCOMPSINSCAN)
            throw "bad sos length"

        loop i: n
        {
            cc := get_bits_1(8)
            c  := get_bits_1(8)
            left -= 2

            ci := 0
            while ci < comps_in_frame
            {
                if (cc == comp_ident[ci])
                    break;
                ci += 1
            }

            if ci >= comps_in_frame
                throw "bad sos comp id"

            comp_list[i]    = ci
            comp_dc_tab[ci] = cast(s32) (c >>,safe 4) & 15
            comp_ac_tab[ci] = cast(s32) (c & 15) + (MAXHUFFTABLES >> 1)
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

    func prep_in_buffer(using self)
    {
        in_buf_left = 0
        in_buf_ofs = in_buf
        if eof_flag return

        while true
        {
            size := INBUFSIZE - in_buf_left
            if size + streamseek > streamlen
            {
                size     = streamlen - streamseek
                eof_flag = 1
            }

            bytes_read := size
            memory.copy(&in_buf[0] + in_buf_left, stream + streamseek, cast(uint) size)
            streamseek += size
            in_buf_left += bytes_read
            if in_buf_left >= INBUFSIZE || eof_flag
                break
        }

        total_bytes_read += in_buf_left
        word_clear(in_buf_ofs + in_buf_left, 0xD9FF, 64)
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
        try decode_init(buffer)
        try decode_start()

        var result: retval = create(image_x_size, image_y_size, PixelFormat.BGR8)
        try decode_process(result.pixels)

        return result
    }
}