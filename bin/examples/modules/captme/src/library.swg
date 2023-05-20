var g_Library: Library

struct Library
{
    files: Array'File.FileInfo
}

impl Library
{
    func init() throw
    {
        str := getFolder()
        if !Directory.exists(str)
            Directory.create(str)
    }

    func getFolder()->String
    {
        str := Env.getSpecialDirectory(.UserDocuments)
        Debug.assert(!str.isEmpty())
        return Path.combine(str, "captMe")
    }

    func newFileName()->String
    {
        t := Time.DateTime.now()
        fileName := Format.toString("%_%_%_%_%_%.captme", t.month, t.day, t.year, t.hour, t.minute, t.second)
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
        file := catch File.getFileInfo(name)
        if @err
            return
        files.add(file)
    }
}