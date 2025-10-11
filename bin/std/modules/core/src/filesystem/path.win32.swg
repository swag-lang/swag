#global #if WINDOWS
#global public
#global namespace Path
using Win32

// Returns true if the given path name is valid
func isValidPathName(fullname: string)->bool
{
    if @countof(fullname) == 0 do
        return false

    foreach c in fullname
    {
        if c < 32 do
            return false
        if c == '"' do
            return false
        if c == '<' do
            return false
        if c == '>' do
            return false
        if c == '|' do
            return false
        if c == '*' do
            return false
        if c == '?' do
            return false
    }

    return true
}

// Returns true if the given file name is valid
func isValidFileName(fullname: string)->bool
{
    if @countof(fullname) == 0 do
        return false
    foreach c in fullname
    {
        if c < 32 do
            return false
        if c == '"' do
            return false
        if c == '<' do
            return false
        if c == '>' do
            return false
        if c == '|' do
            return false
        if c == ':' do
            return false
        if c == '*' do
            return false
        if c == '?' do
            return false
        if c == '\\' do
            return false
        if c == '/' do
            return false
    }

    return true
}

// Returns the length of the root part of the path
func getRootLength(fullname: string)->u32
{
    if fullname == null do
        return 0

    func startsWithOrdinal(source, value: string)->bool
    {
        let sourceLength = @countof(source)
        let valueLength  = @countof(value)
        if sourceLength < valueLength do
            return false

        for i in valueLength
        {
            if value[i] != source[i] do
                return false
        }

        return true
    }

    var i                     = 0'u32
    var volumeSeparatorLength = 2'u32
    // Length to the colon "C:"
    var uncRootLength = 2'u32
    // Length to the start of the server name "\\"
    let pathLength = @countof(fullname)

    const ExtendedPathPrefix    = #"\\?\"#
    const UncExtendedPathPrefix = #"\\?\UNC\"#

    let extendedSyntax    = startsWithOrdinal(fullname, ExtendedPathPrefix)
    let extendedUncSyntax = startsWithOrdinal(fullname, UncExtendedPathPrefix)
    if extendedSyntax
    {
        // Shift the position we look for the root from to account for the extended prefix
        if extendedUncSyntax
        {
            // "\\" -> "\\?\UNC\"
            uncRootLength = @countof(UncExtendedPathPrefix)
        }
        else
        {
            // "C:" -> "\\?\C:"
            volumeSeparatorLength += @countof(ExtendedPathPrefix)
        }
    }

    if (!extendedSyntax or extendedUncSyntax) and pathLength > 0 and isDirectorySeparator(fullname[0])
    {
        // UNC or simple rooted path (e.g. "\foo", NOT "\\?\C:\foo")

        i = 1
        //  Drive rooted (\foo) is one character
        if (extendedUncSyntax or (pathLength > 1 and isDirectorySeparator(fullname[1])))
        {
            // UNC (\\?\UNC\ or \\), scan past the next two directory separators at most
            // (e.g. to \\?\UNC\Server\Share or \\Server\Share\)
            i = uncRootLength
            var n = 1
            // Maximum separators to skip
            while i < pathLength and (!isDirectorySeparator(fullname[i]) or n >= 0)
            {
                n -= 1
                i += 1
            }
        }
    }
    elif pathLength >= volumeSeparatorLength and fullname[volumeSeparatorLength - 1] == VolumeSeparatorChar
    {
        // Path is at least longer than where we expect a colon, and has a colon (\\?\A:, A:)
        // If the colon is followed by a directory separator, #move past it
        i = volumeSeparatorLength
        if pathLength >= volumeSeparatorLength + 1 and isDirectorySeparator(fullname[volumeSeparatorLength]) do
            i += 1
    }

    return i
}

// Transform 'fullname' to an absolute path
func getAbsolute(fullname: string)->String throw
{
    var result: retval

    var tmpArr: Array'u16
    tmpArr.allocator = @getcontext().tempAllocator
    tmpArr.reserve(100)

    let winPath = Utf16.toZeroTerminated(fullname)
    var res     = cast(u64) try GetFullPathNameW(@dataof(winPath), cast(DWORD) tmpArr.capacity, tmpArr.buffer, null)

    // No enough room in destination, give it another try
    if res >= 100
    {
        tmpArr.reserve(res + 1)
        res = cast(u64) try GetFullPathNameW(@dataof(winPath), cast(DWORD) tmpArr.capacity, tmpArr.buffer, null)
    }

    tmpArr.count = res
    result       = Utf8.fromUtf16(tmpArr.toSlice())
    normalize(&result)
    return result
}
