using Pixel.Image

//#[Swag.PrintBc]
#test
{
    var options: DecodeOptions
    options.decodePixels = false
    var img = try Image.load(imagePath("rgb.png"), options)
    @assert(img.width == 660)
    @assert(img.height == 441)
    @assert(img.pixels.buffer == null)
}
