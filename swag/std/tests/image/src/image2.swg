using image, core

#test
{
    data := imagePath("bmp_24.bmp")
    bytes := catch file.readAllBytes(data)
    @assert(!@err)

    var dec: bmp.Decoder
    img := catch dec.IDecoder.decode(bytes.toSlice())
    @assert(!@err)
}