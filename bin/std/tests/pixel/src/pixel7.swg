using Pixel

//#[Swag.PrintBc]
#test
{
    var img = try Image.load(imagePath("rgb.png"))
    @assert(img.width == 660)
    @assert(img.height == 441)
}
