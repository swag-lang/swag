#include "pch.h"
#include "sImageEncoderJpeg.hpp"

typedef struct APP0infotype_
{
    uint16 marker;
    uint16 length;
    uint8  JFIFsignature[5];
    uint8  versionhi;
    uint8  versionlo;
    uint8  xyunits;
    uint16 xdensity;
    uint16 ydensity;
    uint8  thumbnwidth;
    uint8  thumbnheight;
} APP0infotype;

typedef struct
{
    uint16 marker;
    uint16 length;
    uint8  precision;
    uint16 height;
    uint16 width;
    uint8  nrofcomponents;
    uint8  IdY;
    uint8  HVY;
    uint8  QTY;
    uint8  IdCb;
    uint8  HVCb;
    uint8  QTCb;
    uint8  IdCr;
    uint8  HVCr;
    uint8  QTCr;
} SOF0infotype;

typedef struct
{
    uint16 marker;
    uint16 length;
    uint8  nrofcomponents;
    uint8  IdY;
    uint8  HTY;
    uint8  IdCb;
    uint8  HTCb;
    uint8  IdCr;
    uint8  HTCr;
    uint8  Ss, Se, Bf;
} SOSinfotype;

const APP0infotype_ APP0info                       = {0xFFE0, 16, 'J', 'F', 'I', 'F', 0, 1, 1, 0, 1, 1, 0, 0};
const SOF0infotype  SOF0info                       = {0xFFC0, 17, 8, 0, 0, 3, 1, 0x11, 0, 2, 0x11, 1, 3, 0x11, 1};
const SOSinfotype   SOSinfo                        = {0xFFDA, 12, 3, 1, 0, 2, 0x11, 3, 0x11, 0, 0x3F, 0};
const uint8           std_dc_luminance_nrcodes[17]   = {0, 0, 1, 5, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0};
const uint8           std_dc_luminance_values[12]    = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
const uint8           std_dc_chrominance_nrcodes[17] = {0, 0, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0};
const uint8           std_dc_chrominance_values[12]  = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
const uint8           std_ac_luminance_nrcodes[17]   = {0, 0, 2, 1, 3, 3, 2, 4, 3, 5, 5, 4, 4, 0, 0, 1, 0x7d};
const uint8           std_ac_chrominance_nrcodes[17] = {0, 0, 2, 1, 2, 4, 4, 3, 4, 7, 5, 4, 4, 0, 1, 2, 0x77};
const uint16          mask[16]                       = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768};

// clang-format off
const uint8 zigzag[64] = { 
    0,  1,  5,  6,  14, 15, 27, 28, 2,  4,  7,  13, 16, 26, 29, 42, 3,  8,  12, 17, 25, 30,
    41, 43, 9,  11, 18, 24, 31, 40, 44, 53, 10, 19, 23, 32, 39, 45, 52, 54, 20, 22, 33, 38,
    46, 51, 55, 60, 21, 34, 37, 47, 50, 56, 59, 61, 35, 36, 48, 49, 57, 58, 62, 63 };

const uint8 std_luminance_qt[64] = { 
    16, 11, 10, 16, 24, 40,  51,  61, 12, 12, 14, 19, 26, 58,  60,  55, 14, 13, 16, 24, 40,  57,  69,  56,  14, 17, 22, 29, 51,  87,  80,  62,
    18, 22, 37, 56, 68, 109, 103, 77, 24, 35, 55, 64, 81, 104, 113, 92, 49, 64, 78, 87, 103, 121, 120, 101, 72, 92, 95, 98, 112, 100, 103, 99 };

const uint8 std_chrominance_qt[64] = { 
    17, 18, 24, 47, 99, 99, 99, 99, 18, 21, 26, 66, 99, 99, 99, 99, 24, 26, 56, 99, 99, 99, 99, 99, 47, 66, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99 };

const uint8 std_ac_luminance_values[162] = {
    0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12, 0x21, 0x31, 0x41, 0x06, 0x13, 0x51, 0x61, 0x07, 0x22, 0x71, 0x14, 0x32, 0x81, 0x91, 0xa1, 0x08,
    0x23, 0x42, 0xb1, 0xc1, 0x15, 0x52, 0xd1, 0xf0, 0x24, 0x33, 0x62, 0x72, 0x82, 0x09, 0x0a, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x25, 0x26, 0x27, 0x28,
    0x29, 0x2a, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59,
    0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89,
    0x8a, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6,
    0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xe1, 0xe2,
    0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa };

const uint8 std_ac_chrominance_values[162] = {
    0x00, 0x01, 0x02, 0x03, 0x11, 0x04, 0x05, 0x21, 0x31, 0x06, 0x12, 0x41, 0x51, 0x07, 0x61, 0x71, 0x13, 0x22, 0x32, 0x81, 0x08, 0x14, 0x42, 0x91,
    0xa1, 0xb1, 0xc1, 0x09, 0x23, 0x33, 0x52, 0xf0, 0x15, 0x62, 0x72, 0xd1, 0x0a, 0x16, 0x24, 0x34, 0xe1, 0x25, 0xf1, 0x17, 0x18, 0x19, 0x1a, 0x26,
    0x27, 0x28, 0x29, 0x2a, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
    0x59, 0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
    0x88, 0x89, 0x8a, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4,
    0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda,
    0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa };
// clang-format on

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
bool sImageEncoderJpeg::Encode()
{
    bitstring fillbits;

    load_bitmap();

    init_all();

    writeword(0xFFD8); // SOI

    write_APP0info();
    write_DQTinfo();
    write_SOF0info();
    write_DHTinfo();
    write_SOSinfo();

    bytenew = 0;
    bytepos = 7;
    main_encoder();

    // Do the bit alignment of the EOI marker
    if (bytepos >= 0)
    {
        fillbits.length = bytepos + 1;
        fillbits.value  = (1 << (bytepos + 1)) - 1;
        writebits(fillbits);
    }

    writeword(0xFFD9); // EOI

    delete[] RGB_buffer;
    delete category_alloc;
    delete[] bitcode_alloc;
    return true;
}

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void sImageEncoderJpeg::SetQuality(uint8 _quality)
{
    //_quality      = sClamp(_quality, (uint8) 1, (uint8) 100);
    //m_ScaleFactor = 100 - _quality;
}

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
uint8 sImageEncoderJpeg::Y(uint8 _r, uint8 _g, uint8 _b)
{
    return ((uint8)((YRtab[(_r)] + YGtab[(_g)] + YBtab[(_b)]) >> 16) - 128);
}

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
uint8 sImageEncoderJpeg::Cb(uint8 _r, uint8 _g, uint8 _b)
{
    return ((uint8)((CbRtab[(_r)] + CbGtab[(_g)] + CbBtab[(_b)]) >> 16));
}

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
uint8 sImageEncoderJpeg::Cr(uint8 _r, uint8 _g, uint8 _b)
{
    return ((uint8)((CrRtab[(_r)] + CrGtab[(_g)] + CrBtab[(_b)]) >> 16));
}

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void sImageEncoderJpeg::writebyte(uint8 _b)
{
    //fp_jpeg_stream->WriteChar(_b);
}

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void sImageEncoderJpeg::writeword(uint16 _w)
{
    //fp_jpeg_stream->WriteChar((uint8)((_w) >> 8));
    //fp_jpeg_stream->WriteChar((uint8)((_w) &0xFF));
}

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void sImageEncoderJpeg::write_APP0info()
{
    writeword(APP0info.marker);
    writeword(APP0info.length);
    writebyte('J');
    writebyte('F');
    writebyte('I');
    writebyte('F');
    writebyte(0);
    writebyte(APP0info.versionhi);
    writebyte(APP0info.versionlo);
    writebyte(APP0info.xyunits);
    writeword(APP0info.xdensity);
    writeword(APP0info.ydensity);
    writebyte(APP0info.thumbnwidth);
    writebyte(APP0info.thumbnheight);
}

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void sImageEncoderJpeg::write_SOF0info()
{
    writeword(SOF0info.marker);
    writeword(SOF0info.length);
    writebyte(SOF0info.precision);
    //writeword((uint16) fp_jpeg_image->GetHeight());
    //writeword((uint16) fp_jpeg_image->GetWidth());
    writebyte(SOF0info.nrofcomponents);
    writebyte(SOF0info.IdY);
    writebyte(SOF0info.HVY);
    writebyte(SOF0info.QTY);
    writebyte(SOF0info.IdCb);
    writebyte(SOF0info.HVCb);
    writebyte(SOF0info.QTCb);
    writebyte(SOF0info.IdCr);
    writebyte(SOF0info.HVCr);
    writebyte(SOF0info.QTCr);
}

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void sImageEncoderJpeg::write_DQTinfo()
{
    uint8 i;

    writeword(DQTinfo.marker);
    writeword(DQTinfo.length);
    writebyte(DQTinfo.QTYinfo);
    for (i = 0; i < 64; i++)
        writebyte(DQTinfo.Ytable[i]);
    writebyte(DQTinfo.QTCbinfo);
    for (i = 0; i < 64; i++)
        writebyte(DQTinfo.Cbtable[i]);
}

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Set quantization table and zigzag reorder it
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void sImageEncoderJpeg::set_quant_table(const uint8* _basic_table, uint8 _scale_factor, uint8* _newtable)
{
    uint8  i;
    long temp;

    for (i = 0; i < 64; i++)
    {
        temp = ((long) _basic_table[i] * _scale_factor + 50L) / 100L;

        // Limit the values to the valid range
        if (temp <= 0L)
            temp = 1L;
        if (temp > 255L)
            temp = 255L; // Limit to baseline range if requested
        _newtable[zigzag[i]] = (uint8) temp;
    }
}

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    scalefactor controls the visual quality of the image ;
    the smaller is, the better image we'll get, and the smaller compression we'll achieve
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void sImageEncoderJpeg::set_DQTinfo()
{
    DQTinfo.marker   = 0xFFDB;
    DQTinfo.length   = 132;
    DQTinfo.QTYinfo  = 0;
    DQTinfo.QTCbinfo = 1;
    set_quant_table(std_luminance_qt, m_ScaleFactor, DQTinfo.Ytable);
    set_quant_table(std_chrominance_qt, m_ScaleFactor, DQTinfo.Cbtable);
}

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void sImageEncoderJpeg::write_DHTinfo()
{
    uint8 i;

    writeword(DHTinfo.marker);
    writeword(DHTinfo.length);
    writebyte(DHTinfo.HTYDCinfo);
    for (i = 0; i < 16; i++)
        writebyte(DHTinfo.YDC_nrcodes[i]);
    for (i = 0; i <= 11; i++)
        writebyte(DHTinfo.YDC_values[i]);
    writebyte(DHTinfo.HTYACinfo);
    for (i = 0; i < 16; i++)
        writebyte(DHTinfo.YAC_nrcodes[i]);
    for (i = 0; i <= 161; i++)
        writebyte(DHTinfo.YAC_values[i]);
    writebyte(DHTinfo.HTCbDCinfo);
    for (i = 0; i < 16; i++)
        writebyte(DHTinfo.CbDC_nrcodes[i]);
    for (i = 0; i <= 11; i++)
        writebyte(DHTinfo.CbDC_values[i]);
    writebyte(DHTinfo.HTCbACinfo);
    for (i = 0; i < 16; i++)
        writebyte(DHTinfo.CbAC_nrcodes[i]);
    for (i = 0; i <= 161; i++)
        writebyte(DHTinfo.CbAC_values[i]);
}

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void sImageEncoderJpeg::set_DHTinfo()
{
    uint8 i;

    DHTinfo.marker    = 0xFFC4;
    DHTinfo.length    = 0x01A2;
    DHTinfo.HTYDCinfo = 0;
    for (i = 0; i < 16; i++)
        DHTinfo.YDC_nrcodes[i] = std_dc_luminance_nrcodes[i + 1];
    for (i = 0; i <= 11; i++)
        DHTinfo.YDC_values[i] = std_dc_luminance_values[i];
    DHTinfo.HTYACinfo = 0x10;
    for (i = 0; i < 16; i++)
        DHTinfo.YAC_nrcodes[i] = std_ac_luminance_nrcodes[i + 1];
    for (i = 0; i <= 161; i++)
        DHTinfo.YAC_values[i] = std_ac_luminance_values[i];
    DHTinfo.HTCbDCinfo = 1;
    for (i = 0; i < 16; i++)
        DHTinfo.CbDC_nrcodes[i] = std_dc_chrominance_nrcodes[i + 1];
    for (i = 0; i <= 11; i++)
        DHTinfo.CbDC_values[i] = std_dc_chrominance_values[i];
    DHTinfo.HTCbACinfo = 0x11;
    for (i = 0; i < 16; i++)
        DHTinfo.CbAC_nrcodes[i] = std_ac_chrominance_nrcodes[i + 1];
    for (i = 0; i <= 161; i++)
        DHTinfo.CbAC_values[i] = std_ac_chrominance_values[i];
}

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void sImageEncoderJpeg::write_SOSinfo()
{
    writeword(SOSinfo.marker);
    writeword(SOSinfo.length);
    writebyte(SOSinfo.nrofcomponents);
    writebyte(SOSinfo.IdY);
    writebyte(SOSinfo.HTY);
    writebyte(SOSinfo.IdCb);
    writebyte(SOSinfo.HTCb);
    writebyte(SOSinfo.IdCr);
    writebyte(SOSinfo.HTCr);
    writebyte(SOSinfo.Ss);
    writebyte(SOSinfo.Se);
    writebyte(SOSinfo.Bf);
}

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void sImageEncoderJpeg::writebits(bitstring _bs)
{
    uint16 value;
    int8  posval; // Bit position in the bitstring we read, should be<=15 and >=0
    value  = _bs.value;
    posval = _bs.length - 1;
    while (posval >= 0)
    {
        if (value & mask[posval])
            bytenew |= mask[bytepos];
        posval--;
        bytepos--;
        if (bytepos < 0)
        {
            if (bytenew == 0xFF)
            {
                writebyte(0xFF);
                writebyte(0);
            }
            else
            {
                writebyte(bytenew);
            }

            bytepos = 7;
            bytenew = 0;
        }
    }
}

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void sImageEncoderJpeg::compute_Huffman_table(const uint8* _nrcodes, const uint8* _std_table, bitstring* _hT)
{
    uint8  k, j;
    uint8  pos_in_table;
    uint16 codevalue;

    codevalue    = 0;
    pos_in_table = 0;
    for (k = 1; k <= 16; k++)
    {
        for (j = 1; j <= _nrcodes[k]; j++)
        {
            _hT[_std_table[pos_in_table]].value  = codevalue;
            _hT[_std_table[pos_in_table]].length = k;
            pos_in_table++;
            codevalue++;
        }

        codevalue *= 2;
    }
}

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void sImageEncoderJpeg::init_Huffman_tables()
{
    compute_Huffman_table(std_dc_luminance_nrcodes, std_dc_luminance_values, YDC_HT);
    compute_Huffman_table(std_dc_chrominance_nrcodes, std_dc_chrominance_values, CbDC_HT);
    compute_Huffman_table(std_ac_luminance_nrcodes, std_ac_luminance_values, YAC_HT);
    compute_Huffman_table(std_ac_chrominance_nrcodes, std_ac_chrominance_values, CbAC_HT);
}

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void sImageEncoderJpeg::set_numbers_category_and_bitcode()
{
    int nr;
    int nrlower, nrupper;
    uint8  cat;

    category_alloc = new uint8[65535];
    category       = category_alloc + 32767; // Allow negative subscripts
    bitcode_alloc  = new bitstring[65535];
    bitcode        = bitcode_alloc + 32767;
    nrlower        = 1;
    nrupper        = 2;
    for (cat = 1; cat <= 15; cat++)
    {
        // Positive numbers
        for (nr = nrlower; nr < nrupper; nr++)
        {
            category[nr]       = cat;
            bitcode[nr].length = cat;
            bitcode[nr].value  = (uint16) nr;
        }

        // Negative numbers
        for (nr = -(nrupper - 1); nr <= -nrlower; nr++)
        {
            category[nr]       = cat;
            bitcode[nr].length = cat;
            bitcode[nr].value  = (uint16)(nrupper - 1 + nr);
        }

        nrlower <<= 1;
        nrupper <<= 1;
    }
}

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void sImageEncoderJpeg::precalculate_YCbCr_tables()
{
    uint16 R, G, B;

    for (R = 0; R <= 255; R++)
    {
        YRtab[R]  = (int)(65536 * 0.299 + 0.5) * R;
        CbRtab[R] = (int)(65536 * -0.16874 + 0.5) * R;
        CrRtab[R] = (int)(32768) * R;
    }

    for (G = 0; G <= 255; G++)
    {
        YGtab[G]  = (int)(65536 * 0.587 + 0.5) * G;
        CbGtab[G] = (int)(65536 * -0.33126 + 0.5) * G;
        CrGtab[G] = (int)(65536 * -0.41869 + 0.5) * G;
    }

    for (B = 0; B <= 255; B++)
    {
        YBtab[B]  = (int)(65536 * 0.114 + 0.5) * B;
        CbBtab[B] = (int)(32768) * B;
        CrBtab[B] = (int)(65536 * -0.08131 + 0.5) * B;
    }
}

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Using a bit modified form of the FDCT routine from IJG's C source: Forward DCT routine idea taken from Independent
    JPEG Group's C source for JPEG encoders/decoders ;
    For float AA&N IDCT method, divisors are equal to quantization coefficients scaled by
    scalefactor[row]*scalefactor[col], where scalefactor[0] = 1 scalefactor[k] = cos(k*PI/16) * sqrt(2) for k=1..7 We
    apply a further scale factor of 8. What's actually stored is 1/divisor so that the inner loop can use a
    multiplication rather than a division.
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void sImageEncoderJpeg::prepare_quant_tables()
{
    double aan_sf[8] = {1.0, 1.387039845, 1.306562965, 1.175875602, 1.0, 0.785694958, 0.541196100, 0.275899379};
    uint8  row, col;
    uint8  i = 0;

    for (row = 0; row < 8; row++)
    {
        for (col = 0; col < 8; col++)
        {
            fdtbl_Y[i]  = (float)(1.0 / ((double) DQTinfo.Ytable[zigzag[i]] * aan_sf[row] * aan_sf[col] * 8.0));
            fdtbl_Cb[i] = (float)(1.0 / ((double) DQTinfo.Cbtable[zigzag[i]] * aan_sf[row] * aan_sf[col] * 8.0));
            i++;
        }
    }
}

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void sImageEncoderJpeg::fdct_and_quantization(int8* _data, float* _fdtbl, int16* _outdata)
{
    float  tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;
    float  tmp10, tmp11, tmp12, tmp13;
    float  z1, z2, z3, z4, z5, z11, z13;
    float* dataptr;
    float  datafloat[64];
    float  temp;
    int8   ctr;
    uint8   i;

    for (i = 0; i < 64; i++)
        datafloat[i] = _data[i];

    // Pass 1: process rows.
    dataptr = datafloat;
    for (ctr = 7; ctr >= 0; ctr--)
    {
        tmp0 = dataptr[0] + dataptr[7];
        tmp7 = dataptr[0] - dataptr[7];
        tmp1 = dataptr[1] + dataptr[6];
        tmp6 = dataptr[1] - dataptr[6];
        tmp2 = dataptr[2] + dataptr[5];
        tmp5 = dataptr[2] - dataptr[5];
        tmp3 = dataptr[3] + dataptr[4];
        tmp4 = dataptr[3] - dataptr[4];

        // Even part
        tmp10 = tmp0 + tmp3; // Phase 2
        tmp13 = tmp0 - tmp3;
        tmp11 = tmp1 + tmp2;
        tmp12 = tmp1 - tmp2;

        dataptr[0] = tmp10 + tmp11; // Phase 3
        dataptr[4] = tmp10 - tmp11;

        z1         = (tmp12 + tmp13) * ((float) 0.707106781); // C4
        dataptr[2] = tmp13 + z1; // Phase 5
        dataptr[6] = tmp13 - z1;

        // Odd part
        tmp10 = tmp4 + tmp5; // Phase 2
        tmp11 = tmp5 + tmp6;
        tmp12 = tmp6 + tmp7;

        // The rotator is modified from fig 4-8 to avoid extra negations.
        z5 = (tmp10 - tmp12) * ((float) 0.382683433); // C6
        z2 = ((float) 0.541196100) * tmp10 + z5; // C2-c6
        z4 = ((float) 1.306562965) * tmp12 + z5; // C2+c6
        z3 = tmp11 * ((float) 0.707106781); // C4

        z11 = tmp7 + z3; // Phase 5
        z13 = tmp7 - z3;

        dataptr[5] = z13 + z2; // Phase 6
        dataptr[3] = z13 - z2;
        dataptr[1] = z11 + z4;
        dataptr[7] = z11 - z4;

        dataptr += 8; // Advance pointer to next row
    }

    // Pass 2: process columns.
    dataptr = datafloat;
    for (ctr = 7; ctr >= 0; ctr--)
    {
        tmp0 = dataptr[0] + dataptr[56];
        tmp7 = dataptr[0] - dataptr[56];
        tmp1 = dataptr[8] + dataptr[48];
        tmp6 = dataptr[8] - dataptr[48];
        tmp2 = dataptr[16] + dataptr[40];
        tmp5 = dataptr[16] - dataptr[40];
        tmp3 = dataptr[24] + dataptr[32];
        tmp4 = dataptr[24] - dataptr[32];

        // Even part
        tmp10 = tmp0 + tmp3; // Phase 2
        tmp13 = tmp0 - tmp3;
        tmp11 = tmp1 + tmp2;
        tmp12 = tmp1 - tmp2;

        dataptr[0]  = tmp10 + tmp11; // Phase 3
        dataptr[32] = tmp10 - tmp11;

        z1          = (tmp12 + tmp13) * ((float) 0.707106781); // C4
        dataptr[16] = tmp13 + z1; // Phase 5
        dataptr[48] = tmp13 - z1;

        // Odd part
        tmp10 = tmp4 + tmp5; // Phase 2
        tmp11 = tmp5 + tmp6;
        tmp12 = tmp6 + tmp7;

        // The rotator is modified from fig 4-8 to avoid extra negations.
        z5 = (tmp10 - tmp12) * ((float) 0.382683433); // C6
        z2 = ((float) 0.541196100) * tmp10 + z5; // C2-c6
        z4 = ((float) 1.306562965) * tmp12 + z5; // C2+c6
        z3 = tmp11 * ((float) 0.707106781); // C4

        z11 = tmp7 + z3; // Phase 5
        z13 = tmp7 - z3;

        dataptr[40] = z13 + z2; // Phase 6
        dataptr[24] = z13 - z2;
        dataptr[8]  = z11 + z4;
        dataptr[56] = z11 - z4;

        dataptr++; // Advance pointer to next column
    }

    // Quantize/descale the coefficients, and store into output array
    for (i = 0; i < 64; i++)
    {
        // Apply the quantization and scaling factor
        temp = datafloat[i] * _fdtbl[i];

        // Round to nearest integer. Since C does not specify the direction of rounding
        // for negative quotients, we have to force the dividend positive for portability.
        // The maximum coefficient size is +-16K (for 12-bit data), so this code should
        // work for either 16-bit or 32-bit ints.
        _outdata[i] = (int16)((int16)(temp + 16384.5) - 16384);
    }
}

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void sImageEncoderJpeg::process_DU(int8* _componentDU, float* _fdtbl, int16* _dC, bitstring* _hTDC, bitstring* _hTAC)
{
    bitstring EOB       = _hTAC[0x00];
    bitstring M16zeroes = _hTAC[0xF0];
    uint8       i;
    uint8       startpos;
    uint8       end0pos;
    uint8       nrzeroes;
    uint8       nrmarker;
    int16      Diff;

    fdct_and_quantization(_componentDU, _fdtbl, DU_DCT);

    // Zigzag reorder
    for (i = 0; i <= 63; i++)
        DU[zigzag[i]] = DU_DCT[i];
    Diff = DU[0] - *_dC;
    *_dC = DU[0];

    // Encode DC
    if (Diff == 0)
        writebits(_hTDC[0]); // Diff might be 0
    else
    {
        writebits(_hTDC[category[Diff]]);
        writebits(bitcode[Diff]);
    }

    // Encode ACs
    for (end0pos = 63; (end0pos > 0) && (DU[end0pos] == 0); end0pos--)
        ;

    // End0pos = first element in reverse order !=0
    if (end0pos == 0)
    {
        writebits(EOB);
        return;
    }

    i = 1;
    while (i <= end0pos)
    {
        startpos = i;
        for (; (i <= end0pos) && (DU[i] == 0); i++)
            ;
        nrzeroes = i - startpos;
        if (nrzeroes >= 16)
        {
            for (nrmarker = 1; nrmarker <= nrzeroes / 16; nrmarker++)
                writebits(M16zeroes);
            nrzeroes = nrzeroes % 16;
        }

        writebits(_hTAC[nrzeroes * 16 + category[DU[i]]]);
        writebits(bitcode[DU[i]]);
        i++;
    }

    if (end0pos != 63)
        writebits(EOB);
}

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void sImageEncoderJpeg::load_data_units_from_RGB_buffer(uint16 _xpos, uint16 _ypos)
{
    uint8  x, y;
    uint8  pos = 0;
    uint location;
    uint8  R, G, B;

    location = _ypos * Ximage + _xpos;
    for (y = 0; y < 8; y++)
    {
        for (x = 0; x < 8; x++)
        {
            R         = RGB_buffer[location].R;
            G         = RGB_buffer[location].G;
            B         = RGB_buffer[location].B;
            YDU[pos]  = Y(R, G, B);
            CbDU[pos] = Cb(R, G, B);
            CrDU[pos] = Cr(R, G, B);
            location++;
            pos++;
        }

        location += Ximage - 8;
    }
}

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void sImageEncoderJpeg::main_encoder()
{
    int16 DCY = 0, DCCb = 0, DCCr = 0; // DC coefficients used for differential encoding
    uint16 xpos, ypos;

    for (ypos = 0; ypos < Yimage; ypos += 8)
    {
        for (xpos = 0; xpos < Ximage; xpos += 8)
        {
            load_data_units_from_RGB_buffer(xpos, ypos);
            process_DU(YDU, fdtbl_Y, &DCY, YDC_HT, YAC_HT);
            process_DU(CbDU, fdtbl_Cb, &DCCb, CbDC_HT, CbAC_HT);
            process_DU(CrDU, fdtbl_Cb, &DCCr, CbDC_HT, CbAC_HT);
        }
    }
}

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void sImageEncoderJpeg::load_bitmap()
{
    uint16            Xdiv8, Ydiv8;
    uint8             nr_fillingbytes;
    colorRGB        lastcolor;
    uint16            column;
    uint16            nrline_up, nrline_dn, nrline;
    uint16            dimline;
    colorRGB*       tmpline;
    const uint8*      pref;
    uint32_t color;

    Ximage = (uint16) 660;
    Yimage = (uint16) 441;
    FILE* f = fopen("f:/j.bin", "rb");
    auto p = new uint8_t[Ximage * Yimage * 4];
    fread(p, Ximage * Yimage * 4, 1, f);
    fclose(f);
    pref   = p;

    // Of the image
    if (Ximage % 8 != 0)
        Xdiv8 = (Ximage / 8) * 8 + 8;
    else
        Xdiv8 = Ximage;
    if (Yimage % 8 != 0)
        Ydiv8 = (Yimage / 8) * 8 + 8;
    else
        Ydiv8 = Yimage;

    // The image we encode shall be filled with the last line and the last column
    // from the original bitmap, until Ximage and Yimage are divisible by 8 Load BMP
    // image from disk and complete X
    RGB_buffer = new colorRGB[Xdiv8 * Ydiv8];
    if (Ximage % 4 != 0)
        nr_fillingbytes = 4 - (Ximage % 4);
    else
        nr_fillingbytes = 0;
    for (nrline = 0; nrline < Yimage; nrline++)
    {
            memcpy(RGB_buffer + nrline * Xdiv8, pref, Ximage * 3);
            pref += Ximage * 3;

        memcpy(&lastcolor, RGB_buffer + nrline * Xdiv8 + Ximage - 1, 3);
        for (column = Ximage; column < Xdiv8; column++)
        {
            memcpy(RGB_buffer + nrline * Xdiv8 + column, &lastcolor, 3);
        }
    }

    Ximage  = Xdiv8;
    dimline = Ximage * 3;
    tmpline = new colorRGB[Ximage];

    // Reorder in memory the inversed bitmap
    for (nrline_up = Yimage - 1, nrline_dn = 0; nrline_up > nrline_dn; nrline_up--, nrline_dn++)
    {
        memcpy(tmpline, RGB_buffer + nrline_up * Ximage, dimline);
        memcpy(RGB_buffer + nrline_up * Ximage, RGB_buffer + nrline_dn * Ximage, dimline);
        memcpy(RGB_buffer + nrline_dn * Ximage, tmpline, dimline);
    }

    // Y completion:
    memcpy(tmpline, RGB_buffer + (Yimage - 1) * Ximage, dimline);
    for (nrline = Yimage; nrline < Ydiv8; nrline++)
    {
        memcpy(RGB_buffer + nrline * Ximage, tmpline, dimline);
    }

    Yimage = Ydiv8;
}

/*
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void sImageEncoderJpeg::init_all()
{
    set_DQTinfo();
    set_DHTinfo();
    init_Huffman_tables();
    set_numbers_category_and_bitcode();
    precalculate_YCbCr_tables();
    prepare_quant_tables();
}
