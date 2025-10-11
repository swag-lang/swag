#global #if WINDOWS
#global public
#global namespace File
using Win32

// Delete the specified file
func delete(fullName: string) throw
{
    let winPath = Utf16.toZeroTerminated(fullName)
    catch DeleteFileW(@dataof(winPath))
    Errors.transformWinErrors(fullName)
}

// Change the file write date/time
func touch(fullName: string) throw
{
    let winPath = Utf16.toZeroTerminated(fullName)

    let hFile = catch CreateFileW(@dataof(winPath), FILE_WRITE_ATTRIBUTES, 0, null, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, null)
    Errors.transformWinErrors(fullName)
    defer CloseHandle(hFile)

    var ft: FILETIME = undefined
    var st: SYSTEMTIME = undefined
    GetSystemTime(&st)
    SystemTimeToFileTime(&st, &ft)
    SetFileTime(hFile, null, null, &ft)
}

// Returns true if the given file exists
func exists(fullName: string)->bool
{
    if fullName == null do
        return false
    let winPath    = Utf16.toZeroTerminated(fullName)
    let attributes = catch GetFileAttributesW(@dataof(winPath))
    if @err != null do
        return false
    if attributes == 0xFFFFFFFF do
        return false
    if attributes & FILE_ATTRIBUTE_DIRECTORY do
        return false
    return true
}

// Open a new file stream
func open(fullName: string, mode: FileMode, access: FileAccess, share: FileShare)->FileStream throw
{
    var result: retval

    // Open mode
    var wMode: DWORD
    switch mode
    {
    case Append:
        wMode = OPEN_EXISTING
    case Create:
        wMode = CREATE_ALWAYS
    case CreateNew:
        wMode = CREATE_NEW
    case Open:
        wMode = OPEN_EXISTING
    case OpenOrCreate:
        wMode = OPEN_ALWAYS
    case Truncate:
        wMode = TRUNCATE_EXISTING
    }

    // Read/Write mode
    var wAccess: DWORD
    if access & FileAccess.Read
    {
        result.canRead = true
        wAccess |= GENERIC_READ
    }

    if access & FileAccess.Write
    {
        result.canWrite = true
        wAccess |= GENERIC_WRITE
    }

    // Share mode
    var wShare: DWORD
    if share & FileShare.Read do
        wShare |= FILE_SHARE_READ
    if share & FileShare.Write do
        wShare |= FILE_SHARE_WRITE
    if share & FileShare.Delete do
        wShare |= FILE_SHARE_DELETE

    let winPath = Utf16.toZeroTerminated(fullName)
    let hFile   = catch CreateFileW(@dataof(winPath), wAccess, wShare, null, wMode, FILE_ATTRIBUTE_NORMAL, null)
    Errors.transformWinErrors(fullName)

    result.name    = fullName
    result.handle  = cast(FileHandle) hFile
    result.canSeek = true

    return result
}

// Creates a new file stream for reading
func openRead(fullName: string)->FileStream throw
{
    return open(fullName, FileMode.Open, FileAccess.Read, FileShare.Read)
}

// Creates a new file stream for writing
func openWrite(fullName: string)->FileStream throw
{
    return open(fullName, FileMode.Create, FileAccess.Write, FileShare.Zero)
}
