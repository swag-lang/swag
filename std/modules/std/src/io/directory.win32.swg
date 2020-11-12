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
    var searchPath: String = path.combine(dirname, searchPattern)
    var fi:         file.FileInfo

    dataof := cast(const *u8) @dataof(findfile.cFileName)

    h := FindFirstFileA(searchPath.buffer, &findfile);
    if h != INVALID_HANDLE_VALUE
    {
        while true
        {
            if !(findfile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                filename := cast(string) @mkslice(dataof, @cstrlen(dataof))
                fi.fullname = path.combine(dirname, filename)
                result.emplace(&fi)
            }

            if !FindNextFileA(h, &findfile)
                break
        }

        FindClose(h)
    }

    return result
}