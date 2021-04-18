#include "pch.h"
#include "sImageDecoderJpeg.hpp"
#define uint uint32_t

void sMemory_Clear(void* p, int s)
{
    memset(p, 0, s);
}

void sMemory_Copy(void* p, const void* d, int s)
{
    memcpy(p, d, s);
}

#define sAssert(__a)

// clang-format off
static const int extend_off[16] = {0,
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
                                    ((-1) << 15) + 1};

static const int extend_test[16] = {
    0, 0x0001, 0x0002, 0x0004, 0x0008, 0x0010, 0x0020, 
    0x0040, 0x0080, 0x0100, 0x0200, 0x0400, 0x0800, 
    0x1000, 0x2000, 0x4000
};

static const uint zag[64] = {
    0, 1, 8, 16, 9, 2, 3, 10, 17, 24, 32, 25, 18, 11, 4, 5,
    12, 19, 26, 33, 40, 48, 41, 34, 27, 20, 13, 6, 7, 14, 21,
    28, 35, 42, 49, 56, 57, 50, 43, 36, 29, 22, 15, 23, 30, 37,
    44, 51, 58, 59, 52, 45, 38, 31, 39, 46, 53, 60, 61, 54, 47,
    55, 62, 63,
};
// clang-format on

#define jpeg_descale(x, n) (((x) + (((int) 1) << ((n) -1))) >> (n))
#define jpeg_clamp(i) ((uint8)(((i) &0xFFFFFF00) ? (((~(i)) >> 31) & 0xFF) : i))
#define jpeg_clampd(i) ((uint8)(((i) &0xFF00) ? (((~(i)) >> 15) & 0xFF) : i))
#define jpeg_rol(i, j) ((uint)(((i) << (j)) | ((i) >> (32 - (j)))))

/*
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/
sImageDecoderJpeg::sImageDecoderJpeg()
{
}

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
bool sImageDecoderJpeg::Decode()
{
    uint i;

    FILE*    f       = fopen("F:/swag/.out/std/tests/image/datas/yh2v1.jpg", "rb");
    uint8_t* _stream = (uint8_t*) malloc(1 * 1024 * 1024);
    auto _streamlen = fread(_stream, 1, 1 * 1024 * 1024, f);
    fclose(f);

    uint8_t* destBuf    = (uint8_t*) malloc(1 * 1024 * 1024);

    decode_init(_stream, _streamlen);
    decode_start();
    decode_process(destBuf);

    return error_code ? false : true;
}

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
uint sImageDecoderJpeg::get_char()
{
    uint c;
    int  t;

    if (!in_buf_left)
    {
        prep_in_buffer();

        if (!in_buf_left)
        {
            t = tem_flag;
            tem_flag ^= 1;
            if (t)
                return (0xD9);
            else
                return (0xFF);
        }
    }

    c = *Pin_buf_ofs++;
    in_buf_left--;
    return (c);
}

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
uint sImageDecoderJpeg::get_char(bool* _ppadding_flag)
{
    uint c;
    int  t;

    if (!in_buf_left)
    {
        prep_in_buffer();
        if (!in_buf_left)
        {
            *_ppadding_flag = true;
            t               = tem_flag;
            tem_flag ^= 1;
            if (t)
                return (0xD9);
            else
                return (0xFF);
        }
    }

    *_ppadding_flag = false;
    c               = *Pin_buf_ofs++;
    in_buf_left--;
    return (c);
}

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void sImageDecoderJpeg::stuff_char(uint8 _q)
{
    *(--Pin_buf_ofs) = _q;
    in_buf_left++;
}

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
uint8 sImageDecoderJpeg::get_octet()
{
    bool padding_flag;
    int  c;

    c = get_char(&padding_flag);
    if (c == 0xFF)
    {
        if (padding_flag)
            return (0xFF);

        c = get_char(&padding_flag);
        if (padding_flag)
        {
            stuff_char(0xFF);
            return (0xFF);
        }

        if (c == 0x00)
            return (0xFF);
        else
        {
            stuff_char((uint8) c);
            stuff_char(0xFF);
            return (0xFF);
        }
    }

    return ((uint8) c);
}

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
uint sImageDecoderJpeg::get_bits_1(int _num_bits)
{
    uint i;
    uint c1, c2;

    i = (bit_buf >> (16 - _num_bits)) & ((1 << _num_bits) - 1);
    if ((bits_left -= _num_bits) <= 0)
    {
        _num_bits += bits_left;
        bit_buf = jpeg_rol(bit_buf, (uint8) _num_bits);
        c1      = get_char();
        c2      = get_char();
        bit_buf = (bit_buf & 0xFFFF) | (((uint) c1) << 24) | (((uint) c2) << 16);
        bit_buf = jpeg_rol(bit_buf, (uint8)(-bits_left));
        bits_left += 16;
    }
    else
        bit_buf = jpeg_rol(bit_buf, (uint8) _num_bits);

    return i;
}

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
uint sImageDecoderJpeg::get_bits_2(int _numbits)
{
    uint i;
    uint c1, c2;

    i = (bit_buf >> (16 - _numbits)) & ((1 << _numbits) - 1);
    if ((bits_left -= _numbits) <= 0)
    {
        _numbits += bits_left;
        bit_buf = jpeg_rol(bit_buf, (uint8) _numbits);
        c1      = get_octet();
        c2      = get_octet();
        bit_buf = (bit_buf & 0xFFFF) | (((uint) c1) << 24) | (((uint) c2) << 16);
        bit_buf = jpeg_rol(bit_buf, (uint8)(-bits_left));
        bits_left += 16;
    }
    else
        bit_buf = jpeg_rol(bit_buf, (uint8) _numbits);

    return i;
}

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
int sImageDecoderJpeg::huff_decode(huff_tables_pt _ph)
{
    int symbol;

    if ((symbol = _ph->look_up[(bit_buf >> 8) & 0xFF]) < 0)
    {
        get_bits_2(8);

        do
        {
            symbol = _ph->tree[~symbol + (1 - get_bits_2(1))];
        } while (symbol < 0);
    }
    else
        get_bits_2(_ph->code_size[symbol]);

    return symbol;
}

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void sImageDecoderJpeg::free_all_blocks()
{
    for (int i = 0; i < MAXBLOCKS; i++)
    {
        if (blocks[i])
        {
            delete blocks[i];
            blocks[i] = nullptr;
        }
    }
}

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void sImageDecoderJpeg::terminate(const char* _status)
{
    /*sString str;
    str.Format("Jpeg decoder error [%s]", _status);
    sDiagnostic::Error(str);*/
    error_code = _status;
    free_all_blocks();
    throw(_status);
}

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void* sImageDecoderJpeg::alloc(int _n)
{
    uint8* q;
    int    i;

    for (i = 0; i < MAXBLOCKS; i++)
    {
        if (blocks[i] == nullptr)
            break;
    }

    if (i == MAXBLOCKS)
        terminate("Too many blocks");
    q = (uint8*) malloc(_n + 8);
    if (q == nullptr)
        terminate("Not enough memory");

    sMemory_Clear(q, _n + 8);
    blocks[i] = q;
    return ((void*) (((uint64) q + 7) & ~7));
}

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void sImageDecoderJpeg::word_clear(void* _p, uint16 _c, uint _n)
{
    uint16* ps = (uint16*) _p;

    while (_n)
    {
        *ps++ = _c;
        _n--;
    }
}

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void sImageDecoderJpeg::prep_in_buffer()
{
    int  bytes_read;
    uint size;

    in_buf_left = 0;
    Pin_buf_ofs = in_buf;

    if (eof_flag)
        return;

    do
    {
        size = INBUFSIZE - in_buf_left;
        if (size + streamseek > streamlen)
        {
            size     = streamlen - streamseek;
            eof_flag = 1;
        }

        bytes_read = size;
        sMemory_Copy(in_buf + in_buf_left, stream + streamseek, size);
        streamseek += size;
        in_buf_left += bytes_read;
    } while ((in_buf_left < INBUFSIZE) && (!eof_flag));

    total_bytes_read += in_buf_left;
    word_clear(Pin_buf_ofs + in_buf_left, 0xD9FF, 64);
}

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void sImageDecoderJpeg::read_dht_marker()
{
    int   i, index, count;
    uint  left;
    uint8 huff_numt[17];
    uint8 huff_valt[256];

    left = get_bits_1(16);

    if (left < 2)
        terminate("Bad dht marker");

    left -= 2;

    while (left)
    {
        index        = get_bits_1(8);
        huff_numt[0] = 0;
        count        = 0;
        for (i = 1; i <= 16; i++)
        {
            huff_numt[i] = (uint8) get_bits_1(8);
            count += huff_numt[i];
        }

        if (count > 255)
        {
            terminate("Bad dht counts");
            sAssert(0);
        }

        for (i = 0; i < count; i++)
            huff_valt[i] = (uint8) get_bits_1(8);

        i = 1 + 16 + count;
        if (left < (uint) i)
        {
            terminate("Bad dht marker");
            sAssert(0);
        }

        left -= i;
        if ((index & 0x10) > 0x10)
        {
            terminate("Bad dht index");
            sAssert(0);
        }

        index = (index & 0x0F) + ((index & 0x10) >> 4) * (MAXHUFFTABLES >> 1);
        if (index >= MAXHUFFTABLES)
        {
            terminate("Bad dht index");
            sAssert(0);
        }

        if (!this->huff_num[index])
            this->huff_num[index] = (uint8*) alloc(17);
        if (!this->huff_val[index])
            this->huff_val[index] = (uint8*) alloc(256);

        sMemory_Copy(this->huff_num[index], huff_numt, 17);
        sMemory_Copy(this->huff_val[index], huff_valt, 256);
    }
}

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void sImageDecoderJpeg::read_dqt_marker()
{
    int  n, i, prec;
    uint left;
    uint temp;

    left = get_bits_1(16);

    if (left < 2)
    {
        terminate("Bad dqt marker");
        sAssert(0);
    }

    left -= 2;

    while (left)
    {
        n    = get_bits_1(8);
        prec = n >> 4;
        n &= 0x0F;

        if (n >= MAXQUANTTABLES)
        {
            terminate("Bad dqt table");
            sAssert(0);
        }

        if (!quant[n])
            quant[n] = (int16*) alloc(64 * sizeof(int16));

        for (i = 0; i < 64; i++)
        {
            temp = get_bits_1(8);

            if (prec)
                temp = (temp << 8) + get_bits_1(8);
            quant[n][i] = (uint8) temp;
        }

        i = 64 + 1;

        if (prec)
            i += 64;
        if (left < (uint) i)
        {
            terminate("Bad dqt length");
            sAssert(0);
        }

        left -= i;
    }
}

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void sImageDecoderJpeg::read_sof_marker()
{
    int  i;
    uint left;

    left = get_bits_1(16);

    if (get_bits_1(8) != 8)
        terminate("Bad precision");

    image_y_size = get_bits_1(16);
    if ((image_y_size < 1) || (image_y_size > MAX_HEIGHT))
        terminate("Bad height");

    image_x_size = get_bits_1(16);
    if ((image_x_size < 1) || (image_x_size > MAX_WIDTH))
        terminate("Bad width");

    comps_in_frame = get_bits_1(8);

    if (comps_in_frame > MAXCOMPONENTS)
        terminate("Too many components");
    if (left != (uint)(comps_in_frame * 3 + 8))
        terminate("Bad sqf length");

    for (i = 0; i < comps_in_frame; i++)
    {
        comp_ident[i]  = get_bits_1(8);
        comp_h_samp[i] = get_bits_1(4);
        comp_v_samp[i] = get_bits_1(4);
        comp_quant[i]  = get_bits_1(8);
    }
}

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void sImageDecoderJpeg::get_variable_marker(int _marker)
{
    uint   left;
    uint8* p;
    uint   cpt = 0;

    left = get_bits_1(16);
    if (left < 2)
        terminate("Bad variable marker");
    left -= 2;

    while (left)
    {
        get_bits_1(8);
        left--;
    }
}

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void sImageDecoderJpeg::read_dri_marker()
{
    if (get_bits_1(16) != 4)
        terminate("Bad dri length");
    restart_interval = get_bits_1(16);
}

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void sImageDecoderJpeg::read_sos_marker()
{
    uint left;
    int  i, ci, n, c, cc;

    left          = get_bits_1(16);
    n             = get_bits_1(8);
    comps_in_scan = n;
    left -= 3;
    if ((left != (uint)(n * 2 + 3)) || (n < 1) || (n > MAXCOMPSINSCAN))
        terminate("Bad sos length");

    for (i = 0; i < n; i++)
    {
        cc = get_bits_1(8);
        c  = get_bits_1(8);
        left -= 2;

        for (ci = 0; ci < comps_in_frame; ci++)
            if (cc == comp_ident[ci])
                break;
        if (ci >= comps_in_frame)
            terminate("Bad sos comp id");

        comp_list[i]    = ci;
        comp_dc_tab[ci] = (c >> 4) & 15;
        comp_ac_tab[ci] = (c & 15) + (MAXHUFFTABLES >> 1);
    }

    spectral_start  = get_bits_1(8);
    spectral_end    = get_bits_1(8);
    successive_high = get_bits_1(4);
    successive_low  = get_bits_1(4);

    if (!progressive_flag)
    {
        spectral_start = 0;
        spectral_end   = 63;
    }

    left -= 3;
    while (left)
    {
        get_bits_1(8);
        left--;
    }
}

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
int sImageDecoderJpeg::next_marker()
{
    uint c, bytes;

    bytes = 0;
    do
    {
        do
        {
            bytes++;
            c = get_bits_1(8);
        } while (c != 0xFF);

        do
        {
            c = get_bits_1(8);
        } while (c == 0xFF);
    } while (c == 0);

    return c;
}

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
int sImageDecoderJpeg::process_markers()
{
    int c;

    for (;;)
    {
        c = next_marker();

        switch (c)
        {
        case M_SOF0:
        case M_SOF1:
        case M_SOF2:
        case M_SOF3:
        case M_SOF5:
        case M_SOF6:
        case M_SOF7:
        case M_SOF9:
        case M_SOF10:
        case M_SOF11:
        case M_SOF13:
        case M_SOF14:
        case M_SOF15:
        case M_SOI:
        case M_EOI:
        case M_SOS:
            return c;

        case M_DHT:
            read_dht_marker();
            break;

        case M_DAC:
            terminate("No arithmetic support");
            break;

        case M_DQT:
            read_dqt_marker();
            break;

        case M_DRI:
            read_dri_marker();
            break;

        case M_JPG:
        case M_RST0:
        case M_RST1:
        case M_RST2:
        case M_RST3:
        case M_RST4:
        case M_RST5:
        case M_RST6:
        case M_RST7:
        case M_TEM:
            terminate("Unexpected marker");
            break;

        default:
            get_variable_marker(c);
            break;
        }
    }
}

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void sImageDecoderJpeg::locate_soi_marker()
{
    uint lastchar, thischar;
    uint bytesleft;

    lastchar = get_bits_1(8);
    thischar = get_bits_1(8);
    if ((lastchar == 0xFF) && (thischar == M_SOI))
        return;

    bytesleft = 512;
    for (;;)
    {
        if (--bytesleft == 0)
            terminate("Not a jpeg");
        lastchar = thischar;
        thischar = get_bits_1(8);
        if ((lastchar == 0xFF) && (thischar == M_SOI))
            break;
    }

    thischar = (bit_buf >> 8) & 0xFF;
    if (thischar != 0xFF)
        terminate("Not a jpeg");
}

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void sImageDecoderJpeg::locate_sof_marker()
{
    int c;

    locate_soi_marker();
    c = process_markers();
    switch (c)
    {
    case M_SOF2:
        progressive_flag = true;

    case M_SOF0:
    case M_SOF1:
        read_sof_marker();
        break;

    case M_SOF9:
        terminate("No arithmetic support");
        break;

    default:
        terminate("Unsupported marker");
        break;
    }
}

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
int sImageDecoderJpeg::locate_sos_marker()
{
    int c;

    c = process_markers();
    if (c == M_EOI)
        return false;
    else if (c != M_SOS)
        terminate("Unexpected marker");

    read_sos_marker();
    return true;
}

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void sImageDecoderJpeg::init(const uint8* _stream, uint _streamlen)
{
    error_code = nullptr;

    image_x_size = image_y_size = 0;

    stream     = _stream;
    streamlen  = _streamlen;
    streamseek = 0;

    progressive_flag = false;

    sMemory_Clear(huff_num, sizeof(huff_num));
    sMemory_Clear(huff_val, sizeof(huff_val));
    sMemory_Clear(quant, sizeof(quant));

    scan_type      = 0;
    comps_in_frame = 0;

    sMemory_Clear(comp_h_samp, sizeof(comp_h_samp));
    sMemory_Clear(comp_v_samp, sizeof(comp_v_samp));
    sMemory_Clear(comp_quant, sizeof(comp_quant));
    sMemory_Clear(comp_ident, sizeof(comp_ident));
    sMemory_Clear(comp_h_blocks, sizeof(comp_h_blocks));
    sMemory_Clear(comp_v_blocks, sizeof(comp_v_blocks));

    comps_in_scan = 0;
    sMemory_Clear(comp_list, sizeof(comp_list));
    sMemory_Clear(comp_dc_tab, sizeof(comp_dc_tab));
    sMemory_Clear(comp_ac_tab, sizeof(comp_ac_tab));

    spectral_start  = 0;
    spectral_end    = 0;
    successive_low  = 0;
    successive_high = 0;

    max_mcu_x_size = 0;
    max_mcu_y_size = 0;

    blocks_per_mcu     = 0;
    max_blocks_per_row = 0;
    mcus_per_row       = 0;
    mcus_per_col       = 0;

    sMemory_Clear(mcu_org, sizeof(mcu_org));

    total_lines_left = 0;
    mcu_lines_left   = 0;

    real_dest_bytes_per_scan_line = 0;
    dest_bytes_per_scan_line      = 0;
    dest_bytes_per_pixel          = 0;

    sMemory_Clear(blocks, sizeof(blocks));
    sMemory_Clear(h, sizeof(h));
    sMemory_Clear(dc_coeffs, sizeof(dc_coeffs));
    sMemory_Clear(ac_coeffs, sizeof(ac_coeffs));
    sMemory_Clear(block_y_mcu, sizeof(block_y_mcu));

    eob_run = 0;

    sMemory_Clear(block_y_mcu, sizeof(block_y_mcu));

    Pin_buf_ofs = in_buf;
    in_buf_left = 0;
    eof_flag    = false;
    tem_flag    = 0;

    sMemory_Clear(padd_1, sizeof(padd_1));
    sMemory_Clear(in_buf, sizeof(in_buf));
    sMemory_Clear(padd_2, sizeof(padd_2));

    restart_interval = 0;
    restarts_left    = 0;
    next_restart_num = 0;

    max_mcus_per_row   = 0;
    max_blocks_per_mcu = 0;
    max_mcus_per_col   = 0;

    sMemory_Clear(component, sizeof(component));
    sMemory_Clear(last_dc_val, sizeof(last_dc_val));
    sMemory_Clear(dc_huff_seg, sizeof(dc_huff_seg));
    sMemory_Clear(ac_huff_seg, sizeof(ac_huff_seg));
    sMemory_Clear(block_seg, sizeof(block_seg));
    Psample_buf = nullptr;

    total_bytes_read = 0;

    prep_in_buffer();

    bits_left     = 16;
    bit_buf_64[0] = 0;
    bit_buf_64[1] = 0;

    get_bits_1(16);
    get_bits_1(16);

    for (int i = 0; i < MAXBLOCKSPERROW; i++)
        block_max_zag_set[i] = 64;
}

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void sImageDecoderJpeg::create_look_ups()
{
    int i, k;
    int fix[4];

    fix[0] = (int) ((1.40200 / 2) * (1L << 16) + 0.5);
    fix[1] = (int) ((1.77200 / 2) * (1L << 16) + 0.5);
    fix[2] = (int) -((0.71414 / 2) * (1L << 16) + 0.5);
    fix[3] = (int) -((0.34414 / 2) * (1L << 16) + 0.5);

    for (i = 0; i <= 255; i++)
    {
        k = (i * 2) - 256;

        crr[i] = (fix[0] * k + ((int) 1 << (16 - 1))) >> 16;
        cbb[i] = (fix[1] * k + ((int) 1 << (16 - 1))) >> 16;

        crg[i] = fix[2] * k;
        cbg[i] = fix[3] * k + ((int) 1 << (16 - 1));
    }
}

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void sImageDecoderJpeg::fix_in_buffer()
{
    if (bits_left == 16)
        stuff_char((uint8)((bit_buf >> 16) & 0xFF));
    if (bits_left >= 8)
        stuff_char((uint8)((bit_buf >> 24) & 0xFF));

    stuff_char((uint8)(bit_buf & 0xFF));
    stuff_char((uint8)((bit_buf >> 8) & 0xFF));
    bits_left = 16;

    get_bits_2(16);
    get_bits_2(16);
}

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void sImageDecoderJpeg::transform_row()
{
    int16* Psrc_ptr = block_seg[0];
    uint8* Pdst_ptr = Psample_buf;

    for (int i = max_blocks_per_row; i > 0; i--)
    {
        sMemory_Copy(temp_block, Psrc_ptr, 64 * sizeof(int16));
        idct(temp_block, Pdst_ptr);
        Psrc_ptr += 64;
        Pdst_ptr += 64;
    }
}

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
sImageDecoderJpeg::coeff_buf_pt sImageDecoderJpeg::coeff_buf_open(int _block_num_x, int _block_num_y, int _block_len_x, int _block_len_y)
{
    coeff_buf_pt cb = (coeff_buf_pt) alloc(sizeof(coeff_buf_t));

    cb->block_num_x = _block_num_x;
    cb->block_num_y = _block_num_y;
    cb->block_len_x = _block_len_x;
    cb->block_len_y = _block_len_y;
    cb->block_size  = (_block_len_x * _block_len_y) * sizeof(int16);
    cb->Pdata       = (uint8*) alloc(cb->block_size * _block_num_x * _block_num_y);
    return cb;
}

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void sImageDecoderJpeg::coeff_buf_read(coeff_buf_pt _cb, int _block_x, int _block_y, int16* _buffer)
{
    if (_block_x >= _cb->block_num_x)
        terminate("Assertion error");
    if (_block_y >= _cb->block_num_y)
        terminate("Assertion error");

    sMemory_Copy(_buffer, _cb->Pdata + _block_x * _cb->block_size + _block_y * (_cb->block_size * _cb->block_num_x), _cb->block_size);
}

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void sImageDecoderJpeg::coeff_buf_write(coeff_buf_pt _cb, int _block_x, int _block_y, int16* _buffer)
{
    if (_block_x >= _cb->block_num_x)
        terminate("Assertion error");
    if (_block_y >= _cb->block_num_y)
        terminate("Assertion error");

    sMemory_Copy(_cb->Pdata + _block_x * _cb->block_size + _block_y * (_cb->block_size * _cb->block_num_x), _buffer, _cb->block_size);
}

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
int16* sImageDecoderJpeg::coeff_buf_getp(coeff_buf_pt _cb, int _block_x, int _block_y)
{
    if (_block_x >= _cb->block_num_x)
        terminate("Assertion error");
    if (_block_y >= _cb->block_num_y)
        terminate("Assertion error");
    return (int16*) (_cb->Pdata + _block_x * _cb->block_size + _block_y * (_cb->block_size * _cb->block_num_x));
}

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void sImageDecoderJpeg::load_next_row()
{
    int    i;
    int16* p;
    int16* q;
    int    mcu_row, mcu_block, row_block = 0;
    int    component_num, component_id;
    int    block_x_mcu[MAXCOMPONENTS];
    int    block_x_mcu_ofs, block_y_mcu_ofs;
    int16 *pAC, *pDC;

    sMemory_Clear(block_x_mcu, MAXCOMPONENTS * sizeof(int));
    for (mcu_row = 0; mcu_row < mcus_per_row; mcu_row++)
    {
        block_x_mcu_ofs = 0;
        block_y_mcu_ofs = 0;
        for (mcu_block = 0; mcu_block < blocks_per_mcu; mcu_block++)
        {
            component_id = mcu_org[mcu_block];

            p   = block_seg[row_block];
            q   = quant[comp_quant[component_id]];
            pAC = coeff_buf_getp(ac_coeffs[component_id], block_x_mcu[component_id] + block_x_mcu_ofs, block_y_mcu[component_id] + block_y_mcu_ofs);
            pDC = coeff_buf_getp(dc_coeffs[component_id], block_x_mcu[component_id] + block_x_mcu_ofs, block_y_mcu[component_id] + block_y_mcu_ofs);

            p[0] = pDC[0];
            sMemory_Copy(&p[1], &pAC[1], 63 * sizeof(int16));

            for (i = 63; i > 0; i--)
                if (p[zag[i]])
                    break;

            for (; i >= 0; i--)
                if (p[zag[i]])
                    p[zag[i]] *= q[i];

            row_block++;
            if (comps_in_scan == 1)
                block_x_mcu[component_id]++;
            else
            {
                if (++block_x_mcu_ofs == comp_h_samp[component_id])
                {
                    block_x_mcu_ofs = 0;

                    if (++block_y_mcu_ofs == comp_v_samp[component_id])
                    {
                        block_y_mcu_ofs = 0;

                        block_x_mcu[component_id] += comp_h_samp[component_id];
                    }
                }
            }
        }
    }

    if (comps_in_scan == 1)
        block_y_mcu[comp_list[0]]++;
    else
    {
        for (component_num = 0; component_num < comps_in_scan; component_num++)
        {
            component_id = comp_list[component_num];

            block_y_mcu[component_id] += comp_v_samp[component_id];
        }
    }
}

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void sImageDecoderJpeg::process_restart()
{
    int i, c;

    for (i = 1536; i > 0; i--)
        if (get_char() == 0xFF)
            break;
    if (i == 0)
        terminate("Bad restart marker");

    c = 0;
    for (; i > 0; i--)
        if ((c = get_char()) != 0xFF)
            break;
    if (i == 0)
        terminate("Bad restart marker");
    if (c != (next_restart_num + M_RST0))
        terminate("Bad restart marker");

    sMemory_Clear(&last_dc_val, comps_in_frame * sizeof(uint));

    eob_run          = 0;
    restarts_left    = restart_interval;
    next_restart_num = (next_restart_num + 1) & 7;
    bits_left        = 16;

    get_bits_2(16);
    get_bits_2(16);
}

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void sImageDecoderJpeg::decode_next_row()
{
    int            row_block = 0;
    int            kt, n;
    int            component_id;
    int16 *        p, *q;
    int            r, s;
    int            prev_num_set;
    huff_tables_pt Ph;
    int            k;

    for (int mcu_row = 0; mcu_row < mcus_per_row; mcu_row++)
    {
        if ((restart_interval) && (restarts_left == 0))
            process_restart();

        for (int mcu_block = 0; mcu_block < blocks_per_mcu; mcu_block++)
        {
            component_id = mcu_org[mcu_block];
            p            = block_seg[row_block];
            q            = quant[comp_quant[component_id]];

            if ((s = huff_decode(h[comp_dc_tab[component_id]])) != 0)
            {
                r = get_bits_2(s);
                s = (r < extend_test[s] ? r + extend_off[s] : r);
            }

            last_dc_val[component_id] = (s += last_dc_val[component_id]);

            p[0]         = (int16)(s * q[0]);
            prev_num_set = block_max_zag_set[row_block];
            Ph           = h[comp_ac_tab[component_id]];

            for (k = 1; k < 64; k++)
            {
                s = huff_decode(Ph);

                r = s >> 4;
                s &= 0x0F;

                if (s)
                {
                    if (r)
                    {
                        if ((k + r) > 63)
                        {
                            terminate("Decode error");
                            sAssert(0);
                        }

                        if (k < prev_num_set)
                        {
                            n  = min(r, prev_num_set - k);
                            kt = k;
                            while (n--)
                                p[zag[kt++]] = 0;
                        }

                        k += r;
                    }

                    r = get_bits_2(s);

                    if (s > 15)
                    {
                        terminate("Decode error");
                        sAssert(0);
                    }

                    s         = (r < extend_test[s] ? r + extend_off[s] : r);
                    p[zag[k]] = (int16)(s * q[k]);
                }
                else
                {
                    if (r == 15)
                    {
                        if ((k + 15) > 63)
                            terminate("Decode error");

                        if (k < prev_num_set)
                        {
                            n  = min((uint) 16, (uint)(prev_num_set - k));
                            kt = k;
                            while (n--)
                                p[zag[kt++]] = 0;
                        }

                        k += 15;
                    }
                    else
                    {
                        break;
                    }
                }
            }

            if (k < prev_num_set)
            {
                kt = k;
                while (kt < prev_num_set)
                    p[zag[kt++]] = 0;
            }

            block_max_zag_set[row_block] = k;

            row_block++;
        }

        restarts_left--;
    }
}

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void sImageDecoderJpeg::H1V1Convert()
{
    int    row = max_mcu_y_size - mcu_lines_left;
    uint8* d   = scan_line_0;
    uint8* s   = Psample_buf + row * 8;
    int    y, cb, cr;

    for (int i = max_mcus_per_row; i > 0; i--)
    {
        for (int j = 0; j < 8; j++)
        {
            y  = s[j];
            cb = s[64 + j];
            cr = s[128 + j];

            d[2] = jpeg_clamp(y + crr[cr]);
            d[1] = jpeg_clamp(y + ((crg[cr] + cbg[cb]) >> 16));
            d[0] = jpeg_clamp(y + cbb[cb]);
            d += 3;
        }

        s += 64 * 3;
    }
}

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void sImageDecoderJpeg::H2V1Convert()
{
    int    row = max_mcu_y_size - mcu_lines_left;
    uint8* d0  = scan_line_0;
    uint8* y   = Psample_buf + row * 8;
    uint8* c   = Psample_buf + 2 * 64 + row * 8;
    int    cb, cr, rc, gc, bc, yy;

    for (int i = max_mcus_per_row; i > 0; i--)
    {
        for (int l = 0; l < 2; l++)
        {
            for (int j = 0; j < 4; j++)
            {
                cb = c[0];
                cr = c[64];
                rc = crr[cr];
                gc = ((crg[cr] + cbg[cb]) >> 16);
                bc = cbb[cb];
                yy = y[j << 1];

                d0[2] = jpeg_clamp(yy + rc);
                d0[1] = jpeg_clamp(yy + gc);
                d0[0] = jpeg_clamp(yy + bc);

                yy    = y[(j << 1) + 1];
                d0[5] = jpeg_clamp(yy + rc);
                d0[4] = jpeg_clamp(yy + gc);
                d0[3] = jpeg_clamp(yy + bc);

                d0 += 6;

                c++;
            }

            y += 64;
        }

        y += 64 * 4 - 64 * 2;
        c += 64 * 4 - 8;
    }
}

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void sImageDecoderJpeg::H1V2Convert()
{
    int    row = max_mcu_y_size - mcu_lines_left;
    uint8* d0  = scan_line_0;
    uint8* d1  = scan_line_1;
    uint8* y;
    uint8* c;
    int    cb, cr, rc, gc, bc, yy;

    if (row < 8)
        y = Psample_buf + row * 8;
    else
        y = Psample_buf + 64 * 1 + (row & 7) * 8;

    c = Psample_buf + 64 * 2 + (row >> 1) * 8;

    for (int i = max_mcus_per_row; i > 0; i--)
    {
        for (int j = 0; j < 8; j++)
        {
            cb = c[0 + j];
            cr = c[64 + j];
            rc = crr[cr];
            gc = ((crg[cr] + cbg[cb]) >> 16);
            bc = cbb[cb];
            yy = y[j];

            d0[2] = jpeg_clamp(yy + rc);
            d0[1] = jpeg_clamp(yy + gc);
            d0[0] = jpeg_clamp(yy + bc);

            yy    = y[8 + j];
            d1[2] = jpeg_clamp(yy + rc);
            d1[1] = jpeg_clamp(yy + gc);
            d1[0] = jpeg_clamp(yy + bc);

            d0 += 3;
            d1 += 3;
        }

        y += 64 * 4;
        c += 64 * 4;
    }
}

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void sImageDecoderJpeg::GrayConvert()
{
    int    row = max_mcu_y_size - mcu_lines_left;
    uint8* d   = scan_line_0;
    uint8* s   = Psample_buf + row * 8;

    for (int i = max_mcus_per_row; i > 0; i--)
    {
        d[0] = s[0];
        d[1] = s[1];
        d[2] = s[2];
        d[3] = s[3];
        d[4] = s[4];
        d[5] = s[5];
        d[6] = s[6];
        d[7] = s[7];

        s += 64;
        d += 8;
    }
}

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void sImageDecoderJpeg::find_eoi()
{
    if (!progressive_flag)
    {
        bits_left = 16;
        get_bits_1(16);
        get_bits_1(16);
        process_markers();
    }

    total_bytes_read -= in_buf_left;
}

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
int sImageDecoderJpeg::decode(void** _pscan_line_ofs, uint* _pscan_line_len)
{
    if (error_code)
        return (FAILED);
    if (total_lines_left == 0)
        return (DONE);

    if (mcu_lines_left == 0)
    {
        if (progressive_flag)
            load_next_row();
        else
            decode_next_row();

        if (total_lines_left <= max_mcu_y_size)
            find_eoi();
        transform_row();
        mcu_lines_left = max_mcu_y_size;
    }

    switch (scan_type)
    {
    case YH2V2:
        if ((mcu_lines_left & 1) == 0)
        {
            H2V2Convert();
            *_pscan_line_ofs = scan_line_0;
        }
        else
            *_pscan_line_ofs = scan_line_1;
        break;

    case YH2V1:
        H2V1Convert();
        *_pscan_line_ofs = scan_line_0;
        break;

    case YH1V2:
        if ((mcu_lines_left & 1) == 0)
        {
            H1V2Convert();
            *_pscan_line_ofs = scan_line_0;
        }
        else
            *_pscan_line_ofs = scan_line_1;
        break;

    case YH1V1:
        H1V1Convert();
        *_pscan_line_ofs = scan_line_0;
        break;

    case GRAYSCALE:
        GrayConvert();
        *_pscan_line_ofs = scan_line_0;

        break;
    }

    *_pscan_line_len = real_dest_bytes_per_scan_line;
    mcu_lines_left--;
    total_lines_left--;

    return (OKAY);
}

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void sImageDecoderJpeg::make_huff_table(int _index, huff_tables_pt _hs)
{
    int   p, i, l, si;
    uint8 huffsize[257];
    uint  huffcode[257];
    uint  code;
    uint  subtree;
    int   code_size;
    int   lastp;
    int   nextfreeentry;
    int   currententry;

    p = 0;
    for (l = 1; l <= 16; l++)
    {
        for (i = 1; i <= huff_num[_index][l]; i++)
            huffsize[p++] = (uint8) l;
    }

    huffsize[p] = 0;
    lastp       = p;
    code        = 0;
    si          = huffsize[0];
    p           = 0;

    while (huffsize[p])
    {
        while (huffsize[p] == si)
        {
            huffcode[p++] = code;
            code++;
        }

        code <<= 1;
        si++;
    }

    sMemory_Clear(_hs->look_up, sizeof(_hs->look_up));
    sMemory_Clear(_hs->tree, sizeof(_hs->tree));
    sMemory_Clear(_hs->code_size, sizeof(_hs->code_size));

    nextfreeentry = -1;

    p = 0;

    while (p < lastp)
    {
        i                 = huff_val[_index][p];
        code              = huffcode[p];
        code_size         = huffsize[p];
        _hs->code_size[i] = (uint8) code_size;
        if (code_size <= 8)
        {
            code <<= (8 - code_size);
            for (l = 1 << (8 - code_size); l > 0; l--)
            {
                _hs->look_up[code] = i;
                code++;
            }
        }
        else
        {
            subtree      = (code >> (code_size - 8)) & 0xFF;
            currententry = _hs->look_up[subtree];
            if (currententry == 0)
            {
                _hs->look_up[subtree] = currententry = nextfreeentry;
                nextfreeentry -= 2;
            }

            code <<= (16 - (code_size - 8));
            for (l = code_size; l > 9; l--)
            {
                if ((code & 0x8000) == 0)
                    currententry--;

                if (_hs->tree[-currententry - 1] == 0)
                {
                    _hs->tree[-currententry - 1] = nextfreeentry;
                    currententry                 = nextfreeentry;
                    nextfreeentry -= 2;
                }
                else
                    currententry = _hs->tree[-currententry - 1];

                code <<= 1;
            }

            if ((code & 0x8000) == 0)
                currententry--;
            _hs->tree[-currententry - 1] = i;
        }

        p++;
    }
}

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void sImageDecoderJpeg::check_quant_tables()
{
    int i;

    for (i = 0; i < comps_in_scan; i++)
    {
        if (quant[comp_quant[comp_list[i]]] == nullptr)
            terminate("Undefined quant table");
    }
}

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void sImageDecoderJpeg::check_huff_tables()
{
    int i;

    for (i = 0; i < comps_in_scan; i++)
    {
        if ((spectral_start == 0) && (huff_num[comp_dc_tab[comp_list[i]]] == nullptr))
        {
            terminate("Undefined huff table");
        }

        if ((spectral_end > 0) && (huff_num[comp_ac_tab[comp_list[i]]] == nullptr))
        {
            terminate("Undefined huff table");
        }
    }

    for (i = 0; i < MAXHUFFTABLES; i++)
    {
        if (huff_num[i])
        {
            if (!h[i])
                h[i] = (huff_tables_pt) alloc(sizeof(huff_tables_t));

            make_huff_table(i, h[i]);
        }
    }

    for (i = 0; i < blocks_per_mcu; i++)
    {
        dc_huff_seg[i] = h[comp_dc_tab[mcu_org[i]]];
        ac_huff_seg[i] = h[comp_ac_tab[mcu_org[i]]];
        component[i]   = &last_dc_val[mcu_org[i]];
    }
}

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void sImageDecoderJpeg::calc_mcu_block_order()
{
    int component_num, cid;
    int max_h_samp = 0, max_v_samp = 0;
    int num_blocks;

    for (cid = 0; cid < comps_in_frame; cid++)
    {
        if (comp_h_samp[cid] > max_h_samp)
            max_h_samp = comp_h_samp[cid];
        if (comp_v_samp[cid] > max_v_samp)
            max_v_samp = comp_v_samp[cid];
    }

    for (cid = 0; cid < comps_in_frame; cid++)
    {
        comp_h_blocks[cid] = ((((image_x_size * comp_h_samp[cid]) + (max_h_samp - 1)) / max_h_samp) + 7) / 8;
        comp_v_blocks[cid] = ((((image_y_size * comp_v_samp[cid]) + (max_v_samp - 1)) / max_v_samp) + 7) / 8;
    }

    if (comps_in_scan == 1)
    {
        mcus_per_row = comp_h_blocks[comp_list[0]];
        mcus_per_col = comp_v_blocks[comp_list[0]];
    }
    else
    {
        mcus_per_row = (((image_x_size + 7) / 8) + (max_h_samp - 1)) / max_h_samp;
        mcus_per_col = (((image_y_size + 7) / 8) + (max_v_samp - 1)) / max_v_samp;
    }

    if (comps_in_scan == 1)
    {
        mcu_org[0]     = comp_list[0];
        blocks_per_mcu = 1;
    }
    else
    {
        blocks_per_mcu = 0;
        for (component_num = 0; component_num < comps_in_scan; component_num++)
        {
            cid        = comp_list[component_num];
            num_blocks = comp_h_samp[cid] * comp_v_samp[cid];
            while (num_blocks--)
                mcu_org[blocks_per_mcu++] = cid;
        }
    }
}

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
int sImageDecoderJpeg::init_scan()
{
    if (!locate_sos_marker())
        return false;

    calc_mcu_block_order();
    check_huff_tables();
    check_quant_tables();
    memset(last_dc_val, 0, comps_in_frame * sizeof(uint));
    eob_run = 0;
    if (restart_interval)
    {
        restarts_left    = restart_interval;
        next_restart_num = 0;
    }

    fix_in_buffer();
    return true;
}

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void sImageDecoderJpeg::init_frame()
{
    int    i;
    uint8* q;

    if (comps_in_frame == 1)
    {
        scan_type          = GRAYSCALE;
        max_blocks_per_mcu = 1;
        max_mcu_x_size     = 8;
        max_mcu_y_size     = 8;
    }
    else if (comps_in_frame == 3)
    {
        if (((comp_h_samp[1] != 1) || (comp_v_samp[1] != 1)) || ((comp_h_samp[2] != 1) || (comp_v_samp[2] != 1)))
        {
            terminate("Unsupported samp factors");
        }

        if ((comp_h_samp[0] == 1) && (comp_v_samp[0] == 1))
        {
            scan_type          = YH1V1;
            max_blocks_per_mcu = 3;
            max_mcu_x_size     = 8;
            max_mcu_y_size     = 8;
        }
        else if ((comp_h_samp[0] == 2) && (comp_v_samp[0] == 1))
        {
            scan_type          = YH2V1;
            max_blocks_per_mcu = 4;
            max_mcu_x_size     = 16;
            max_mcu_y_size     = 8;
        }
        else if ((comp_h_samp[0] == 1) && (comp_v_samp[0] == 2))
        {
            scan_type          = YH1V2;
            max_blocks_per_mcu = 4;
            max_mcu_x_size     = 8;
            max_mcu_y_size     = 16;
        }
        else if ((comp_h_samp[0] == 2) && (comp_v_samp[0] == 2))
        {
            scan_type          = YH2V2;
            max_blocks_per_mcu = 6;
            max_mcu_x_size     = 16;
            max_mcu_y_size     = 16;
        }
        else
            terminate("Unsupported samp factors");
    }
    else
        terminate("Unsupported color space");

    max_mcus_per_row = (image_x_size + (max_mcu_x_size - 1)) / max_mcu_x_size;
    max_mcus_per_col = (image_y_size + (max_mcu_y_size - 1)) / max_mcu_y_size;

    if (scan_type == GRAYSCALE)
        dest_bytes_per_pixel = 1;
    else
        dest_bytes_per_pixel = 4;

    dest_bytes_per_scan_line      = ((image_x_size + 15) & 0xFFF0) * dest_bytes_per_pixel;
    real_dest_bytes_per_scan_line = (image_x_size * dest_bytes_per_pixel);

    scan_line_0 = (uint8*) alloc(dest_bytes_per_scan_line + 8);
    sMemory_Clear(scan_line_0, dest_bytes_per_scan_line);

    scan_line_1 = (uint8*) alloc(dest_bytes_per_scan_line + 8);
    sMemory_Clear(scan_line_1, dest_bytes_per_scan_line);

    max_blocks_per_row = max_mcus_per_row * max_blocks_per_mcu;
    if (max_blocks_per_row > MAXBLOCKSPERROW)
        terminate("Assertion error");

    q = (uint8*) alloc(max_blocks_per_row * 64 * sizeof(int16) + 8);
    q = (uint8*) (((uint64) q + 7) & ~7);

    for (i = 0; i < max_blocks_per_row; i++)
        block_seg[i] = (int16*) (q + i * 64 * sizeof(int16));
    for (i = 0; i < max_blocks_per_row; i++)
        block_max_zag_set[i] = 64;

    Psample_buf = (uint8*) (((uint64) alloc(max_blocks_per_row * 64 + 8) + 7) & ~7);

    total_lines_left = image_y_size;
    mcu_lines_left   = 0;
    create_look_ups();
}

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void sImageDecoderJpeg::decode_block_dc_first(sImageDecoderJpeg* _pd, int _component_id, int _block_x, int _block_y)
{
    int    s, r;
    int16* p = _pd->coeff_buf_getp(_pd->dc_coeffs[_component_id], _block_x, _block_y);

    if ((s = _pd->huff_decode(_pd->h[_pd->comp_dc_tab[_component_id]])) != 0)
    {
        r = _pd->get_bits_2(s);
        s = (r < extend_test[s] ? r + extend_off[s] : r);
    }

    _pd->last_dc_val[_component_id] = (s += _pd->last_dc_val[_component_id]);
    p[0]                            = (int16)(s << _pd->successive_low);
}

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void sImageDecoderJpeg::decode_block_dc_refine(sImageDecoderJpeg* _pd, int _component_id, int _block_x, int _block_y)
{
    int16* p;

    if (_pd->get_bits_2(1))
    {
        p = _pd->coeff_buf_getp(_pd->dc_coeffs[_component_id], _block_x, _block_y);
        p[0] |= (1 << _pd->successive_low);
    }
}

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void sImageDecoderJpeg::decode_block_ac_first(sImageDecoderJpeg* _pd, int _component_id, int _block_x, int _block_y)
{
    int    k, s, r;
    int16* p;

    if (_pd->eob_run)
    {
        _pd->eob_run--;
        return;
    }

    p = _pd->coeff_buf_getp(_pd->ac_coeffs[_component_id], _block_x, _block_y);

    for (k = _pd->spectral_start; k <= _pd->spectral_end; k++)
    {
        s = _pd->huff_decode(_pd->h[_pd->comp_ac_tab[_component_id]]);
        r = s >> 4;
        s &= 0x0F;

        if (s)
        {
            if ((k += r) > 63)
            {
                _pd->terminate("Decode error");
                sAssert(0);
            }

            r = _pd->get_bits_2(s);

            if (s > 15)
            {
                _pd->terminate("Decode error");
                sAssert(0);
            }

            s         = (r < extend_test[s] ? r + extend_off[s] : r);
            p[zag[k]] = (int16)(s << _pd->successive_low);
        }
        else
        {
            if (r == 15)
            {
                if ((k += 15) > 63)
                {
                    _pd->terminate("Decode error");
                    sAssert(0);
                }
            }
            else
            {
                _pd->eob_run = 1 << r;
                if (r)
                    _pd->eob_run += _pd->get_bits_2(r);
                _pd->eob_run--;
                break;
            }
        }
    }
}

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void sImageDecoderJpeg::decode_block_ac_refine(sImageDecoderJpeg* _pd, int _component_id, int _block_x, int _block_y)
{
    int    s, k, r;
    int    p1 = 1 << _pd->successive_low;
    int    m1 = (-1) << _pd->successive_low;
    int16* p  = _pd->coeff_buf_getp(_pd->ac_coeffs[_component_id], _block_x, _block_y);
    int16* this_coef;

    k = _pd->spectral_start;
    if (_pd->eob_run == 0)
    {
        if (_pd->spectral_end >= 64)
        {
            _pd->terminate("Decode error");
            sAssert(0);
        }

        for (; k <= _pd->spectral_end; k++)
        {
            s = _pd->huff_decode(_pd->h[_pd->comp_ac_tab[_component_id]]);

            r = s >> 4;
            s &= 15;

            if (s)
            {
                if (s != 1)
                {
                    _pd->terminate("Decode error");
                    sAssert(0);
                }

                if (_pd->get_bits_2(1))
                    s = p1;
                else
                    s = m1;
            }
            else
            {
                if (r != 15)
                {
                    _pd->eob_run = 1 << r;
                    if (r)
                        _pd->eob_run += _pd->get_bits_2(r);
                    break;
                }
            }

            do
            {
                this_coef = p + zag[k];
                if (*this_coef != 0)
                {
                    if (_pd->get_bits_2(1))
                    {
                        if ((*this_coef & p1) == 0)
                        {
                            if (*this_coef >= 0)
                                *this_coef += (int16) p1;
                            else
                                *this_coef += (int16) m1;
                        }
                    }
                }
                else
                {
                    if (--r < 0)
                        break;
                }

                k++;
            } while (k <= _pd->spectral_end);

            if ((s) && (k < 64))
            {
                p[zag[k]] = (int16) s;
            }
        }
    }

    if (_pd->eob_run > 0)
    {
        if (_pd->spectral_end >= 64)
        {
            _pd->terminate("Decode error");
            sAssert(0);
        }

        for (; k <= _pd->spectral_end; k++)
        {
            this_coef = p + zag[k];
            if (*this_coef != 0)
            {
                if (_pd->get_bits_2(1))
                {
                    if ((*this_coef & p1) == 0)
                    {
                        if (*this_coef >= 0)
                            *this_coef += (int16) p1;
                        else
                            *this_coef += (int16) m1;
                    }
                }
            }
        }

        _pd->eob_run--;
    }
}

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void sImageDecoderJpeg::decode_scan(decode_block_func _decode_block_func)
{
    int mcu_row, mcu_col, mcu_block;
    int block_x_mcut[MAXCOMPONENTS], block_y_mcut[MAXCOMPONENTS];
    int component_num, component_id;
    int block_x_mcu_ofs, block_y_mcu_ofs;

    sMemory_Clear(block_y_mcut, sizeof(block_y_mcut));
    for (mcu_col = 0; mcu_col < mcus_per_col; mcu_col++)
    {
        sMemory_Clear(block_x_mcut, sizeof(block_x_mcut));
        for (mcu_row = 0; mcu_row < mcus_per_row; mcu_row++)
        {
            block_x_mcu_ofs = 0;
            block_y_mcu_ofs = 0;

            if ((restart_interval) && (restarts_left == 0))
                process_restart();

            for (mcu_block = 0; mcu_block < blocks_per_mcu; mcu_block++)
            {
                component_id = mcu_org[mcu_block];
                _decode_block_func(this, component_id, block_x_mcut[component_id] + block_x_mcu_ofs, block_y_mcut[component_id] + block_y_mcu_ofs);

                if (comps_in_scan == 1)
                    block_x_mcut[component_id]++;
                else
                {
                    if (++block_x_mcu_ofs == comp_h_samp[component_id])
                    {
                        block_x_mcu_ofs = 0;
                        if (++block_y_mcu_ofs == comp_v_samp[component_id])
                        {
                            block_y_mcu_ofs = 0;
                            block_x_mcut[component_id] += comp_h_samp[component_id];
                        }
                    }
                }
            }

            restarts_left--;
        }

        if (comps_in_scan == 1)
            block_y_mcut[comp_list[0]]++;
        else
        {
            for (component_num = 0; component_num < comps_in_scan; component_num++)
            {
                component_id = comp_list[component_num];
                block_y_mcut[component_id] += comp_v_samp[component_id];
            }
        }
    }
}

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void sImageDecoderJpeg::init_progressive()
{
    int               i;
    int               dc_only_scan, refinement_scan;
    decode_block_func decode_block_func;

    if (comps_in_frame == 4)
        terminate("Unsupported color space");

    for (i = 0; i < comps_in_frame; i++)
    {
        dc_coeffs[i] = coeff_buf_open(max_mcus_per_row * comp_h_samp[i], max_mcus_per_col * comp_v_samp[i], 1, 1);
        ac_coeffs[i] = coeff_buf_open(max_mcus_per_row * comp_h_samp[i], max_mcus_per_col * comp_v_samp[i], 8, 8);
    }

    for (;;)
    {
        if (!init_scan())
            break;

        dc_only_scan    = (spectral_start == 0);
        refinement_scan = (successive_high != 0);

        if ((spectral_start > spectral_end) || (spectral_end > 63))
            terminate("Bad sos spectral");

        if (dc_only_scan)
        {
            if (spectral_end)
                terminate("Bad sos spectral");
        }
        else if (comps_in_scan != 1)
            terminate("Bad sos spectral");

        if ((refinement_scan) && (successive_low != successive_high - 1))
            terminate("Bad sos successive");

        if (dc_only_scan)
        {
            if (refinement_scan)
                decode_block_func = decode_block_dc_refine;
            else
                decode_block_func = decode_block_dc_first;
        }
        else
        {
            if (refinement_scan)
                decode_block_func = decode_block_ac_refine;
            else
                decode_block_func = decode_block_ac_first;
        }

        decode_scan(decode_block_func);

        bits_left = 16;

        get_bits_1(16);
        get_bits_1(16);
    }

    comps_in_scan = comps_in_frame;
    for (i = 0; i < comps_in_frame; i++)
        comp_list[i] = i;
    calc_mcu_block_order();
}

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void sImageDecoderJpeg::init_sequential()
{
    if (!init_scan())
        terminate("Unexpected marker");
}

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void sImageDecoderJpeg::decode_start()
{
    init_frame();
    if (progressive_flag)
        init_progressive();
    else
        init_sequential();
}

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void sImageDecoderJpeg::idct(int16* _data, uint8* _pdst_ptr)
{
    int    tmp0, tmp1, tmp2, tmp3;
    int    tmp10, tmp11, tmp12, tmp13;
    int    z1, z2, z3, z4, z5;
    int16* dataptr;
    int    rowctr;
    int16  i, dcval;

    dataptr = _data;
    for (rowctr = 8 - 1; rowctr >= 0; rowctr--)
    {
        if ((dataptr[1] | dataptr[2] | dataptr[3] | dataptr[4] | dataptr[5] | dataptr[6] | dataptr[7]) == 0)
        {
            dcval = (int16)(dataptr[0] << PASS1_BITS);

            dataptr[0] = dcval;
            dataptr[1] = dcval;
            dataptr[2] = dcval;
            dataptr[3] = dcval;
            dataptr[4] = dcval;
            dataptr[5] = dcval;
            dataptr[6] = dcval;
            dataptr[7] = dcval;

            dataptr += 8;
            continue;
        }

        z2 = (int) dataptr[2];
        z3 = (int) dataptr[6];

        z1   = (z2 + z3) * FIX_0_541196100;
        tmp2 = z1 + (z3 * -FIX_1_847759065);
        tmp3 = z1 + (z2 * FIX_0_765366865);

        tmp0 = ((int) dataptr[0] + (int) dataptr[4]) << CONST_BITS;
        tmp1 = ((int) dataptr[0] - (int) dataptr[4]) << CONST_BITS;

        tmp10 = tmp0 + tmp3;
        tmp13 = tmp0 - tmp3;
        tmp11 = tmp1 + tmp2;
        tmp12 = tmp1 - tmp2;

        tmp0 = (int) dataptr[7];
        tmp1 = (int) dataptr[5];
        tmp2 = (int) dataptr[3];
        tmp3 = (int) dataptr[1];

        z1 = tmp0 + tmp3;
        z2 = tmp1 + tmp2;
        z3 = tmp0 + tmp2;
        z4 = tmp1 + tmp3;
        z5 = (z3 + z4) * FIX_1_175875602;

        tmp0 = (tmp0 * FIX_0_298631336);
        tmp1 = (tmp1 * FIX_2_053119869);
        tmp2 = (tmp2 * FIX_3_072711026);
        tmp3 = (tmp3 * FIX_1_501321110);
        z1   = (z1 * -FIX_0_899976223);
        z2   = (z2 * -FIX_2_562915447);
        z3   = (z3 * -FIX_1_961570560);
        z4   = (z4 * -FIX_0_390180644);

        z3 += z5;
        z4 += z5;

        tmp0 += z1 + z3;
        tmp1 += z2 + z4;
        tmp2 += z2 + z3;
        tmp3 += z1 + z4;

        dataptr[0] = (int16) jpeg_descale(tmp10 + tmp3, CONST_BITS - PASS1_BITS);
        dataptr[7] = (int16) jpeg_descale(tmp10 - tmp3, CONST_BITS - PASS1_BITS);
        dataptr[1] = (int16) jpeg_descale(tmp11 + tmp2, CONST_BITS - PASS1_BITS);
        dataptr[6] = (int16) jpeg_descale(tmp11 - tmp2, CONST_BITS - PASS1_BITS);
        dataptr[2] = (int16) jpeg_descale(tmp12 + tmp1, CONST_BITS - PASS1_BITS);
        dataptr[5] = (int16) jpeg_descale(tmp12 - tmp1, CONST_BITS - PASS1_BITS);
        dataptr[3] = (int16) jpeg_descale(tmp13 + tmp0, CONST_BITS - PASS1_BITS);
        dataptr[4] = (int16) jpeg_descale(tmp13 - tmp0, CONST_BITS - PASS1_BITS);

        dataptr += 8;
    }

    dataptr = _data;
    for (rowctr = 8 - 1; rowctr >= 0; rowctr--)
    {
        if ((dataptr[8 * 1] | dataptr[8 * 2] | dataptr[8 * 3] | dataptr[8 * 4] | dataptr[8 * 5] | dataptr[8 * 6] | dataptr[8 * 7]) == 0)
        {
            dcval = (int16) jpeg_descale((int) dataptr[0], PASS1_BITS + 3);
            if ((dcval += 128) < 0)
                dcval = 0;
            else if (dcval > 255)
                dcval = 255;

            _pdst_ptr[8 * 0] = (uint8) dcval;
            _pdst_ptr[8 * 1] = (uint8) dcval;
            _pdst_ptr[8 * 2] = (uint8) dcval;
            _pdst_ptr[8 * 3] = (uint8) dcval;
            _pdst_ptr[8 * 4] = (uint8) dcval;
            _pdst_ptr[8 * 5] = (uint8) dcval;
            _pdst_ptr[8 * 6] = (uint8) dcval;
            _pdst_ptr[8 * 7] = (uint8) dcval;

            dataptr++;
            _pdst_ptr++;
            continue;
        }

        z2 = (int) dataptr[8 * 2];
        z3 = (int) dataptr[8 * 6];

        z1   = (z2 + z3) * FIX_0_541196100;
        tmp2 = z1 + (z3 * -FIX_1_847759065);
        tmp3 = z1 + (z2 * FIX_0_765366865);

        tmp0 = ((int) dataptr[8 * 0] + (int) dataptr[8 * 4]) << CONST_BITS;
        tmp1 = ((int) dataptr[8 * 0] - (int) dataptr[8 * 4]) << CONST_BITS;

        tmp10 = tmp0 + tmp3;
        tmp13 = tmp0 - tmp3;
        tmp11 = tmp1 + tmp2;
        tmp12 = tmp1 - tmp2;

        tmp0 = (int) dataptr[8 * 7];
        tmp1 = (int) dataptr[8 * 5];
        tmp2 = (int) dataptr[8 * 3];
        tmp3 = (int) dataptr[8 * 1];

        z1 = tmp0 + tmp3;
        z2 = tmp1 + tmp2;
        z3 = tmp0 + tmp2;
        z4 = tmp1 + tmp3;
        z5 = (z3 + z4) * FIX_1_175875602;

        tmp0 = (tmp0 * FIX_0_298631336);
        tmp1 = (tmp1 * FIX_2_053119869);
        tmp2 = (tmp2 * FIX_3_072711026);
        tmp3 = (tmp3 * FIX_1_501321110);
        z1   = (z1 * -FIX_0_899976223);
        z2   = (z2 * -FIX_2_562915447);
        z3   = (z3 * -FIX_1_961570560);
        z4   = (z4 * -FIX_0_390180644);

        z3 += z5;
        z4 += z5;

        tmp0 += z1 + z3;
        tmp1 += z2 + z4;
        tmp2 += z2 + z3;
        tmp3 += z1 + z4;

        i                = (int16) jpeg_descale(tmp10 + tmp3, CONST_BITS + PASS1_BITS + 3) + 128;
        i                = jpeg_clampd(i);
        _pdst_ptr[8 * 0] = (uint8) i;

        i                = (int16) jpeg_descale(tmp10 - tmp3, CONST_BITS + PASS1_BITS + 3) + 128;
        i                = jpeg_clampd(i);
        _pdst_ptr[8 * 7] = (uint8) i;

        i                = (int16) jpeg_descale(tmp11 + tmp2, CONST_BITS + PASS1_BITS + 3) + 128;
        i                = jpeg_clampd(i);
        _pdst_ptr[8 * 1] = (uint8) i;

        i                = (int16) jpeg_descale(tmp11 - tmp2, CONST_BITS + PASS1_BITS + 3) + 128;
        i                = jpeg_clampd(i);
        _pdst_ptr[8 * 6] = (uint8) i;

        i                = (int16) jpeg_descale(tmp12 + tmp1, CONST_BITS + PASS1_BITS + 3) + 128;
        i                = jpeg_clampd(i);
        _pdst_ptr[8 * 2] = (uint8) i;

        i                = (int16) jpeg_descale(tmp12 - tmp1, CONST_BITS + PASS1_BITS + 3) + 128;
        i                = jpeg_clampd(i);
        _pdst_ptr[8 * 5] = (uint8) i;

        i                = (int16) jpeg_descale(tmp13 + tmp0, CONST_BITS + PASS1_BITS + 3) + 128;
        i                = jpeg_clampd(i);
        _pdst_ptr[8 * 3] = (uint8) i;

        i                = (int16) jpeg_descale(tmp13 - tmp0, CONST_BITS + PASS1_BITS + 3) + 128;
        i                = jpeg_clampd(i);
        _pdst_ptr[8 * 4] = (uint8) i;

        dataptr++;
        _pdst_ptr++;
    }
}

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void sImageDecoderJpeg::H2V2Convert()
{
    int    row = max_mcu_y_size - mcu_lines_left;
    uint8* d0  = scan_line_0;
    uint8* d1  = scan_line_1;
    uint8* y;
    uint8* c;
    int    cb, cr, rc, gc, bc, yy;

    if (row < 8)
        y = Psample_buf + row * 8;
    else
        y = Psample_buf + 64 * 2 + (row & 7) * 8;

    c = Psample_buf + 64 * 4 + (row >> 1) * 8;

    for (int i = max_mcus_per_row; i > 0; i--)
    {
        for (int l = 0; l < 2; l++)
        {
            for (int j = 0; j < 8; j += 2)
            {
                cb = c[0];
                cr = c[64];
                rc = crr[cr];
                gc = ((crg[cr] + cbg[cb]) >> 16);
                bc = cbb[cb];
                yy = y[j];

                d0[2] = jpeg_clamp(yy + rc);
                d0[1] = jpeg_clamp(yy + gc);
                d0[0] = jpeg_clamp(yy + bc);

                yy    = y[j + 1];
                d0[5] = jpeg_clamp(yy + rc);
                d0[4] = jpeg_clamp(yy + gc);
                d0[3] = jpeg_clamp(yy + bc);

                yy    = y[j + 8];
                d1[2] = jpeg_clamp(yy + rc);
                d1[1] = jpeg_clamp(yy + gc);
                d1[0] = jpeg_clamp(yy + bc);

                yy    = y[j + 8 + 1];
                d1[5] = jpeg_clamp(yy + rc);
                d1[4] = jpeg_clamp(yy + gc);
                d1[3] = jpeg_clamp(yy + bc);

                d0 += 6;
                d1 += 6;

                c++;
            }

            y += 64;
        }

        y += 64 * 6 - 64 * 2;
        c += 64 * 6 - 8;
    }
}

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void sImageDecoderJpeg::decode_init(const uint8* _stream, uint _streamlen)
{
    init(_stream, _streamlen);
    locate_sof_marker();
}

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void sImageDecoderJpeg::decode_process(uint8* _pdest)
{
    int    lines_decoded;
    void*  Pscan_line_ofs;
    uint   scan_line_len;
    uint8 *Pdest1, *Psrc;
    int    x;

    if (comps_in_frame == 1)
    {
        // Gray scale (1BPP)
        _pdest += (image_y_size - 1) * image_x_size * 3;
        lines_decoded = 0;
        for (;;)
        {
            if (decode(&Pscan_line_ofs, &scan_line_len))
                break;
            Pdest1 = _pdest;
            Psrc   = (uint8*) Pscan_line_ofs;
            for (x = 0; x < image_x_size; x++)
            {
                *Pdest1++ = *Psrc;
                *Pdest1++ = *Psrc;
                *Pdest1++ = *Psrc++;
            }

            _pdest -= image_x_size * 3;
        }
    }
    else
    {
        // 3 BPP
        sAssert(comps_in_frame == 3);
        _pdest += (image_y_size - 1) * image_x_size * 3;
        lines_decoded = 0;
        for (;;)
        {
            if (decode(&Pscan_line_ofs, &scan_line_len))
                break;
            sMemory_Copy(_pdest, Pscan_line_ofs, image_x_size * 3);
            _pdest -= image_x_size * 3;
        }
    }
}
