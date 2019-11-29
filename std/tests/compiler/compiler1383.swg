#import "zlib"

#test
{
    const SRC = "this is something to compress"
    var DST: [100] u8
    var DST1: [100] u8
    var destLen: u32 = @sizeof(DST)
    
    result := zlib.compress(@dataof(DST), &destLen, @dataof(SRC), @countof(SRC) + 1)
    @assert(result == zlib.Z_OK)
    
    destLen1 := destLen
    result = zlib.uncompress(@dataof(DST1), &destLen1, @dataof(DST), destLen)
    @assert(result == zlib.Z_OK)
    
    var tt: [..] u8 = [&DST1, destLen1]
    @print(tt)
}