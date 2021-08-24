using Core, Pixel

#test
{
    img := try Image.load(imagePath("h1v1.jpg"))
    try img.save(tmpPath("__h1v1.jpg"))
    img1 := try Image.load(tmpPath("__h1v1.jpg"))
    try File.delete(tmpPath("__h1v1.jpg"))
}