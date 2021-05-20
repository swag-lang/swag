using core

#test
{
    var dec: Compress.ZLibDecoder

    {
        f   := try file.readAllBytes(filePath("0.zlib"))
        res := try dec.decompress(f.toSlice())
    }

    {
        f   := try file.readAllBytes(filePath("9.zlib"))
        res := try dec.decompress(f.toSlice())
    }

    {
        f   := try file.readAllBytes(filePath("fixed9.zlib"))
        res := try dec.decompress(f.toSlice())
    }
}