#import "zlib"

#test
{
    const SRC = "this is something to compress"
    var DST: [100] u8
    var destLen: u32
    
    zlib.compress(@dataof(DST), &destLen, @dataof(SRC), @countof(SRC))
    @print(cast(s64) destLen)
}