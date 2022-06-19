using Core
const C = #load(#file)

#test
{
    var arr: Array'u8
    arr.add(@mkslice(&C[0], @countof(C)))

    var comp: Compress.Deflate
    res := try comp.compress(arr.toSlice())

    var decomp: Compress.Inflate
    res1 := try decomp.decompress(res.toSlice())

    @assert(@countof(C) == res1.count)
    visit v, i: C
        @assert(v == res1[i])
}