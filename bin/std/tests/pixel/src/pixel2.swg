using Core, Pixel.Image

#test
{
    data := imagePath("rgb.bmp")
    bytes := catch File.readAllBytes(data)
    @assert(!@errmsg)

    var dec: Bmp.Decoder
    img := catch dec.IImageDecoder.decode(bytes.toSlice(), {})
    @assert(!@errmsg)
}