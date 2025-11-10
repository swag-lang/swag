struct Icon {}

func getFileDirectoryIcon(name: string, isDirectory: bool, small = true)->Icon
{
    var result: retval
    if !@countof(name) do
        return result

    var ext = getExtension(name)

    var extractFromFile = false
    if isDirectory and name[@countof(name) - 1] == '/' do
        ext = "c:"
    elif isDirectory do
        ext = "c:/temp"
    elif ext == ".exe" or ext == ".dll"
    {
        extractFromFile = true
        ext             = name
    }
    elif ext == null do
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
    if str == null do
        return null
    return ""
}

#[Swag.NoInline]
func(K) find(x: K)->bool
{
    return false
}
