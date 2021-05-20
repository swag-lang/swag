using image, core

#test
{
    data := imagePath("rgb.bmp")
    bytes := catch File.readAllBytes(data)
    @assert(!@err)

    var dec: bmp.Decoder
    img := catch dec.IDecoder.decode(bytes.toSlice(), @{})
    @assert(!@err)
}