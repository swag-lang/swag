//#global skip
using Core, Pixel

#[Swag.Overload]
internal func tmpPath1()->String
{
    var dataPath = Path.getDirectoryName(#location.fileName)
    dataPath = Path.getDirectoryName(dataPath)
    return Path.combine(dataPath, "tmp")
}

#[Swag.Overload, Swag.Inline]
internal func tmpPath1(name: string)->String
{
    return Path.combine(tmpPath1(), name)
}

//#[Swag.PrintBc]
#test
{
    var img = Image.create(10, 10, PixelFormat.RGBA8)
    try img.save(tmpPath1("__gameover.png"))
    try File.delete(tmpPath1("__gameover.png"))
}
