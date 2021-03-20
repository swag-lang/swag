#test
{
    var byte: u8 = 255
    ptr := &byte
    var k: s32 = 4
    ks := &k

    gs := cast(s32) ptr[0] * :ks
}