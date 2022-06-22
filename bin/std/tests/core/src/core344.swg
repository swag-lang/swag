using Core

#test
{
    var arr: Array'u8
    loop i: 100
        arr.add(6)

    var comp: Compress.Deflate
    var res: Array'u8
    try comp.compress(&res, arr.toSlice())

    var decomp: Compress.Inflate
    res1 := try decomp.decompress(res.toSlice())

    @assert(res1.count == 100)
    visit v, i: res1
        @assert(v == 6)
}