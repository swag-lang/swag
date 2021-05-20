#global if WINDOWS
#global public
#global namespace Directory
using win32

// Returns true if the given directory exists
func exists(dirname: string)->bool
{
    if dirname == null return false
    winPath := utf16.toZeroTerminated(dirname)
    attributes := catch GetFileAttributesW(@dataof(winPath))
    if @err return false
    if attributes == 0xFFFFFFFF return false
    if !(attributes & FILE_ATTRIBUTE_DIRECTORY) return false
    return true
}

// Enumerate a directory
func enumerate(dirname: string, options: EnumerationOptions)->Array'(File.FileInfo) throw
{
    using File

    var result: retval
    if dirname == null return result
    if options.matchPattern == null return result

    var findfile:   WIN32_FIND_DATAW
    var fi:         FileInfo
    var fileTime:   SYSTEMTIME
    var stackPath:  Array'String

    dataof := cast(const *u16) @dataof(findfile.cFileName)

    stackPath.add(String.from(dirname))
    while !stackPath.isEmpty()
    {
        toScan := stackPath.popBack()
        searchPath := Path.combine(toScan.toString(), options.matchPattern)

        winPath := utf16.toZeroTerminated(searchPath)
        h := try FindFirstFileW(@dataof(winPath), &findfile)
        if h == INVALID_HANDLE_VALUE
            continue

        loop
        {
            filename := Utf8.fromUtf16(@mkslice(dataof, utf16.lengthZeroTerminated(dataof)))

            // Filter by file/folder
            mustAdd := false
            if findfile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY
            {
                if filename != "." and filename != ".."
                {
                    mustAdd = options.wantDirectories
                    if options.recurse
                    {
                        newPath := Path.combine(toScan.toString(), filename.toString())
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
                    fi.fullname = Path.combine(toScan, filename.toString())

                    fi.size = findfile.nFileSizeLow + (cast(u64) findfile.nFileSizeHigh << 32)
                    catch FileTimeToSystemTime(&findfile.ftCreationTime, &fileTime)
                    fi.creationTime.setSYSTEMTIME(fileTime)
                    catch FileTimeToSystemTime(&findfile.ftLastAccessTime, &fileTime)
                    fi.lastAccessTime.setSYSTEMTIME(fileTime)
                    catch FileTimeToSystemTime(&findfile.ftLastWriteTime, &fileTime)
                    fi.lastWriteTime.setSYSTEMTIME(fileTime)

                    result.emplace(&fi)
                }
            }

            next := catch FindNextFileW(h, &findfile)
            if !next break
        }

        assume FindClose(h)
    }

    return result
}

// Returns the current directory
func getCurrent()->String throw
{
    var result: retval
    length := try GetCurrentDirectoryW(0, null)

    var arr16: Array'u16
    arr16.allocator = @getcontext().tempAllocator

    arr16.reserve(cast(uint) length + 1)
    arr16.count = try GetCurrentDirectoryW(length + 1, @dataof(arr16))

    result = Utf8.fromUtf16(arr16.toSlice())
    Path.normalize(&result)
    return result
}

// Set the current directory
func setCurrent(dirname: string) throw
{
    winPath := utf16.toZeroTerminated(dirname)
    try SetCurrentDirectoryW(@dataof(winPath))
}