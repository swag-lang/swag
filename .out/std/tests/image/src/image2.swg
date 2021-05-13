using image, core

#test
{
    data := imagePath("rgb.bmp")
    bytes := catch file.readAllBytes(data)
    @assert(!@err)

    var dec: bmp.Decoder
    img := catch dec.IDecoder.decode(bytes.toSlice(), null)
    @assert(!@err)
}