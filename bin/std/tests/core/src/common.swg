using Core, Core.Memory

internal func dataPath()->String
{
    var dataPath = Path.getDirectoryName(#location.fileName)
    dataPath = Path.getDirectoryName(dataPath)
    return Path.combine(dataPath, "datas")
}

internal func filePath(name: string)->String
{
    return Path.combine(dataPath(), name)
}

#premain
{
    try Jobs.setNumWorkers(4)
}