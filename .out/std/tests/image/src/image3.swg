using image

//#[swag.printbc]
#test
{
    {
        data := imagePath("bmp_24.bmp")
        img := try image.load(data)
    }

    {
        data := imagePath("bmp_32.bmp")
        img := try image.load(data)
    }
}