using Core, Pixel.Image

#test
{
    var data = imagePath("rgb.bmp")
    var bytes = catch File.readAllBytes(data)
    @assert(!@errmsg)

    var dec: Bmp.Decoder
    var img = catch dec.IImageDecoder.decode(bytes.toSlice(), {})
    @assert(!@errmsg)
}