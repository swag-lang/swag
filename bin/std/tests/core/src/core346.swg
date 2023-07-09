using Core

#test
{
    var arr: Array'u8

    var rng = Random.shared()
    loop 1000
    {
        var v = cast(u8) (rng.nextU32() % 256)
        arr.add(v)
    }

    var comp: Compress.Deflate
    var res: Array'u8

    comp.init()
    try comp.compress(&res, arr.toSlice())

    var decomp: Compress.Inflate
    var res1 = try decomp.decompress(res.toSlice())

    @assert(arr.count == res1.count)
    visit v, i: arr
    {
        @assert(v == res1[i])
    }
}