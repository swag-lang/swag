#global public
#global namespace Path
using Swag

const DirectorySeparatorChar    = `/`'u8
const AltDirectorySeparatorChar = `\\`'u8
const VolumeSeparatorChar       = `:`'u8

// Returns true if the given character is a directory separator
#[Inline, ConstExpr]
func isDirectorySeparator(c: u8)->bool
{
    return c == DirectorySeparatorChar or c == AltDirectorySeparatorChar
}

// Returns the root part of the path, null if not found
#[ConstExpr]
func getRootName(fullname: string)->string
{
    if fullname == null
        return null
    Debug.assert(isValidPathName(fullname))
    let rootLength = getRootLength(fullname)
    return acast @mkslice(@dataof(fullname), cast(u64) rootLength)
}

// Tests if the given path contains a root. A path is considered rooted
// if it starts with a directory separator or a drive letter and a colon.
#[ConstExpr]
func isRooted(fullname: string)->bool
{
    if fullname == null
        return false
    Debug.assert(isValidPathName(fullname))
    let countof = @countof(fullname)
    if countof >= 1 and isDirectorySeparator(fullname[0])
        return true
    if countof >= 2 and fullname[1] == VolumeSeparatorChar
        return true
    return false
}

// Returns the directory part of the path, null if not found
#[ConstExpr]
func getDirectoryName(fullname: string)->string
{
    if fullname == null
        return null
    Debug.assert(isValidPathName(fullname))

    let countof = @countof(fullname)
    var cpt = countof
    while cpt
    {
        cpt -= 1
        let c = fullname[cpt]
        if isDirectorySeparator(c)
        {
            if cpt == 0
                return null
            if cpt == 2 and fullname[1] == VolumeSeparatorChar
                return null
            return acast @mkslice(@dataof(fullname), cast(u64) cpt)
        }
    }

    return null
}

// Returns the file name part of the path, null if not found
#[ConstExpr]
func getFileName(fullname: string)->string
{
    if fullname == null
        return null
    Debug.assert(isValidPathName(fullname))

    let countof = @countof(fullname)
    var cpt = countof
    while cpt
    {
        cpt -= 1
        let c = fullname[cpt]
        if isDirectorySeparator(c) or c == VolumeSeparatorChar
        {
            if cpt == countof - 1 return null
            return acast @mkslice(@dataof(fullname) + cpt + 1, cast(u64) (countof - cpt) - 1)
        }
    }

    return fullname
}

// Returns the path file name, null if not found
#[ConstExpr]
func getFileNameWithoutExtension(fullname: string)->string
{
    if fullname == null
        return null
    Debug.assert(isValidPathName(fullname))

    let countof = @countof(fullname)
    var cpt = countof
    var last = countof
    while cpt
    {
        cpt -= 1
        let c = fullname[cpt]
        if c == `.` and last == countof
            last = cpt
        elif isDirectorySeparator(c) or c == VolumeSeparatorChar
        {
            if cpt == countof - 1 return null
            return acast @mkslice(@dataof(fullname) + cpt + 1, cast(u64) (last - cpt) - 1)
        }
    }

    return fullname
}

// Returns the file name extension, including the '.' character
// Returns null if no extension was found
#[ConstExpr]
func getExtension(fullname: string)->string
{
    if fullname == null
        return null
    Debug.assert(isValidPathName(fullname))

    var cpt = @countof(fullname)
    while cpt
    {
        cpt -= 1
        let c = fullname[cpt]
        if c == `.`
            return acast @mkslice(@dataof(fullname) + cpt, cast(u64) @countof(fullname) - cpt)
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

// Returns true if the given file is of the given extension
func isExtension(fullname, extension: string)->bool
{
    return getExtensionLowerCase(fullname) == String.toLower(extension)
}

// Returns true if the file name contains an extension
#[ConstExpr]
func hasExtension(fullname: string)->bool
{
    if fullname == null
        return false
    Debug.assert(isValidPathName(fullname))

    var cpt = @countof(fullname)
    while cpt
    {
        cpt -= 1
        let c = fullname[cpt]
        if c == `.`
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
            let c = result.back()
            if !isDirectorySeparator(c)
                result += DirectorySeparatorChar
        }

        result += one
    }

    return result
}

// Returns true if the two paths are equal
func equals(path1, path2: string)->bool
{
    return Utf8.compare(path1, path2, .Latin1NoCase) == 0
}

// Normalize path
// '\' are replaced with '/' as this is the std norm for paths
func normalize(fullname: *String)
{
    fullname.replace("\\", "/")
}