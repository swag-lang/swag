using Core

#test
{
    var arr: Array'u8

    let rng = Random.shared()
    for 1000
    {
        let v = cast(u8) (rng.nextU32() % 256)
        arr.add(v)
    }

    var comp: Compress.Deflate
    var res:  Array'u8

    comp.init()
    try comp.compress(&res, arr.toSlice())

    var decomp: Compress.Inflate
    var res1:   Array'u8
    try decomp.decompress(&res1, res.toSlice())

    @assert(arr.count == res1.count)
    foreach v, i in arr
    {
        @assert(v == res1[i])
    }
}
