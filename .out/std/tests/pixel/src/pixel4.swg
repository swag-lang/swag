using Pixel

//#[Swag.printbc]
#test
{
    {
        img1 := try Image.load(imagePath("rgba.tga"))
        img2 := try Image.load(imagePath("rle32.tga"))
        @assert(img1.size == img2.size)
        @assert(@memcmp(img1.pixels, img2.pixels, img1.size) == 0)
    }

    {
        img1 := try Image.load(imagePath("rgb.tga"))
        img2 := try Image.load(imagePath("rle24.tga"))
        @assert(img1.size == img2.size)
        @assert(@memcmp(img1.pixels, img2.pixels, img1.size) == 0)
    }
}