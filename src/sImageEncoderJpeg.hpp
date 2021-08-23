#pragma once
#define uint16 uint16_t
#define uint8 uint8_t
#define int16 int16_t
#define int8 int8_t
#define uint uint32_t

class sImageEncoderJpeg
{
public:
    sImageEncoderJpeg() = default;
    void SetQuality(uint8 _quality);

    struct DQTinfotype
    {
        uint16 marker;
        uint16 length;
        uint8  QTYinfo;
        uint8  Ytable[64];
        uint8  QTCbinfo;
        uint8  Cbtable[64];
    } DQTinfo;

    struct DHTinfotype
    {
        uint16 marker;
        uint16 length;
        uint8  HTYDCinfo;
        uint8  YDC_nrcodes[16];
        uint8  YDC_values[12];
        uint8  HTYACinfo;
        uint8  YAC_nrcodes[16];
        uint8  YAC_values[162];
        uint8  HTCbDCinfo;
        uint8  CbDC_nrcodes[16];
        uint8  CbDC_values[12];
        uint8  HTCbACinfo;
        uint8  CbAC_nrcodes[16];
        uint8  CbAC_values[162];
    } DHTinfo;

    struct colorRGB
    {
        uint8 B, G, R;
    };

    struct bitstring 
    {
        uint8  length;
        uint16 value;
    };

    uint8           m_ScaleFactor = 10;
    uint8           bytenew;
    int8           bytepos;
    bitstring     YDC_HT[12];
    bitstring     CbDC_HT[12];
    bitstring     YAC_HT[256];
    bitstring     CbAC_HT[256];
    uint8*          category_alloc;
    uint8*          category;
    bitstring*    bitcode_alloc;
    bitstring*    bitcode;
    int          YRtab[256], YGtab[256], YBtab[256];
    int          CbRtab[256], CbGtab[256], CbBtab[256];
    int          CrRtab[256], CrGtab[256], CrBtab[256];
    float          fdtbl_Y[64];
    float          fdtbl_Cb[64];
    colorRGB*     RGB_buffer;
    uint16          Ximage, Yimage;
    int8           YDU[64];
    int8           CbDU[64];
    int8           CrDU[64];
    int16          DU_DCT[64];
    int16          DU[64];

    bool Encode();

    uint8  Y(uint8 _r, uint8 _g, uint8 _b);
    uint8  Cb(uint8 _r, uint8 _g, uint8 _b);
    uint8  Cr(uint8 _r, uint8 _g, uint8 _b);
    void writebyte(uint8 _b);
    void writeword(uint16 _w);
    void write_APP0info();
    void write_SOF0info();
    void write_DQTinfo();
    void set_quant_table(const uint8* _basic_table, uint8 _scale_factor, uint8* _newtable);
    void set_DQTinfo();
    void write_DHTinfo();
    void set_DHTinfo();
    void write_SOSinfo();
    void writebits(bitstring _bs);
    void compute_Huffman_table(const uint8* _nrcodes, const uint8* _std_table, bitstring* _hT);
    void init_Huffman_tables();
    void set_numbers_category_and_bitcode();
    void precalculate_YCbCr_tables();
    void prepare_quant_tables();
    void fdct_and_quantization(int8* _data, float* _fdtbl, int16* _outdata);
    void process_DU(int8* _componentDU, float* _fdtbl, int16* _dC, bitstring* _hTDC, bitstring* _hTAC);
    void load_data_units_from_RGB_buffer(uint16 _xpos, uint16 _ypos);
    void main_encoder();
    void load_bitmap();
    void init_all();
};
