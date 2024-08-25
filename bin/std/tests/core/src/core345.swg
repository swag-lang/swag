using Core
const C = """
using Core
const C = #include #file

//#[Swag.PrintBc]
#[Swag.Optimize("", false)]
#test
{
    arr.add(@mkslice(&C[0], @countof(C)))
    @assert(@countof(C) == arr.count)

    var comp: Compress.Deflate
    var res = try comp.compress(arr.toSlice())

    var decomp: Compress.Inflate
    var res1 = try decomp.decompress(res.toSlice())

    @assert(arr.count == res1.count)
    @print(@mkstring(res1.buffer, res1.count))
}"""

//#[Swag.PrintBc]
#[Swag.Optimize("", false)]
#test
{
    var arr: Array'u8
    arr.add(@mkslice(@dataof(C), @countof(C)))
    @assert(@countof(C) == arr.count)

    var comp: Compress.Deflate
    var res:  Array'u8

    comp.init()
    try comp.compress(&res, arr.toSlice())

    var decomp: Compress.Inflate
    var res1    = try decomp.decompress(res.toSlice())

    @assert(arr.count == res1.count)
    foreach v, i in arr
    {
        @assert(v == res1[i])
    }

    //@print(@mkstring(res1.buffer, res1.count))
}
