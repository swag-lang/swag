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
    if @err return false
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
    hFile := try CreateFileW(@dataof(winPath), wAccess, wShare, null, wMode, FILE_ATTRIBUTE_NORMAL, null);

    result.name    = fullname
    result.handle  = cast(FileHandle) hFile
    result.canSeek = true

    return result
}

// Creates a new file stream for reading
func openRead(fullname: string)->FileStream throw
{
    return try open(fullname, FileMode.Open, FileAccess.Read, FileShare.Read)
}

// Creates a new file stream for writing
func openWrite(fullname: string)->FileStream throw
{
    return try open(fullname, FileMode.Create, FileAccess.Write, FileShare.None)
}

// Locate file in the file explorer
func locateInExplorer(name: string)
{
    var x: String = name
    x.replace("/", "\\")
    x = Format.toString("/select,\"%\"", x)
    discard catch Env.startProcess("explorer.exe", x)
}

impl FileStream
{
    // Read from the given file stream, and returns the number of bytes
    mtd read(buffer: *void, length: uint)->uint
    {
        Debug.assert(isOpen())
        Debug.assert(canRead)

        var dwRead: DWORD = 0
        Debug.assert(length <= Swag.U32.Max)
        discard ReadFile(handle, buffer, cast(u32) length, &dwRead, null)
        return cast(uint) dwRead
    }

    // Write to the given file stream, and returns the number of written bytes
    mtd write(buffer: const *void, length: uint)->uint throw
    {
        Debug.assert(isOpen())
        Debug.assert(canWrite)

        var dwWrite: DWORD = 0
        try WriteFile(handle, buffer, cast(u32) length, &dwWrite, null)
        return cast(uint) dwWrite
    }

    // Close the given file stream
    mtd close()
    {
        if handle
        {
            CloseHandle(handle)
            handle = null
        }
    }

    // Returns the given file stream length on disk
    mtd getSize()->u64 throw
    {
        Debug.assert(isOpen())
        Debug.assert(canSeek)

        hFile := cast(HANDLE) handle
        high  := 0'u32
        low   := try GetFileSize(hFile, &high)
        return low + (cast(u64) high << 32)
    }

    // Returns the current seek position of the given file stream
    mtd getPosition()->u64 throw
    {
        Debug.assert(isOpen())
        Debug.assert(canSeek)

        hFile := cast(HANDLE) handle
        high  := 0
        low   := try SetFilePointer(hFile, 0, &high, FILE_CURRENT)
        return low + (cast(u64) high << 32)
    }

    // Set the current seek position of the given file stream
    mtd setPosition(origin: SeekOrigin, seek: u64) throw
    {
        Debug.assert(isOpen())
        Debug.assert(canSeek)

        hFile := cast(HANDLE) handle
        low   := cast(LONG) (seek & 0xFFFFFFFF)
        high  := cast(LONG) (seek >> 32)

        var seekMode: DWORD
        switch origin
        {
        case Begin:      seekMode = FILE_BEGIN
        case Current:    seekMode = FILE_CURRENT
        case End:        seekMode = FILE_END
        }

        try SetFilePointer(hFile, low, &high, seekMode)
    }
}