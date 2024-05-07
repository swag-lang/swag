using Core, Pixel

#test
{
    {
        var imgA = assume Image.load(imagePath("testA.jpg"))
        assume imgA.save(tmpPath("testA.jpg"))
//        var imgB = assume Image.load(tmpPath("testA.jpg"))
//        @assert(Memory.compare(imgA.pixels.buffer, imgB.pixels.buffer, imgA.width * imgA.height * imgA.bpp8) == 0)
    }
/*
    {
        var imgA = assume Image.load(imagePath("testA.jpg"))
        imgA.setPixelFormat(.RGB8)
        assume imgA.save(tmpPath("testA.jpg"))
        var imgB = assume Image.load(tmpPath("testA.jpg"))
        imgB.setPixelFormat(.RGB8)
        //@assert(Memory.compare(imgA.pixels.buffer, imgB.pixels.buffer, imgA.width * imgA.height * imgA.bpp8) == 0)
    }

    {
        var imgA = assume Image.load(imagePath("testA.jpg"))
        imgA.setPixelFormat(.RGBA8)
        assume imgA.save(tmpPath("testA.jpg"))
        var imgB = assume Image.load(tmpPath("testA.jpg"))
        imgB.setPixelFormat(.RGBA8)
        //@assert(Memory.compare(imgA.pixels.buffer, imgB.pixels.buffer, imgA.width * imgA.height * imgA.bpp8) == 0)
    }*/

    try File.delete(tmpPath("testA.jpg"))
}
