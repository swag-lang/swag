using Core

#test
{
    var arr: Array'u8
    for i in 100:
        arr.add(6)

    var comp: Compress.Deflate
    var res:  Array'u8

    comp.init()
    try comp.compress(&res, arr.toSlice())

    var decomp: Compress.Inflate
    var res1    = try decomp.decompress(res.toSlice())

    @assert(res1.count == 100)
    foreach v, i in res1:
        @assert(v == 6)
}
