using core

#test
{
    {
        f   := try file.readAllBytes(filePath("rfc1951.txt.z.0"))
        zip := try compress.ZLibStream.create(f.toSlice())
        res := try zip.decompress()
    }

    {
        f   := try file.readAllBytes(filePath("rfc1951.txt.z.9"))
        zip := try compress.ZLibStream.create(f.toSlice())
        res := try zip.decompress()
    }

    {
        f   := try file.readAllBytes(filePath("rfc1951.txt.fixed.z.9"))
        zip := try compress.ZLibStream.create(f.toSlice())
        res := try zip.decompress()
    }
}