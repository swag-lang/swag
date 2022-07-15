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
}