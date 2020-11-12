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

func enumerateFiles(dirname: string, searchPattern: string = "*")->Array'(file.FileInfo)
{
    var options: EnumerationOptions
    options.searchPattern = searchPattern
    options.wantDirectories = false
    return enumerate(dirname, options)
}

func enumerate(dirname: string, options: EnumerationOptions)->Array'(file.FileInfo)
{
    var result: retval
    if dirname == null return result
    if options.searchPattern == null return result

    var findfile:   WIN32_FIND_DATAA
    var fi:         file.FileInfo
    var fileTime:   SYSTEMTIME
    var stackPath:  Array'String

    dataof := cast(const *u8) @dataof(findfile.cFileName)

    stackPath.add(String.from(dirname))
    while !stackPath.isEmpty()
    {
        toScan := stackPath.popBack()
        searchPath := path.combine(toScan.toString(), options.searchPattern)

        h := FindFirstFileA(searchPath.buffer, &findfile);
        if h != INVALID_HANDLE_VALUE
        {
            while true
            {
                filename := cast(string) @mkslice(dataof, @cstrlen(dataof))

                // Filter by file/folder
                mustAdd := false
                if findfile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY
                {
                    if filename != "." && filename != ".."
                    {
                        mustAdd = options.wantDirectories
                        if options.recurse
                        {
                            newPath := path.combine(toScan.toString(), filename)
                            stackPath.emplace(&newPath)
                        }
                    }
                }
                else
                    mustAdd = options.wantFiles

                // Add file/folder to the result
                if mustAdd
                {
                    fi.fullname = path.combine(toScan.toString(), filename)

                    fi.attributes = file.FileAttribute.fromDWORD(findfile.dwFileAttributes)
                    fi.size = findfile.nFileSizeLow + (cast(u64) findfile.nFileSizeHigh << 32)
                    FileTimeToSystemTime(&findfile.ftCreationTime, &fileTime)
                    fi.creationTime.fromSYSTEMTIME(fileTime)
                    FileTimeToSystemTime(&findfile.ftLastAccessTime, &fileTime)
                    fi.lastAccessTime.fromSYSTEMTIME(fileTime)
                    FileTimeToSystemTime(&findfile.ftLastWriteTime, &fileTime)
                    fi.lastWriteTime.fromSYSTEMTIME(fileTime)

                    result.emplace(&fi)
                }

                if !FindNextFileA(h, &findfile)
                    break
            }

            FindClose(h)
        }
    }

    return result
}