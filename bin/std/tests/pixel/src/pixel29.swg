using Core, Pixel

#test
{
    {
        var imgA = assume Image.load(imagePath("testA.png"))
        assume imgA.save(tmpPath("testA.png"))
        var imgB = assume Image.load(tmpPath("testA.png"))
        @assert(Memory.compare(imgA.pixels.buffer, imgB.pixels.buffer, imgA.width * imgA.height * imgA.bpp8) == 0)
    }

    {
        var imgA = assume Image.load(imagePath("testA.png"))
        imgA.setPixelFormat(.RGB8)
        assume imgA.save(tmpPath("testA.png"))
        var imgB = assume Image.load(tmpPath("testA.png"))
        imgB.setPixelFormat(.RGB8)
        @assert(Memory.compare(imgA.pixels.buffer, imgB.pixels.buffer, imgA.width * imgA.height * imgA.bpp8) == 0)
    }

    {
        var imgA = assume Image.load(imagePath("testA.png"))
        imgA.setPixelFormat(.RGBA8)
        assume imgA.save(tmpPath("testA.png"))
        var imgB = assume Image.load(tmpPath("testA.png"))
        imgB.setPixelFormat(.RGBA8)
        @assert(Memory.compare(imgA.pixels.buffer, imgB.pixels.buffer, imgA.width * imgA.height * imgA.bpp8) == 0)
    }

    try File.delete(tmpPath("testA.png"))
}
