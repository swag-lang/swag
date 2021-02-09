using core

protected
{
    func dataPath()->String
    {
        dataPath := path.getDirectoryName(#location.fileName)
        dataPath = path.getDirectoryName(dataPath)
        return path.combine(dataPath, "datas")
    }

    func imagePath(name: string)->String
    {
        return path.combine(dataPath(), name)
    }
}