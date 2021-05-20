using image

//#[Swag.printbc]
#test
{
    var options: DecodeOptions
    options.decodePixels = false
    img := try image.load(imagePath("rgb.png"), options)
    @assert(img.width == 660)
    @assert(img.height == 441)
    @assert(img.pixels == null)

    /*pathName := dataPath()
    files := try Directory.enumerateFiles(pathName)
    visit f: files
    {
        img := try image.load(f.fullname)
    }  */
}