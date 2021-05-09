using core

#test
{
    f   := try file.readAllBytes(filePath("rfc1951.txt.z.0"))
    zip := try compress.ZLibStream.create(f.toSlice())
    res := try zip.decompress()
}