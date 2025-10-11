#global #if WINDOWS
#global public
#global namespace Directory
using Win32

func create(dirname: string) throw
{
    let winPath = Utf16.toZeroTerminated(dirname)
    CreateDirectoryW(@dataof(winPath), null)
}

// Returns true if the given directory exists
func exists(dirName: string)->bool
{
    if @countof(dirName) == 0 do
        return false
    let winPath    = Utf16.toZeroTerminated(dirName)
    let attributes = catch GetFileAttributesW(@dataof(winPath))
    if @err != null do
        return false
    if attributes == 0xFFFFFFFF do
        return false
    if !(attributes & FILE_ATTRIBUTE_DIRECTORY) do
        return false
    return true
}

// Enumerate a directory
func enumerate(dirName: string, options: EnumerationOptions)->Array'(File.FileInfo) throw
{
    using File
    var result: retval
    if dirName == null do
        return result

    var findfile:  WIN32_FIND_DATAW
    var fi:        FileInfo
    var fileTime:  SYSTEMTIME
    var stackPath: Array'String

    let dataof = cast(const [*] u16) @dataof(findfile.cFileName)

    stackPath.add(String.from(dirName))
    while !stackPath.isEmpty()
    {
        var toScan = stackPath.popBack()
        if toScan.back() == Path.VolumeSeparatorChar do
            toScan += Path.DirectorySeparatorChar
        let searchPath = Path.combine(toScan.toString(), "*")

        let winPath = Utf16.toZeroTerminated(searchPath)
        let h       = try FindFirstFileExW(@dataof(winPath), .FindExInfoBasic, &findfile, .FindExSearchNameMatch, null, FIND_FIRST_EX_LARGE_FETCH)
        if h == INVALID_HANDLE_VALUE do
            continue

        for
        {
            let filename = Utf8.fromUtf16(@mkslice(dataof, Utf16.lengthZeroTerminated(dataof)))

            // Filter by file/folder
            var mustAdd = false
            if findfile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY
            {
                if filename != "." and filename != ".."
                {
                    mustAdd = options.wantDirectories
                    if options.recurse
                    {
                        var newPath = Path.combine(toScan.toString(), filename.toString())
                        stackPath.add(moveref &newPath)
                    }
                }
                else do
                    mustAdd = options.wantSpecialDirectories
            }
            else do
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

                    mustAdd = false
                    if options.matchExtension and Path.isExtension(fi.fullname, options.matchExtension) do
                        mustAdd = true
                    elif options.matchFileName and filename == options.matchFileName do
                        mustAdd = true
                    elif !options.matchExtension and !options.matchFileName do
                        mustAdd = true

                    if mustAdd
                    {
                        if !options.filterLambda or options.filterLambda(fi)
                        {
                            result.add(moveref &fi)
                        }
                    }
                }
            }

            let next = catch FindNextFileW(h, &findfile)
            if !next do
                break
        }

        assume FindClose(h)
    }

    return result
}

// Returns the current directory
func getCurrent()->String throw
{
    var result: retval
    let length  = GetCurrentDirectoryW(0, null)

    var arr16: Array'u16
    arr16.allocator = @getcontext().tempAllocator

    arr16.reserve(cast(u64) length + 1)
    arr16.count = GetCurrentDirectoryW(length + 1, @dataof(arr16))

    result = Utf8.fromUtf16(arr16.toSlice())
    Path.normalize(&result)
    return result
}

// Set the current directory
func setCurrent(dirName: string) throw
{
    let winPath = Utf16.toZeroTerminated(dirName)
    catch SetCurrentDirectoryW(@dataof(winPath))
    Errors.transformWinErrors(dirName)
}

// Get the list of logical drives
func getDrives()->Array'String throw
{
    var result:  retval
    var volName: [MAX_PATH] u8

    let slen = GetLogicalDriveStringsA(MAX_PATH, volName)
    if slen >= MAX_PATH do
        return result

    var ptr = &volName[0]
    while ptr[0]
    {
        let len = @strlen(ptr)
        result.add(@mkstring(ptr, len))
        ptr += len + 1
    }

    return result
}

// Get the list of volumes
func getVolumes()->Array'String throw
{
    var result: retval

    var volName:   [MAX_PATH] u8
    var driveName: [MAX_PATH] u8

    let h = FindFirstVolumeA(volName, MAX_PATH)
    if h == INVALID_HANDLE_VALUE do
        return result

    for
    {
        var ret: DWORD
        catch GetVolumePathNamesForVolumeNameA(volName, driveName, MAX_PATH, &ret)
        if @err == null
        {
            let len = @strlen(driveName)
            if len
            {
                var str = String.from(@mkstring(&driveName[0], len))
                Path.normalize(&str)
                result.add(str)
            }
        }

        if !FindNextVolumeA(h, volName, MAX_PATH) do
            break
    }

    FindVolumeClose(h)
    return result
}
