//#global skip
using Core, Pixel

/*internal func dataPath()->String
{
    var dataPath = Path.getDirectoryName(#location.fileName)
    dataPath = Path.getDirectoryName(dataPath)
    return Path.combine(dataPath, "datas")
}

internal func tmpPath()->String
{
    var dataPath = Path.getDirectoryName(#location.fileName)
    dataPath = Path.getDirectoryName(dataPath)
    return Path.combine(dataPath, "tmp")
}

internal func imagePath(name: string)->String
{
    return Path.combine(dataPath(), name)
}

internal func tmpPath(name: string)->String
{
    return Path.combine(tmpPath(), name)
}*/

#test
{
    try
    {
        var img  = Image.load(imagePath("h1v1.jpg"))
        var opt: Png.EncodeOptions
        img.save(tmpPath("__h1v1.png"), opt)
        var img1 = Image.load(tmpPath("__h1v1.png"))
        File.delete(tmpPath("__h1v1.png"))
    }
}
