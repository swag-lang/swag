#global public
#global namespace Path
using Swag

const DirectorySeparatorChar    = '/''u8      // Default directory separator character
const AltDirectorySeparatorChar = '\\''u8     // Alternate directory separator character
const VolumeSeparatorChar       = ':''u8      // Volume separator character (e.g., 'C:')

// Returns true if the given character is a directory separator
#[Inline, ConstExpr]
func isDirectorySeparator(c: u8)->bool
{
    return c == DirectorySeparatorChar or c == AltDirectorySeparatorChar
}

// Returns the root part of the path, null if not found
#[ConstExpr]
func getRootName(fullname: string)->#null string
{
    if fullname == null do
        return null
    Debug.assert(isValidPathName(fullname))
    let rootLength = getRootLength(fullname)
    return cast() @mkslice(@dataof(fullname), cast(u64) rootLength)
}

// Returns true if the given path is rooted (starts with a separator or a drive letter and colon)
#[ConstExpr]
func isRooted(fullname: string)->bool
{
    if fullname == null do
        return false
    Debug.assert(isValidPathName(fullname))
    let countof = @countof(fullname)
    if countof >= 1 and isDirectorySeparator(fullname[0]) do
        return true
    if countof >= 2 and fullname[1] == VolumeSeparatorChar do
        return true
    return false
}

// Returns the directory part of the path, null if not found
#[ConstExpr]
func getDirectoryName(fullname: string)->#null string
{
    if fullname == null do
        return null
    Debug.assert(isValidPathName(fullname))

    let countof = @countof(fullname)
    var cpt     = countof
    while cpt
    {
        cpt -= 1
        let c = fullname[cpt]
        if isDirectorySeparator(c)
        {
            if cpt == 0 do
                return null
            if cpt == 2 and fullname[1] == VolumeSeparatorChar do
                return null
            return cast() @mkslice(@dataof(fullname), cast(u64) cpt)
        }
    }

    return null
}

// Returns the file name part of the path, null if not found
#[ConstExpr]
func getFileName(fullname: string)->#null string
{
    if fullname == null do
        return null
    Debug.assert(isValidPathName(fullname))

    let countof = @countof(fullname)
    var cpt     = countof
    while cpt
    {
        cpt -= 1
        let c = fullname[cpt]
        if isDirectorySeparator(c) or c == VolumeSeparatorChar
        {
            if cpt == countof - 1 do
                return null
            return cast() @mkslice(@dataof(fullname) + cpt + 1, cast(u64) (countof - cpt) - 1)
        }
    }

    return fullname
}

// Returns the file name without its extension, null if not found
#[ConstExpr]
func getFileNameWithoutExtension(fullname: string)->#null string
{
    if fullname == null do
        return null
    Debug.assert(isValidPathName(fullname))

    let countof = @countof(fullname)
    var cpt     = countof
    var last    = countof
    while cpt
    {
        cpt -= 1
        let c = fullname[cpt]
        if c == '.' and last == countof do
            last = cpt
        elif isDirectorySeparator(c) or c == VolumeSeparatorChar
        {
            if cpt == countof - 1 do
                return null
            return cast() @mkslice(@dataof(fullname) + cpt + 1, cast(u64) (last - cpt) - 1)
        }
    }

    return fullname
}

// Returns the file name extension, including the '.' character
// Returns null if no extension was found
#[ConstExpr]
func getExtension(fullname: string)->#null string
{
    if fullname == null do
        return null
    Debug.assert(isValidPathName(fullname))

    var cpt = @countof(fullname)
    while cpt
    {
        cpt -= 1
        let c = fullname[cpt]
        if c == '.' do
            return cast() @mkslice(@dataof(fullname) + cpt, cast(u64) @countof(fullname) - cpt)
        if isDirectorySeparator(c) or c == VolumeSeparatorChar do
            break
    }

    return null
}

// Returns the file name extension in lower case, including the '.' character
// Returns null if no extension was found
func getExtensionLowerCase(fullname: string)->String
{
    return String.toLower(getExtension(fullname))
}

// Returns true if the given file has the specified extension (case-insensitive)
func isExtension(fullname, extension: string)->bool
{
    return getExtensionLowerCase(fullname) == String.toLower(extension)
}

// Returns true if the file name contains an extension
#[ConstExpr]
func hasExtension(fullname: string)->bool
{
    if fullname == null do
        return false
    Debug.assert(isValidPathName(fullname))

    var cpt = @countof(fullname)
    while cpt
    {
        cpt -= 1
        let c = fullname[cpt]
        if c == '.' do
            return true
        if isDirectorySeparator(c) or c == VolumeSeparatorChar do
            break
    }

    return false
}

// Combines multiple path segments into one, adding a directory separator between them if necessary
func combine(fullnames: string...)->String
{
    var result: retval
    foreach one in fullnames
    {
        if @countof(one) == 0 do
            continue

        if result.length
        {
            let c = result.back()
            if !isDirectorySeparator(c) do
                result += DirectorySeparatorChar
        }

        result += one
    }

    return result
}

// Returns true if the two paths are equal (case-insensitive Latin1 comparison)
func equals(path1, path2: string)->bool
{
    return Utf8.compare(path1, path2, .Latin1NoCase) == 0
}

// Normalizes the path by replacing '\' with '/' to conform to standard path format
func normalize(fullname: *String)
{
    fullname.replace("\\", "/")
}
