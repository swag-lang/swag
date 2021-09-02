using Pixel

//#[Swag.PrintBc]
#test
{
    {
        data := imagePath("rgb.bmp")
        img := try Image.load(data)
    }

    {
        data := imagePath("rgba.bmp")
        img := try Image.load(data)
    }
}