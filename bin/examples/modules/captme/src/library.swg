var g_Library: Library

struct Library
{
    files:            Array'File.FileInfo
    onDeleteFile:     SigArray'func||(string)
}

impl Library
{
    func init() throw
    {
        var str = getFolder()
        if !Directory.exists(str):
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
        me.files = catch Directory.enumerateFiles(getFolder(), ".captme")
    }

    mtd removeFile(name: string)
    {
        foreach fi, idx in me.files
        {
            if fi.fullname == name
            {
                catch File.delete(name)
                if @err == null
                {
                    me.files.removeAtOrdered(idx)
                    me.onDeleteFile.call(name)
                }

                break
            }
        }
    }

    mtd addFile(name: string)
    {
        var file = catch File.getFileInfo(name)
        if @err != null:
            return
        me.files.add(file)
    }
}
