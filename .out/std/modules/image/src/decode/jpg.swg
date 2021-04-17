#global namespace jpg
using swag, core

const INBUFSIZE      = 4096
const MAX_HEIGHT     = 8192
const MAX_WIDTH      = 8192
const MAXCOMPONENTS  = 4

public struct Decoder
{
    image_x_size:       s32
    image_y_size:       s32
    stream:             const *u8
    streamlen:          s32
    streamseek:         s32
    in_buf_ofs:         *u8
    bit_buf:            u32
    bits_left:          s32
    in_buf_left:        s32
    tem_flag:           s32
    eof_flag:           s32
    total_bytes_read:   s32
    comps_in_frame:     s32
    restart_interval:   s32
    accept_meta:        bool
    progressive_flag:   bool
    in_buf:             [INBUFSIZE + 128] u8
    comp_h_samp:        [MAXCOMPONENTS] s32
    comp_v_samp:        [MAXCOMPONENTS] s32
    comp_quant:         [MAXCOMPONENTS] s32
    comp_ident:         [MAXCOMPONENTS] s32
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

        try locate_sof_marker()
    }

    func init_frame()
    {
    }

    func init_progressive()
    {
    }

    func init_sequential()
    {
    }

    func decode_start(using self) throw
    {
        init_frame()
        if progressive_flag
            init_progressive()
        else
            init_sequential()
    }

    func decode_process(pdest: *u8) throw
    {
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
            case M_DHT: try read_dht_marker()
            case M_DQT: try read_dqt_marker()
            case M_DRI: try read_dri_marker()
            default:    try read_variable_marker(c)
            case M_DAC: throw "no arithmetic support"
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

    }

    func read_dqt_marker(using self) throw
    {

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

    func locate_sof_marker(using self) throw
    {
        try locate_soi_marker()
        c := try process_markers()
        switch c
        {
        case M_SOF2:
            progressive_flag = true
            fallthrough
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