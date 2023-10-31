using Core, Pixel.Image

#test
{
    var data = imagePath("rgb.bmp")
    var bytes = catch File.readAllBytes(data)
    @assert(!@err())

    var dec: Bmp.Decoder
    var img = catch dec.IImageDecoder.decode(bytes.toSlice(), {})
    @assert(!@err())
}