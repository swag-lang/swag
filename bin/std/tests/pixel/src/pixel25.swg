//#global skip
using Core, Pixel

/*internal func dataPath()->String
{
    var dataPath = Path.getDirectoryName(#curlocation.fileName)
    dataPath = Path.getDirectoryName(dataPath)
    return Path.combine(dataPath, "datas")
}

internal func tmpPath()->String
{
    var dataPath = Path.getDirectoryName(#curlocation.fileName)
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
    var img = try Image.load(imagePath("gameover.png"))
    try img.save(tmpPath("__gameover.png"))

    /*var img1 = try Image.load(tmpPath("__rgb.tga"))
    @assert(img.size == img1.size)
    @assert(Memory.compare(img.pixels.buffer, img1.pixels.buffer, img.size) == 0)*/

    try File.delete(tmpPath("__gameover.png"))
}
