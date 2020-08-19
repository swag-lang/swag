#module "compiler54"
;

var a: s8 = 127;
var b: s8 = 0x80's8;
var a1: s16 = 32767;
var a2: s32 = 127;
var a3: s64 = 127;

#unittest error;
var a10: s16 = 32768;
#unittest error;
var a11: s32 = 2147483648;
