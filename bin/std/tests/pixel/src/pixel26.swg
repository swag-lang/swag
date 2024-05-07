using Core, Pixel

#test
{
    {
        var imgA = assume Image.load(imagePath("testA.tga"))
        assume imgA.save(tmpPath("testA.tga"))
        var imgB = assume Image.load(tmpPath("testA.tga"))
        @assert(Memory.compare(imgA.pixels.buffer, imgB.pixels.buffer, imgA.width * imgA.height * imgA.bpp8) == 0)
    }

    {
        var imgA = assume Image.load(imagePath("testA.tga"))
        imgA.setPixelFormat(.RGB8)
        assume imgA.save(tmpPath("testA.tga"))
        var imgB = assume Image.load(tmpPath("testA.tga"))
        imgB.setPixelFormat(.RGB8)
        @assert(Memory.compare(imgA.pixels.buffer, imgB.pixels.buffer, imgA.width * imgA.height * imgA.bpp8) == 0)
    }

    {
        var imgA = assume Image.load(imagePath("testA.tga"))
        imgA.setPixelFormat(.RGBA8)
        assume imgA.save(tmpPath("testA.tga"))
        var imgB = assume Image.load(tmpPath("testA.tga"))
        imgB.setPixelFormat(.RGBA8)
        @assert(Memory.compare(imgA.pixels.buffer, imgB.pixels.buffer, imgA.width * imgA.height * imgA.bpp8) == 0)
    }

    try File.delete(tmpPath("testA.tga"))
}
