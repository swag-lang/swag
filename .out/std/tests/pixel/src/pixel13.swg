using Pixel

#test
{
    data := imagePath("rgb.bmp")
    img := try Image.load(data)
    try img.save(imagePath("__rgb.bmp"))
    img1 := try Image.load(imagePath("__rgb.bmp"))
}