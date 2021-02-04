#global if WINDOWS
#global public
#global namespace directory
using win32

// Returns true if the given directory exists
func exists(dirname: string)->bool
{
    if dirname == null return false
    attributes := GetFileAttributesA(@dataof(dirname))
    if attributes == 0xFFFFFFFF return false
    if !(attributes & FILE_ATTRIBUTE_DIRECTORY) return false
    return true
}

// Enumerate a directory
func enumerate(dirname: string, options: EnumerationOptions)->Array'(file.FileInfo)
{
    using file

    var result: retval
    if dirname == null return result
    if options.matchPattern == null return result

    var findfile:   WIN32_FIND_DATAA
    var fi:         FileInfo
    var fileTime:   SYSTEMTIME
    var stackPath:  Array'String

    dataof := cast(const *u8) @dataof(findfile.cFileName)

    stackPath.add(String.from(dirname))
    while !stackPath.isEmpty()
    {
        toScan := stackPath.popBack()
        searchPath := path.combine(toScan.toString(), options.matchPattern)

        h := FindFirstFileA(searchPath.buffer, &findfile);
        if h == INVALID_HANDLE_VALUE
            continue

        while true
        {
            filename := @mkstring(dataof, @cstrlen(dataof))

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
                else
                    mustAdd = options.wantSpecialDirectories
            }
            else
                mustAdd = options.wantFiles

            // Add file/folder to the result
            if mustAdd
            {
                fi.attributes = FileAttributes.fromDWORD(findfile.dwFileAttributes)
                if !(fi.attributes & options.skipAttributes)
                {
                    fi.fullname = path.combine(toScan, filename)

                    fi.size = findfile.nFileSizeLow + (cast(u64) findfile.nFileSizeHigh << 32)
                    FileTimeToSystemTime(&findfile.ftCreationTime, &fileTime)
                    fi.creationTime.setSYSTEMTIME(fileTime)
                    FileTimeToSystemTime(&findfile.ftLastAccessTime, &fileTime)
                    fi.lastAccessTime.setSYSTEMTIME(fileTime)
                    FileTimeToSystemTime(&findfile.ftLastWriteTime, &fileTime)
                    fi.lastWriteTime.setSYSTEMTIME(fileTime)

                    result.emplace(&fi)
                }
            }

            if !FindNextFileA(h, &findfile)
                break
        }

        FindClose(h)
    }

    return result
}

// Returns the current directory
func getCurrent()->String
{
    var result: retval
    length := GetCurrentDirectoryA(0, null)
    result.reserve(cast(uint) length + 1)
    result.length = GetCurrentDirectoryA(length + 1, result.buffer)
    path.normalize(result)
    return result
}

// Set the current directory
func setCurrent(dirname: string)->ErrorId
{
    if !SetCurrentDirectoryA(@dataof(dirname))
    {
        error.setLastErrorId()
        return error.last()
    }

    return null
}