using image

//#[swag.printbc]
#test
{
    {
        img1 := try image.load(imagePath("rgba.tga"))
        img2 := try image.load(imagePath("rle32.tga"))
        @assert(img1.size == img2.size)
        @assert(@memcmp(img1.pixels, img2.pixels, img1.size) == 0)
    }

    {
        img1 := try image.load(imagePath("rgb.tga"))
        img2 := try image.load(imagePath("rle24.tga"))
        @assert(img1.size == img2.size)
        @assert(@memcmp(img1.pixels, img2.pixels, img1.size) == 0)
    }
}