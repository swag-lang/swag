using Core, Pixel

#test
{
    img := try Image.load(imagePath("rgb.bmp"))
    try img.save(tmpPath("__rgb.bmp"))
    img1 := try Image.load(tmpPath("__rgb.bmp"))
    @assert(img.size == img1.size)
    @assert(Memory.compare(img.pixels, img1.pixels, img.size) == 0)
    try File.delete(tmpPath("__rgb.bmp"))
}