using image

//#[Swag.printbc]
#test
{
    {
        data := imagePath("rgb.bmp")
        img := try image.load(data)
    }

    {
        data := imagePath("rgba.bmp")
        img := try image.load(data)
    }
}