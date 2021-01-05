
#global testpass lexer;

#unittest error;
0xFFFFFFFF'u8
#unittest error;
0xFFFFFFFF'u16
#unittest error;
0xFFFFFFFFF'u32

#unittest error;
0xFFFFFFFF's8
#unittest error;
0xFFFFFFFF's16
#unittest error;
0xFFFFFFFFF's32

#unittest error;
0xFFFFFFFFF'bool
#unittest error;
0xFFFFFFFFF'f32
#unittest error;
0xFFFFFFFFF'f64

#global testpass lexer;
0x7F's8
0x00000007F's8
0xFFFF's16
0xFFFF_FFFF's32
0xFFFF_FFFF_FFFF_FFFF's64