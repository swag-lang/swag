struct Library
{
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
        folder := getFolder()
        return Path.combine(folder, fileName.toString())
    }
}