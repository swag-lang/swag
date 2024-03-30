using Pixel

//#[Swag.PrintBc]
#test
{
    {
        var img1 = try Image.load(imagePath("rgba.tga"))
        var img2 = try Image.load(imagePath("rle32.tga"))
        @assert(img1.size == img2.size)
        @assert(@memcmp(img1.pixels.buffer, img2.pixels.buffer, img1.size) == 0)
    }

    {
        var img1 = try Image.load(imagePath("rgb.tga"))
        var img2 = try Image.load(imagePath("rle24.tga"))
        @assert(img1.size == img2.size)
        @assert(@memcmp(img1.pixels.buffer, img2.pixels.buffer, img1.size) == 0)
    }
}
