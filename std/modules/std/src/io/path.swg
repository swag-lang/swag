public namespace std.path;

const DirectorySeparatorChar    = "\\"'u8
const AltDirectorySeparatorChar = "/"'u8
const VolumeSeparatorChar       = ":"'u8

// Returns the path file name, null if not found
func getFileName(fullname: string)->string
{
    if(fullname == null) return null
    debug.assert(isValidPathName(fullname))

    countof := @countof(fullname)
    cpt := countof
    while cpt
    {
        cpt -= 1
        c := fullname[cpt]
        if c == DirectorySeparatorChar || c == AltDirectorySeparatorChar || c == VolumeSeparatorChar
        {
            if cpt == countof - 1 return null
            return acast @mkslice(@dataof(fullname) + cpt + 1, (countof - cpt) - 1)
        }
    }

    return fullname
}

// Returns the file name extension, including the '.' character
// Returns null if no extension was found
func getExtension(fullname: string)->string
{
    if(fullname == null) return null
    debug.assert(isValidPathName(fullname))

    cpt := @countof(fullname)
    while cpt
    {
        cpt -= 1
        c := fullname[cpt]
        if c == "."'u8
            return acast @mkslice(@dataof(fullname) + cpt, @countof(fullname) - cpt)
        if c == DirectorySeparatorChar || c == AltDirectorySeparatorChar || c == VolumeSeparatorChar
            break
    }

    return null
}

// Returns true if the file name contains an extension
func hasExtension(fullname: string)->bool
{
    if(fullname == null) return false
    debug.assert(isValidPathName(fullname))

    cpt := @countof(fullname)
    while cpt
    {
        cpt -= 1
        c := fullname[cpt]
        if c == "."'u8
            return true
        if c == DirectorySeparatorChar || c == AltDirectorySeparatorChar || c == VolumeSeparatorChar
            break
    }

    return false
}