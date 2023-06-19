#global if WINDOWS
#global public
#global namespace File
using Win32

// Delete the specified file
func delete(fullname: string) throw
{
    winPath := Utf16.toZeroTerminated(fullname)
    try DeleteFileW(@dataof(winPath))
}

// Change the file write date/time
func touch(fullname: string) throw
{
    winPath := Utf16.toZeroTerminated(fullname)

    hFile := CreateFileW(@dataof(winPath), FILE_WRITE_ATTRIBUTES, 0, null, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, null)
    defer CloseHandle(hFile)

    var ft: FILETIME = undefined
    var st: SYSTEMTIME = undefined
    GetSystemTime(&st)
    SystemTimeToFileTime(&st, &ft)
    SetFileTime(hFile, null, null, &ft)
}

// Returns true if the given file exists
func exists(fullname: string)->bool
{
    if fullname == null return false
    winPath := Utf16.toZeroTerminated(fullname)
    attributes := catch GetFileAttributesW(@dataof(winPath))
    if @errmsg return false
    if attributes == 0xFFFFFFFF return false
    if attributes & FILE_ATTRIBUTE_DIRECTORY return false
    return true
}

// Open a new file stream
func open(fullname: string, mode: FileMode, access: FileAccess, share: FileShare)->FileStream throw
{
    var result: retval

    // Open mode
    var wMode: DWORD
    switch mode
    {
    case Append:       wMode = OPEN_EXISTING
    case Create:       wMode = CREATE_ALWAYS
    case CreateNew:    wMode = CREATE_NEW
    case Open:         wMode = OPEN_EXISTING
    case OpenOrCreate: wMode = OPEN_ALWAYS
    case Truncate:     wMode = TRUNCATE_EXISTING
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
    if share & FileShare.Read
        wShare |= FILE_SHARE_READ
    if share & FileShare.Write
        wShare |= FILE_SHARE_WRITE
    if share & FileShare.Delete
        wShare |= FILE_SHARE_DELETE

    winPath := Utf16.toZeroTerminated(fullname)
    hFile := CreateFileW(@dataof(winPath), wAccess, wShare, null, wMode, FILE_ATTRIBUTE_NORMAL, null);

    result.name    = fullname
    result.handle  = cast(FileHandle) hFile
    result.canSeek = true

    return result
}

// Creates a new file stream for reading
func openRead(fullname: string)->FileStream throw
{
    return open(fullname, FileMode.Open, FileAccess.Read, FileShare.Read)
}

// Creates a new file stream for writing
func openWrite(fullname: string)->FileStream throw
{
    return open(fullname, FileMode.Create, FileAccess.Write, FileShare.Zero)
}

// Locate file in the file explorer
func locateInExplorer(name: string)
{
    var x: String = name
    x.replace("/", "\\")
    x = Format.toString("/select,\"%\"", x)
    discard catch Env.startProcess("explorer.exe", x)
}
