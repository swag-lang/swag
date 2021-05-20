using core

#test
{
    var dec: Compress.ZLibDecoder

    {
        f   := try File.readAllBytes(filePath("0.zlib"))
        res := try dec.decompress(f.toSlice())
    }

    {
        f   := try File.readAllBytes(filePath("9.zlib"))
        res := try dec.decompress(f.toSlice())
    }

    {
        f   := try File.readAllBytes(filePath("fixed9.zlib"))
        res := try dec.decompress(f.toSlice())
    }
}