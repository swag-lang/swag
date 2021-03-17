using image

//#[swag.printbc]
#test
{
    {
        img1 := try image.load(imagePath("tga_32.tga"))
        img2 := try image.load(imagePath("tga_rle32.tga"))
        @assert(img1.size == img2.size)
        @assert(@memcmp(img1.pixels, img2.pixels, img1.size) == 0)
    }

    {
        img1 := try image.load(imagePath("tga_24.tga"))
        img2 := try image.load(imagePath("tga_rle24.tga"))
        @assert(img1.size == img2.size)
        @assert(@memcmp(img1.pixels, img2.pixels, img1.size) == 0)
    }
}