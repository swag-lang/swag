#if std.WINDOWS;
using win32
public namespace std.path;

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

    const ExtendedPathPrefix = #"\\?\"#
    const UncExtendedPathPrefix = #"\\?\UNC\"#

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