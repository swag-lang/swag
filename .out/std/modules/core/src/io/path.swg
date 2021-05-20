#global public
#global namespace Path
using Swag

const DirectorySeparatorChar    = "/"'u8
const AltDirectorySeparatorChar = "\\"'u8
const VolumeSeparatorChar       = ":"'u8

// Returns true if the given character is a directory separator
#[inline, constexpr]
func isDirectorySeparator(c: u8)->bool
{
    return c == DirectorySeparatorChar or c == AltDirectorySeparatorChar
}

// Returns the file name part of the path, null if not found
#[constexpr]
func getFileName(fullname: string)->string
{
    if fullname == null return null
    Debug.assert(isValidPathName(fullname))

    countof := @countof(fullname)
    cpt := countof
    while cpt
    {
        cpt -= 1
        c := fullname[cpt]
        if isDirectorySeparator(c) or c == VolumeSeparatorChar
        {
            if cpt == countof - 1 return null
            return acast @mkslice(@dataof(fullname) + cpt + 1, cast(uint) (countof - cpt) - 1)
        }
    }

    return fullname
}

// Returns the directory part of the path, null if not found
#[constexpr]
func getRootName(fullname: string)->string
{
    if fullname == null return null
    Debug.assert(isValidPathName(fullname))
    rootLength := getRootLength(fullname)
    return acast @mkslice(@dataof(fullname), cast(uint) rootLength)
}

// Tests if the given path contains a root. A path is considered rooted
// if it starts with a directory separatpr or a drive letter and a colon.
#[constexpr]
func isRooted(fullname: string)->bool
{
    if fullname == null return false
    Debug.assert(isValidPathName(fullname))
    countof := @countof(fullname)
    if countof >= 1 and isDirectorySeparator(fullname[0])
        return true
    if countof >= 2 and fullname[1] == VolumeSeparatorChar
        return true
    return false
}

// Returns the directory part of the path, null if not found
#[constexpr]
func getDirectoryName(fullname: string)->string
{
    if fullname == null return null
    Debug.assert(isValidPathName(fullname))

    countof := @countof(fullname)
    cpt := countof
    while cpt
    {
        cpt -= 1
        c := fullname[cpt]
        if isDirectorySeparator(c)
            return acast @mkslice(@dataof(fullname), cast(uint) cpt)
    }

    return null
}

// Returns the path file name, null if not found
#[constexpr]
func getFileNameWithoutExtension(fullname: string)->string
{
    if(fullname == null) return null
    Debug.assert(isValidPathName(fullname))

    countof := @countof(fullname)
    cpt := countof
    last := countof
    while cpt
    {
        cpt -= 1
        c := fullname[cpt]
        if c == "."'u8 and last == countof
            last = cpt
        else if isDirectorySeparator(c) or c == VolumeSeparatorChar
        {
            if cpt == countof - 1 return null
            return acast @mkslice(@dataof(fullname) + cpt + 1, cast(uint) (last - cpt) - 1)
        }
    }

    return fullname
}

// Returns the file name extension, including the '.' character
// Returns null if no extension was found
#[constexpr]
func getExtension(fullname: string)->string
{
    if(fullname == null) return null
    Debug.assert(isValidPathName(fullname))

    cpt := @countof(fullname)
    while cpt
    {
        cpt -= 1
        c := fullname[cpt]
        if c == "."'u8
            return acast @mkslice(@dataof(fullname) + cpt, cast(uint) @countof(fullname) - cpt)
        if isDirectorySeparator(c) or c == VolumeSeparatorChar
            break
    }

    return null
}

// Returns the file name extension, including the '.' character, in lower case
// Returns null if no extension was found
func getExtensionLowerCase(fullname: string)->String
{
    return String.toLower(getExtension(fullname))
}

// Returns true if the file name contains an extension
#[constexpr]
func hasExtension(fullname: string)->bool
{
    if(fullname == null) return false
    Debug.assert(isValidPathName(fullname))

    cpt := @countof(fullname)
    while cpt
    {
        cpt -= 1
        c := fullname[cpt]
        if c == "."'u8
            return true
        if isDirectorySeparator(c) or c == VolumeSeparatorChar
            break
    }

    return false
}

// Combine multiple paths into one, by adding a directory separator
// between them if necessary
func combine(fullnames: string...)->String
{
    var result: retval
    visit one: fullnames
    {
        if @countof(one) == 0
            continue

        if result.length
        {
            c := result[result.length - 1]
            if !isDirectorySeparator(c) and c != VolumeSeparatorChar
                result += DirectorySeparatorChar
        }

        result += one
    }

    return result
}

// Normalize path
// '\' are replaced with '/' as this is the std norm for paths
func normalize(fullname: *String)
{
    fullname.replace("\\", "/")
}