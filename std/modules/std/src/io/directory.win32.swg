#if std.WINDOWS;
using win32
public namespace std.directory;

// Returns true if the given directory exists
func exists(fullname: string)->bool
{
    if fullname == null return false
    attributes := GetFileAttributesA(@dataof(fullname))
    if attributes == 0xFFFFFFFF return false
    if !(attributes & FILE_ATTRIBUTE_DIRECTORY) return false
    return true
}
