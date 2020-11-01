public namespace std.path;

const DirectorySeparatorChar    = "\\"'u8
const AltDirectorySeparatorChar = "/"'u8
const VolumeSeparatorChar       = ":"'u8

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