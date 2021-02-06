#global if WINDOWS
#global public
#global namespace path
using win32

// Returns true if the given path name is valid
func isValidPathName(fullname: string)->bool
{
    visit c: fullname
    {
        if(c <= 32)       return false
        if(c == "\""'u8)  return false
        if(c == "<"'u8)   return false
        if(c == ">"'u8)   return false
        if(c == "|"'u8)   return false
        if(c == "*"'u8)   return false
        if(c == "?"'u8)   return false
    }

    return true
}

// Returns true if the given file name is valid
func isValidFileName(fullname: string)->bool
{
    visit c: fullname
    {
        if(c <= 32)       return false
        if(c == "\""'u8)  return false
        if(c == "<"'u8)   return false
        if(c == ">"'u8)   return false
        if(c == "|"'u8)   return false
        if(c == "*"'u8)   return false
        if(c == "?"'u8)   return false
        if(c == ":"'u8)   return false
        if(c == "/"'u8)   return false
        if(c == "\\"'u8)  return false
    }

    return true
}

// Returns the length of the root part of the path
func getRootLength(fullname: string)->u32
{
    func startsWithOrdinal(source, value: string)->bool
    {
        sourceLength := @countof(source)
        valueLength := @countof(value)
        if sourceLength < valueLength
            return false

        loop i: valueLength
        {
            if value[i] != source[i]
                return false;
        }

        return true;
    }

    i := 0'u32;
    volumeSeparatorLength := 2'u32;  // Length to the colon "C:"
    uncRootLength := 2'u32;          // Length to the start of the server name "\\"
    pathLength := @countof(fullname)

    const ExtendedPathPrefix = @"\\?\"@
    const UncExtendedPathPrefix = @"\\?\UNC\"@

    extendedSyntax := startsWithOrdinal(fullname, ExtendedPathPrefix);
    extendedUncSyntax := startsWithOrdinal(fullname, UncExtendedPathPrefix);
    if extendedSyntax
    {
        // Shift the position we look for the root from to account for the extended prefix
        if extendedUncSyntax
        {
            // "\\" -> "\\?\UNC\"
            uncRootLength = @countof(UncExtendedPathPrefix);
        }
        else
        {
            // "C:" -> "\\?\C:"
            volumeSeparatorLength += @countof(ExtendedPathPrefix);
        }
    }

    if (!extendedSyntax || extendedUncSyntax) && pathLength > 0 && isDirectorySeparator(fullname[0])
    {
        // UNC or simple rooted path (e.g. "\foo", NOT "\\?\C:\foo")

        i = 1; //  Drive rooted (\foo) is one character
        if (extendedUncSyntax || (pathLength > 1 && isDirectorySeparator(fullname[1])))
        {
            // UNC (\\?\UNC\ or \\), scan past the next two directory separators at most
            // (e.g. to \\?\UNC\Server\Share or \\Server\Share\)
            i = uncRootLength
            n := 1; // Maximum separators to skip
            while i < pathLength && (!isDirectorySeparator(fullname[i]) || n >= 0)
            {
                n -= 1
                i += 1
            }
        }
    }
    else if pathLength >= volumeSeparatorLength && fullname[volumeSeparatorLength - 1] == VolumeSeparatorChar
    {
        // Path is at least longer than where we expect a colon, and has a colon (\\?\A:, A:)
        // If the colon is followed by a directory separator, move past it
        i = volumeSeparatorLength;
        if pathLength >= volumeSeparatorLength + 1 && isDirectorySeparator(fullname[volumeSeparatorLength])
            i += 1;
    }

    return i
}

// Transform 'fullname' to an absolute path
func getAbsolute(fullname: string) -> {name: String, err: ErrorId }
{
    var result: retval

    result.name.reserve(100)
    res := cast(uint) GetFullPathNameA(@dataof(fullname), cast(DWORD) result.name.capacity, result.name.buffer, null)
    if res == 0
    {
        error.setLastErrorId()
        result.err = error.last()
        return result
    }

    if(res >= 100)
    {
        result.name.reserve(res + 1)
        res = cast(uint) GetFullPathNameA(@dataof(fullname), cast(DWORD) result.name.capacity, result.name.buffer, null)
        if res == 0
        {
            error.setLastErrorId()
            result.err = error.last()
            return result
        }
    }

    result.name.length = res
    normalize(result.name)
    return result
}

// Convert path a unicode 16, for windows functions in the 'W' form
protected func toWin16(src: string)->Array'u16
{
    var result: retval

    result.allocator = @getcontext().tempAllocator
    result.reserve(2 * @countof(src) + 1)
    result.count = utf16.fromUtf8(@mkslice(result.buffer, result.capacity), src) + 1
    result[result.count - 1] = 0

    return result
}