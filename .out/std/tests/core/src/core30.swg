/*#test
{
    const SRC = "this is something to compress this is something to compress"
    var DST: [100] u8
    var DST1: [100] u8
    var destLen: u32 = @sizeof(DST)

    result := zlib.compress(@dataof(DST), &destLen, @dataof(SRC), cast(u32) @countof(SRC) + 1)
    @assert(result == zlib.Z_OK)
    @assert(destLen < @countof(SRC) + 1)

    var destLen1: u32 = @sizeof(DST1)
    result = zlib.uncompress(@dataof(DST1), &destLen1, @dataof(DST), destLen)
    @assert(result == zlib.Z_OK)
    @assert(destLen1 == @countof(SRC) + 1)

    //var tt: [..] u8 = [&DST1, destLen1]
    //@print(tt)
    c := zlib.compressBound(100)
}*/