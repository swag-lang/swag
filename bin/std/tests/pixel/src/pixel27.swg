using Core, Pixel

#test
{
    {
        var imgA = assume Image.load(imagePath("testA.bmp"))
        assume imgA.save(tmpPath("testA.bmp"))
        var imgB = assume Image.load(tmpPath("testA.bmp"))
        @assert(Memory.compare(imgA.pixels.buffer, imgB.pixels.buffer, imgA.width * imgA.height * imgA.bpp8) == 0)
    }

    {
        var imgA = assume Image.load(imagePath("testA.bmp"))
        imgA.setPixelFormat(.RGB8)
        assume imgA.save(tmpPath("testA.bmp"))
        var imgB = assume Image.load(tmpPath("testA.bmp"))
        imgB.setPixelFormat(.RGB8)
        @assert(Memory.compare(imgA.pixels.buffer, imgB.pixels.buffer, imgA.width * imgA.height * imgA.bpp8) == 0)
    }

    {
        var imgA = assume Image.load(imagePath("testA.bmp"))
        imgA.setPixelFormat(.RGBA8)
        assume imgA.save(tmpPath("testA.bmp"))
        var imgB = assume Image.load(tmpPath("testA.bmp"))
        imgB.setPixelFormat(.RGBA8)
        @assert(Memory.compare(imgA.pixels.buffer, imgB.pixels.buffer, imgA.width * imgA.height * imgA.bpp8) == 0)
    }

    try File.delete(tmpPath("testA.bmp"))
}
