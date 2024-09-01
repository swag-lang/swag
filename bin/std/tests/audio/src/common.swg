using Core

internal func dataPath()->String
{
    var dataPath = Path.getDirectoryName(#curlocation.fileName)
    dataPath = Path.getDirectoryName(dataPath)
    return Path.combine(dataPath, "datas")
}

internal func sndPath(name: string)->String
{
    return Path.combine(dataPath(), name)
}
