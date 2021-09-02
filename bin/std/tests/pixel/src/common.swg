using Core

protected func dataPath()->String
{
    dataPath := Path.getDirectoryName(#location.fileName)
    dataPath = Path.getDirectoryName(dataPath)
    return Path.combine(dataPath, "datas")
}

protected func tmpPath()->String
{
    dataPath := Path.getDirectoryName(#location.fileName)
    dataPath = Path.getDirectoryName(dataPath)
    return Path.combine(dataPath, "tmp")
}

protected func imagePath(name: string)->String
{
    return Path.combine(dataPath(), name)
}

protected func tmpPath(name: string)->String
{
    return Path.combine(tmpPath(), name)
}
