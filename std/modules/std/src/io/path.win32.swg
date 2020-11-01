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