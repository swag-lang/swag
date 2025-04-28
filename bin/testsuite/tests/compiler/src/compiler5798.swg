struct Icon {}

func getFileDirectoryIcon(name: string, isDirectory: bool, small = true)->Icon
{
    var result: retval
    if !@countof(name):
        return result

    var ext = getExtension(name)

    var extractFromFile = false
    if isDirectory and name[@countof(name) - 1] == '/':
        ext = "c:"
    elif isDirectory:
        ext = "c:/temp"
    elif ext == ".exe" or ext == ".dll"
    {
        extractFromFile = true
        ext             = name
    }
    elif ext == null:
        ext = ".txt"

    if small
    {
        let here = find(ext)
    }

    return result
}

#[Swag.NoInline]
func getExtension(str: string)->#null string
{
    if str == null: return null
    return ""
}

#[Swag.NoInline]
func(K) find(x: K)->bool
{
    return false
}