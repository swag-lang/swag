using core, image

//#[swag.printbc]
#test
{
    var options: DecodeOptions
    options.decodePixels = false

    pathName := dataPath()
    files := try directory.enumerateFiles(pathName)
    visit f: files
    {
        img := try image.load(f.fullname, options)
        @assert(img.pixels == null)
    }
}