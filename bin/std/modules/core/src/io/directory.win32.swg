#global if WINDOWS
#global public
#global namespace Directory
using Win32

func create(dirname: string) throw
{
    winPath := Utf16.toZeroTerminated(dirname)
    CreateDirectoryW(@dataof(winPath), null)
}

// Returns true if the given directory exists
func exists(dirName: string)->bool
{
    if @countof(dirName) == 0
        return false
    winPath := Utf16.toZeroTerminated(dirName)
    attributes := catch GetFileAttributesW(@dataof(winPath))
    if @errmsg
        return false
    if attributes == 0xFFFFFFFF
        return false
    if !(attributes & FILE_ATTRIBUTE_DIRECTORY)
        return false
    return true
}

// Enumerate a directory
func enumerate(dirName: string, options: EnumerationOptions)->Array'(File.FileInfo) throw
{
    using File
    var result: retval
    if dirName == null
        return result

    var findfile:   WIN32_FIND_DATAW
    var fi:         FileInfo
    var fileTime:   SYSTEMTIME
    var stackPath:  Array'String

    dataof := cast(const ^u16) @dataof(findfile.cFileName)

    stackPath.add(String.from(dirName))
    while !stackPath.isEmpty()
    {
        toScan := stackPath.popBack()
        if toScan.back() == Path.VolumeSeparatorChar
            toScan += Path.DirectorySeparatorChar
        searchPath := Path.combine(toScan.toString(), "*")

        winPath := Utf16.toZeroTerminated(searchPath)
        h := try FindFirstFileW(@dataof(winPath), &findfile)
        if h == INVALID_HANDLE_VALUE
            continue

        loop
        {
            filename := Utf8.fromUtf16(@mkslice(dataof, Utf16.lengthZeroTerminated(dataof)))

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
                        stackPath.add(moveref &newPath)
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

                    if !options.matchExtension or Path.isExtension(fi.fullname, options.matchExtension)
                    {
                        if !options.filterLambda or options.filterLambda(fi)
                        {
                            result.add(moveref &fi)
                        }
                    }
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

    arr16.reserve(cast(u64) length + 1)
    arr16.count = try GetCurrentDirectoryW(length + 1, @dataof(arr16))

    result = Utf8.fromUtf16(arr16.toSlice())
    Path.normalize(&result)
    return result
}

// Set the current directory
func setCurrent(dirname: string) throw
{
    winPath := Utf16.toZeroTerminated(dirname)
    try SetCurrentDirectoryW(@dataof(winPath))
}

// Get the list of logical drives
func getDrives()->Array'String throw
{
    var result: retval

    var volName: [MAX_PATH] u8
    var driveName: [MAX_PATH] u8

    h := FindFirstVolumeA(volName, acast MAX_PATH)
    if h == INVALID_HANDLE_VALUE
        return result

    loop
    {
        var ret: DWORD
        catch GetVolumePathNamesForVolumeNameA(volName, driveName, acast MAX_PATH, &ret)
        if !@errmsg
        {
            len := @strlen(driveName)
            if len
            {
                str := String.from(@mkstring(&driveName[0], len))
                Path.normalize(&str)
                result.add(str)
            }
        }

        if !FindNextVolumeA(h, volName, acast MAX_PATH)
            break
    }

    FindVolumeClose(h)
    return result
}