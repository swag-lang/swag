using image

//#[swag.printbc]
#test
{
    {
        data := imagePath("image1.bmp")
        img := try image.load(data)
    }

    {
        data := imagePath("image2.tga")
        img := try image.load(data)
    }
}