public namespace std.path;
using swag

const DirectorySeparatorChar    = "\\"'u8
const AltDirectorySeparatorChar = "/"'u8
const VolumeSeparatorChar       = ":"'u8

// Returns true if the given character is a directory separator
#[inline, constexpr]
func isDirectorySeparator(c: u8)->bool
{
    return c == DirectorySeparatorChar || c == AltDirectorySeparatorChar
}

// Returns the file name part of the path, null if not found
#[constexpr]
func getFileName(fullname: string)->string
{
    if fullname == null return null
    debug.assert(isValidPathName(fullname))

    countof := @countof(fullname)
    cpt := countof
    while cpt
    {
        cpt -= 1
        c := fullname[cpt]
        if isDirectorySeparator(c) || c == VolumeSeparatorChar
        {
            if cpt == countof - 1 return null
            return autocast @mkslice(@dataof(fullname) + cpt + 1, (countof - cpt) - 1)
        }
    }

    return fullname
}

// Returns the directory part of the path, null if not found
#[constexpr]
func getRootName(fullname: string)->string
{
    if fullname == null return null
    debug.assert(isValidPathName(fullname))
    rootLength := getRootLength(fullname)
    return autocast @mkslice(@dataof(fullname), rootLength)
}

// Tests if the given path contains a root. A path is considered rooted
// if it starts with a directory separatpr or a drive letter and a colon.
#[constexpr]
func isRooted(fullname: string)->bool
{
    if fullname == null return false
    debug.assert(isValidPathName(fullname))
    countof := @countof(fullname)
    if countof >= 1 && isDirectorySeparator(fullname[0])
        return true
    if countof >= 2 && fullname[1] == VolumeSeparatorChar
        return true
    return false
}

// Returns the directory part of the path, null if not found
#[constexpr]
func getDirectoryName(fullname: string)->string
{
    if fullname == null return null
    debug.assert(isValidPathName(fullname))

    countof := @countof(fullname)
    cpt := countof
    while cpt
    {
        cpt -= 1
        c := fullname[cpt]
        if isDirectorySeparator(c)
            return autocast @mkslice(@dataof(fullname), cpt)
    }

    return null
}

// Returns the path file name, null if not found
#[constexpr]
func getFileNameWithoutExtension(fullname: string)->string
{
    if(fullname == null) return null
    debug.assert(isValidPathName(fullname))

    countof := @countof(fullname)
    cpt := countof
    last := countof
    while cpt
    {
        cpt -= 1
        c := fullname[cpt]
        if c == "."'u8 && last == countof
            last = cpt
        else if isDirectorySeparator(c) || c == VolumeSeparatorChar
        {
            if cpt == countof - 1 return null
            return autocast @mkslice(@dataof(fullname) + cpt + 1, (last - cpt) - 1)
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
    debug.assert(isValidPathName(fullname))

    cpt := @countof(fullname)
    while cpt
    {
        cpt -= 1
        c := fullname[cpt]
        if c == "."'u8
            return autocast @mkslice(@dataof(fullname) + cpt, @countof(fullname) - cpt)
        if isDirectorySeparator(c) || c == VolumeSeparatorChar
            break
    }

    return null
}

// Returns true if the file name contains an extension
#[constexpr]
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
        if isDirectorySeparator(c) || c == VolumeSeparatorChar
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
            if !isDirectorySeparator(c) && c != VolumeSeparatorChar
                result += AltDirectorySeparatorChar
        }

        result += one
    }

    return result
}