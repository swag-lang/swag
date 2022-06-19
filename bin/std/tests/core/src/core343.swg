using Core

#test
{
    var arr: Array'u8
    loop i: 100
        arr.add(cast(u8) i)

    var comp: Compress.Deflate
    res := try comp.compress(arr.toSlice())

    var decomp: Compress.Inflate
    res1 := try decomp.decompress(res.toSlice())

    visit v, i: res1
        @assert(v == i)
}