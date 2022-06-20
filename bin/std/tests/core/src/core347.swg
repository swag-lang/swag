//#global skip
using Core

//#[Swag.PrintBc]
#test
{
    arr := try File.readAllBytes(filePath("0.zlib"))

    var comp: Compress.Deflate
    res := try comp.compress(arr.toSlice())

    var decomp: Compress.Inflate
    res1 := try decomp.decompress(res.toSlice())

    @assert(arr.count == res1.count)
    visit v, i: arr
    {
        @assert(v == res1[i])
    }

    //@print(@mkstring(res1.buffer, res1.count))
}