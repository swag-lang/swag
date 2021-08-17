using Core, Pixel

#test
{
    img := try Image.load(imagePath("rgb.bmp"))
    try img.save(imagePath("__rgb.bmp"))
    img1 := try Image.load(imagePath("__rgb.bmp"))
    @assert(img.size == img1.size)
    @assert(Memory.compare(img.pixels, img1.pixels, img.size) == 0)
}