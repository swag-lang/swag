#pragma once
#define uint uint32_t
#define uint8 uint8_t
#define int8 int8_t
#define uint16 uint16_t
#define int16 int16_t
#define uint64 uint64_t
#define int64 int64_t

class sImageDecoderJpeg
{
public:
    sImageDecoderJpeg();
    bool Decode();

private:
    static const uint INBUFSIZE       = 4096;
    static const uint MAXBLOCKSPERMCU = 10;
    static const uint MAXHUFFTABLES   = 8;
    static const uint MAXQUANTTABLES  = 4;
    static const uint MAXCOMPONENTS   = 4;
    static const uint MAXCOMPSINSCAN  = 4;
    static const uint MAXBLOCKSPERROW = 6144;
    static const uint MAXBLOCKS       = 100;
    static const uint MAX_HEIGHT      = 8192;
    static const uint MAX_WIDTH       = 8192;
    static const uint GRAYSCALE       = 0;
    static const uint YH1V1           = 1;
    static const uint YH2V1           = 2;
    static const uint YH1V2           = 3;
    static const uint YH2V2           = 4;
    static const int  FAILED          = -1;
    static const uint DONE            = 1;
    static const uint OKAY            = 0;
    static const uint CONST_BITS      = 13;
    static const uint PASS1_BITS      = 2;
    static const int  FIX_0_298631336 = 2446;
    static const int  FIX_0_390180644 = 3196;
    static const int  FIX_0_541196100 = 4433;
    static const int  FIX_0_765366865 = 6270;
    static const int  FIX_0_899976223 = 7373;
    static const int  FIX_1_175875602 = 9633;
    static const int  FIX_1_501321110 = 12299;
    static const int  FIX_1_847759065 = 15137;
    static const int  FIX_1_961570560 = 16069;
    static const int  FIX_2_053119869 = 16819;
    static const int  FIX_2_562915447 = 20995;
    static const int  FIX_3_072711026 = 25172;

private:
    typedef enum
    {
        M_SOF0  = 0xC0,
        M_SOF1  = 0xC1,
        M_SOF2  = 0xC2,
        M_SOF3  = 0xC3,
        M_SOF5  = 0xC5,
        M_SOF6  = 0xC6,
        M_SOF7  = 0xC7,
        M_JPG   = 0xC8,
        M_SOF9  = 0xC9,
        M_SOF10 = 0xCA,
        M_SOF11 = 0xCB,
        M_SOF13 = 0xCD,
        M_SOF14 = 0xCE,
        M_SOF15 = 0xCF,
        M_DHT   = 0xC4,
        M_DAC   = 0xCC,
        M_RST0  = 0xD0,
        M_RST1  = 0xD1,
        M_RST2  = 0xD2,
        M_RST3  = 0xD3,
        M_RST4  = 0xD4,
        M_RST5  = 0xD5,
        M_RST6  = 0xD6,
        M_RST7  = 0xD7,
        M_SOI   = 0xD8,
        M_EOI   = 0xD9,
        M_SOS   = 0xDA,
        M_DQT   = 0xDB,
        M_DNL   = 0xDC,
        M_DRI   = 0xDD,
        M_DHP   = 0xDE,
        M_EXP   = 0xDF,
        M_APP0  = 0xE0,
        M_APP1  = 0xE1,
        M_APP15 = 0xEF,
        M_JPG0  = 0xF0,
        M_JPG13 = 0xFD,
        M_COM   = 0xFE,
        M_TEM   = 0x01,
        M_ERROR = 0x100
    } marker;

    typedef struct
    {
        uint  look_up[256];
        uint8 code_size[256];
        uint  tree[512];
    } huff_tables_t;

    typedef struct
    {
        uint8* Pdata;
        int    block_num_x, block_num_y;
        int    block_len_x, block_len_y;
        int    block_size;
    } coeff_buf_t;

    typedef huff_tables_t* huff_tables_pt;
    typedef coeff_buf_t*   coeff_buf_pt;
    typedef void (*decode_block_func)(class sImageDecoderJpeg*, int, int, int);

private:
    int            image_x_size;
    int            image_y_size;
    bool           accept_meta;
    const uint8*   stream;
    uint           streamseek;
    uint           streamlen;
    int            progressive_flag;
    uint8*         huff_num[MAXHUFFTABLES];
    uint8*         huff_val[MAXHUFFTABLES];
    int16*         quant[MAXQUANTTABLES];
    int            scan_type;
    int            comps_in_frame;
    int            comp_h_samp[MAXCOMPONENTS];
    int            comp_v_samp[MAXCOMPONENTS];
    int            comp_quant[MAXCOMPONENTS];
    int            comp_ident[MAXCOMPONENTS];
    int            comp_h_blocks[MAXCOMPONENTS];
    int            comp_v_blocks[MAXCOMPONENTS];
    int            comps_in_scan;
    int            comp_list[MAXCOMPSINSCAN];
    int            comp_dc_tab[MAXCOMPONENTS];
    int            comp_ac_tab[MAXCOMPONENTS];
    int            spectral_start;
    int            spectral_end;
    int            successive_low;
    int            successive_high;
    int            max_mcu_x_size;
    int            max_mcu_y_size;
    int            blocks_per_mcu;
    int            max_blocks_per_row;
    int            mcus_per_row, mcus_per_col;
    int            mcu_org[MAXBLOCKSPERMCU];
    int            total_lines_left;
    int            mcu_lines_left;
    int            real_dest_bytes_per_scan_line;
    int            dest_bytes_per_scan_line;
    int            dest_bytes_per_pixel;
    void*          blocks[MAXBLOCKS];
    huff_tables_pt h[MAXHUFFTABLES];
    coeff_buf_pt   dc_coeffs[MAXCOMPONENTS];
    coeff_buf_pt   ac_coeffs[MAXCOMPONENTS];
    int            eob_run;
    int            block_y_mcu[MAXCOMPONENTS];
    uint8*         Pin_buf_ofs;
    int            in_buf_left;
    int            tem_flag;
    bool           eof_flag;
    uint8          padd_1[128];
    uint8          in_buf[INBUFSIZE + 128];
    uint8          padd_2[128];
    int            bits_left;
    uint           saved_mm1[2];
    int            restart_interval;
    int            restarts_left;
    int            next_restart_num;
    int            max_mcus_per_row;
    int            max_blocks_per_mcu;
    int            max_mcus_per_col;
    uint*          component[MAXBLOCKSPERMCU];
    uint           last_dc_val[MAXCOMPONENTS];
    huff_tables_pt dc_huff_seg[MAXBLOCKSPERMCU];
    huff_tables_pt ac_huff_seg[MAXBLOCKSPERMCU];
    int16*         block_seg[MAXBLOCKSPERROW];
    int            block_max_zag_set[MAXBLOCKSPERROW];
    uint8*         Psample_buf;
    int            crr[256];
    int            cbb[256];
    int            padd;
    int            crg[256];
    int            cbg[256];
    uint8*         scan_line_0;
    uint8*         scan_line_1;
    int16          temp_block[64];
    const char*    error_code;
    int            total_bytes_read;

    union
    {
        uint bit_buf;
        uint bit_buf_64[2];
    };

private:
    uint         get_char();
    uint         get_char(bool* _ppadding_flag);
    void         stuff_char(uint8 _q);
    uint8        get_octet();
    uint         get_bits_1(int _num_bits);
    uint         get_bits_2(int _numbits);
    int          huff_decode(huff_tables_pt _ph);
    void         free_all_blocks();
    void         terminate(const char* _status);
    void*        alloc(int _n);
    void         word_clear(void* _p, uint16 _c, uint _n);
    void         prep_in_buffer();
    void         read_dht_marker();
    void         read_dqt_marker();
    void         read_sof_marker();
    void         get_variable_marker(int _marker);
    void         read_dri_marker();
    void         read_sos_marker();
    int          next_marker();
    int          process_markers();
    void         locate_soi_marker();
    void         locate_sof_marker();
    int          locate_sos_marker();
    void         init(const uint8* _stream, uint _streamlen);
    void         create_look_ups();
    void         fix_in_buffer();
    void         transform_row();
    coeff_buf_pt coeff_buf_open(int _block_num_x, int _block_num_y, int _block_len_x, int _block_len_y);
    void         coeff_buf_read(coeff_buf_pt _cb, int _block_x, int _block_y, int16* _buffer);
    void         coeff_buf_write(coeff_buf_pt _cb, int _block_x, int _block_y, int16* _buffer);
    int16*       coeff_buf_getp(coeff_buf_pt _cb, int _block_x, int _block_y);
    void         load_next_row();
    void         process_restart();
    void         decode_next_row();
    void         H1V1Convert();
    void         H2V1Convert();
    void         H1V2Convert();
    void         GrayConvert();
    void         find_eoi();
    int          decode(void** _pscan_line_ofs, uint* _pscan_line_len);
    void         make_huff_table(int _index, huff_tables_pt _hs);
    void         check_quant_tables();
    void         check_huff_tables();
    void         calc_mcu_block_order();
    int          init_scan();
    void         init_frame();
    static void  decode_block_dc_first(sImageDecoderJpeg* _pd, int _component_id, int _block_x, int _block_y);
    static void  decode_block_dc_refine(sImageDecoderJpeg* _pd, int _component_id, int _block_x, int _block_y);
    static void  decode_block_ac_first(sImageDecoderJpeg* _pd, int _component_id, int _block_x, int _block_y);
    static void  decode_block_ac_refine(sImageDecoderJpeg* _pd, int _component_id, int _block_x, int _block_y);
    void         decode_scan(decode_block_func _decode_block_func);
    void         init_progressive();
    void         init_sequential();
    void         decode_start();
    void         idct(int16* _data, uint8* _pdst_ptr);
    void         H2V2Convert();
    void         decode_init(const uint8* _stream, uint _streamlen);
    void         decode_process(uint8* _pdest);
};
