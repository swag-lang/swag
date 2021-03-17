;
;

#assert 0xFF << 4 == 0xFF0;
#assert 1 << 0x1 == 2;
#assert 0b1001 << 0b1 == 0b10010;

#assert 0b10010 >> 0b1 == 0b1001;
#assert 0xF0 >> 0x4 == 15'u8;
