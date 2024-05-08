using Core, Pixel

#test
{
    var imgA = assume Image.load(imagePath("resize.png"))
    imgA.resize(700, 700, .Bilinear)
    @assert(imgA.getPixelColor(0, 0) == imgA.getPixelColor(4, 4))
    //assume imgA.save(tmpPath("resize.png"))
    //try File.delete(tmpPath("resize.png"))
}
