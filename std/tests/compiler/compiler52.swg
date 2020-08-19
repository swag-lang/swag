#module "compiler52"
;

#unittest error;
var a: u8 = 256;
var b: u8 = 255;

#unittest error;
var c: u8 = 0xFF1;
var d: u8 = 0xFF;

#unittest error;
var e: u8 = 0b11111111_1;
var f: u8 = 0b11111111;

