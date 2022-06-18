#global skip
using Core, Pixel

#test
{
    try
    {
        img := Image.load(imagePath("h1v1.jpg"))
        img.save(tmpPath("__h1v1.png"))
        img1 := Image.load(tmpPath("__h1v1.png"))
        File.delete(tmpPath("__h1v1.png"))
    }
}