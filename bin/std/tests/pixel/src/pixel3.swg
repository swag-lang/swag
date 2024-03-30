using Pixel

//#[Swag.PrintBc]
#test
{
    {
        var data = imagePath("rgb.bmp")
        var img  = try Image.load(data)
    }

    {
        var data = imagePath("rgba.bmp")
        var img  = try Image.load(data)
    }
}
