using Core, Memory

var g_dbg: Swag.DebugAllocator
var g_cxt: Swag.Context

private func dataPath()->String
{
    dataPath := Path.getDirectoryName(#location.fileName)
    dataPath = Path.getDirectoryName(dataPath)
    return Path.combine(dataPath, "datas")
}

private func filePath(name: string)->String
{
    return Path.combine(dataPath(), name)
}

#premain
{
    try Jobs.setNumWorkers(4)
    Swag.DebugAllocator.install()
}