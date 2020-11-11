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

func strlen(value: const *u8)->u32
{
    idx := 0'u32
    while value[idx] idx += 1
    return idx
}

func enumerateFiles(dirname: string, searchPattern: string = "*.*")->Array'(file.FileInfo)
{
    var result: retval
    if dirname == null return result
    if searchPattern == null return result

    var findfile:   WIN32_FIND_DATAA
    var searchPath: String = path.combine(dirname, searchPattern)

    h := FindFirstFileA(searchPath.buffer, &findfile);
    if h != INVALID_HANDLE_VALUE
    {
        while true
        {
            if findfile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY
                continue

            var fi: file.FileInfo

            dataof := cast(const *u8) @dataof(findfile.cFileName)
            fi.fullname = cast(string) @mkslice(dataof, strlen(dataof))
            result.add(fi)

            if !FindNextFileA(h, &findfile)
                break
        }

        FindClose(h)
    }

    return result
}