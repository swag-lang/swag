using Core, Pixel

#test
{
    var img = try Image.load(imagePath("rgb.bmp"))
    try img.save(tmpPath("__rgb.bmp"))
    var img1 = try Image.load(tmpPath("__rgb.bmp"))
    @assert(img.size == img1.size)
    @assert(Memory.compare(img.pixels.buffer, img1.pixels.buffer, img.size) == 0)
    try File.delete(tmpPath("__rgb.bmp"))
}
