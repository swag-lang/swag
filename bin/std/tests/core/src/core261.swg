using Core

#test
{
    var dec: Compress.ZLib

    {
        var f   = try File.readAllBytes(filePath("0.zlib"))
        var res = try dec.decompress(f.toSlice())
    }

    {
        var f   = try File.readAllBytes(filePath("9.zlib"))
        var res = try dec.decompress(f.toSlice())
    }

    {
        var f   = try File.readAllBytes(filePath("fixed9.zlib"))
        var res = try dec.decompress(f.toSlice())
    }
}
