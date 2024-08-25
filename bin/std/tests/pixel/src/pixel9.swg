using Core, Pixel

//#[Swag.PrintBc]
#test
{
    var options: DecodeOptions
    options.decodePixels = false

    var pathName = dataPath()
    var files    = try Directory.enumerateFiles(pathName)
    foreach f in files
    {
        if Image.canLoad(f.fullname)
        {
            var img = try Image.load(f.fullname, options)
            @assert(img.pixels.buffer == null)
        }
    }
}
