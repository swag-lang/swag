using Core, Pixel

#test
{
    var img = try Image.load(imagePath("rgb.tga"))
    try img.save(tmpPath("__rgb.tga"))
    var img1 = try Image.load(tmpPath("__rgb.tga"))
    @assert(img.size == img1.size)
    @assert(Memory.compare(img.pixels.buffer, img1.pixels.buffer, img.size) == 0)
    try File.delete(tmpPath("__rgb.tga"))
}
