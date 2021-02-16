using image, core

#test
{
    data := imagePath("image1.bmp")
    bytes := catch file.readAllBytes(data)
    @assert(!@err)

    var dec: bmp.Decoder
    img := catch dec.decode(bytes.toSlice())
    @assert(!@err)
}