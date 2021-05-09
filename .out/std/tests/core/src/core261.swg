using core

#test
{
    //f   := try file.readAllBytes(filePath("rfc1951.txt.z.0"))
    f   := try file.readAllBytes(filePath("rfc1951.txt.z.9"))
    zip := try compress.ZLibStream.create(f.toSlice())
    //res := try zip.decompress()
}