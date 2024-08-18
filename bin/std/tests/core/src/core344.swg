using Core

#test
{
    var arr: Array'u8
    loop i in 100 do
        arr.add(6)

    var comp: Compress.Deflate
    var res:  Array'u8

    comp.init()
    try comp.compress(&res, arr.toSlice())

    var decomp: Compress.Inflate
    var res1    = try decomp.decompress(res.toSlice())

    @assert(res1.count == 100)
    visit v, i in res1 do
        @assert(v == 6)
}
