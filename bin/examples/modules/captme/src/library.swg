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
        let str = getFolder()
        if !Directory.exists(str) do
            Directory.create(str)
    }

    func getFolder()->String
    {
        let str = Env.getSpecialDirectory(.UserDocuments)
        Debug.assert(!str.isEmpty())
        return Path.combine(str, "captMe")
    }

    func newFileName()->String
    {
        let t        = Time.DateTime.now()
        let fileName = Format.toString("%_%_%_%_%_%.captme", t.month, t.day, t.year, t.hour, t.minute, t.second)
        return Path.combine(getFolder(), fileName.toString())
    }

    mtd fill()
    {
        .files = catch Directory.enumerateFiles(getFolder(), ".captme")
    }

    mtd removeFile(name: string)
    {
        foreach fi, idx in .files
        {
            if fi.fullname == name
            {
                catch File.delete(name)
                if @err == null
                {
                    .files.removeAtOrdered(idx)
                    .onDeleteFile.call(name)
                }

                break
            }
        }
    }

    mtd addFile(name: string)
    {
        let file = catch File.getFileInfo(name)
        if @err != null do
            return
        .files.add(file)
    }
}
