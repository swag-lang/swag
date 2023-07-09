using Core

private func dataPath()->String
{
    var dataPath = Path.getDirectoryName(#location.fileName)
    dataPath = Path.getDirectoryName(dataPath)
    return Path.combine(dataPath, "datas")
}

private func tmpPath()->String
{
    var dataPath = Path.getDirectoryName(#location.fileName)
    dataPath = Path.getDirectoryName(dataPath)
    return Path.combine(dataPath, "tmp")
}

private func imagePath(name: string)->String
{
    return Path.combine(dataPath(), name)
}

private func tmpPath(name: string)->String
{
    return Path.combine(tmpPath(), name)
}

#init
{
    //@dbgalloc().breakOnAllocId = 14244
    //@dbgalloc().showMaxLeaks = 2
    //@dbgalloc().captureAllocStack = true
}