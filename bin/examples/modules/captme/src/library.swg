var g_Library: Library

struct Library
{
    files: Array'File.FileInfo
}

impl Library
{
    func init() throw
    {
        var str = getFolder()
        if !Directory.exists(str) do
            Directory.create(str)
    }

    func getFolder()->String
    {
        var str = Env.getSpecialDirectory(.UserDocuments)
        Debug.assert(!str.isEmpty())
        return Path.combine(str, "captMe")
    }

    func newFileName()->String
    {
        var t        = Time.DateTime.now()
        var fileName = Format.toString("%_%_%_%_%_%.captme", t.month, t.day, t.year, t.hour, t.minute, t.second)
        return Path.combine(getFolder(), fileName.toString())
    }

    mtd fill()
    {
        files = catch Directory.enumerateFiles(getFolder(), ".captme")
    }

    mtd removeFile(name: string)
    {
        visit fi, idx: files
        {
            if fi.fullname == name
            {
                files.removeAtOrdered(idx)
                break
            }
        }
    }

    mtd addFile(name: string)
    {
        var file = catch File.getFileInfo(name)
        if @err() != null do
            return
        files.add(file)
    }
}
