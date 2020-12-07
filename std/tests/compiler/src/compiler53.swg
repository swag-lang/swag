#module "compiler53"
;

var a: u16 = 256;
var b: u32 = 256;
var c: u64 = 256;
var cc: u64 = 0xFFFFFFFF_FFFFFFFF;

#unittest error;
var aa: u16 = 0xFFFF_1;
#unittest error;
var bb: u32 = 0xFFFFFFFF_1;


