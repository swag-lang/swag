using core

#test
{
    {
        f   := try file.readAllBytes(filePath("0.zlib"))
        zip := try compress.ZLibStream.create(f.toSlice())
        res := try zip.decompress()
    }

    {
        f   := try file.readAllBytes(filePath("9.zlib"))
        zip := try compress.ZLibStream.create(f.toSlice())
        res := try zip.decompress()
    }

    {
        f   := try file.readAllBytes(filePath("fixed9.zlib"))
        zip := try compress.ZLibStream.create(f.toSlice())
        res := try zip.decompress()
    }
}