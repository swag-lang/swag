#if std.WINDOWS;
using win32
public namespace std.directory;

// Returns true if the given directory exists
func exists(dirname: string)->bool
{
    if dirname == null return false
    attributes := GetFileAttributesA(@dataof(dirname))
    if attributes == 0xFFFFFFFF return false
    if !(attributes & FILE_ATTRIBUTE_DIRECTORY) return false
    return true
}

func enumerateFiles(dirname: string, searchPattern: string = "*.*")->Array'(file.FileInfo)
{
    var result: retval
    if dirname == null return result
    if searchPattern == null return result

    var findfile:   WIN32_FIND_DATAA
    var searchPath: String = dirname
    return result
}