using Core, Pixel

#test
{
    try
    {
        img := Image.load(imagePath("h1v1.jpg"))

        img.save(tmpPath("__h1v1A.jpg"), EncodeOptions{quality: 1})
        img.save(tmpPath("__h1v1B.jpg"), EncodeOptions{quality: 100})

        img1 := Image.load(tmpPath("__h1v1A.jpg"))
        img2 := Image.load(tmpPath("__h1v1B.jpg"))

        //File.delete(tmpPath("__h1v1A.jpg"))
        //File.delete(tmpPath("__h1v1B.jpg"))
    }
}