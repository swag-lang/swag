using Core, Pixel

#test
{
    try
    {
        var img = Image.load(imagePath("h1v1.jpg"))

        var opt: Jpg.EncodeOptions
        opt.quality = 1
        img.save(tmpPath("__h1v1A.jpg"), opt)
        opt.quality = 100
        img.save(tmpPath("__h1v1B.jpg"), opt)

        var img1 = Image.load(tmpPath("__h1v1A.jpg"))
        var img2 = Image.load(tmpPath("__h1v1B.jpg"))

        File.delete(tmpPath("__h1v1A.jpg"))
        File.delete(tmpPath("__h1v1B.jpg"))
    }
}