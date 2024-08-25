//#global skip
using Core

//#[Swag.PrintBc]
#test
{
    var arr = try File.readAllBytes(filePath("core349.bin"))

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
