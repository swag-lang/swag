using Core

internal func dataPath()->String
{
    var dataPath = Path.getDirectoryName(#curlocation.fileName)
    dataPath = Path.getDirectoryName(dataPath)
    return Path.combine(dataPath, "datas")
}

#[Swag.Overload]
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

#[Swag.Overload]
internal func tmpPath(name: string)->String
{
    return Path.combine(tmpPath(), name)
}

#init
{
    //@dbgalloc().breakOnAllocId = 14244
    //@dbgalloc().showMaxLeaks = 2
    //@dbgalloc().captureAllocStack = true
}
