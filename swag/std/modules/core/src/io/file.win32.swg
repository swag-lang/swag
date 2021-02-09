#global if WINDOWS
#global public
#global namespace file
using win32

// Returns true if the given file exists
func exists(fullname: string)->bool
{
    if fullname == null return false
    winPath := path.toWin16(fullname)
    attributes := GetFileAttributesW(@dataof(winPath))
    if attributes == 0xFFFFFFFF return false
    if attributes & FILE_ATTRIBUTE_DIRECTORY return false
    return true
}

// Open a new file stream
#[swag.haserror]
func open(fullname: string, mode: FileMode, access: FileAccess, share: FileShare)->FileStream
{
    var result: retval

    // Open mode
    var wMode: DWORD
    using FileMode
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

    winPath := path.toWin16(fullname)
    hFile := CreateFileW(@dataof(winPath), wAccess, wShare, null, wMode, FILE_ATTRIBUTE_NORMAL, null);
    if hFile == INVALID_HANDLE_VALUE
    {
        error.setErrorWin32(GetLastError())
        return result
    }

    result.name    = fullname
    result.handle  = cast(FileHandle) hFile
    result.canSeek = true

    return result
}

// Creates a new file stream for reading
func openRead(fullname: string)->FileStream
{
    return open(fullname, FileMode.Open, FileAccess.Read, FileShare.Read)
}

// Read from the given file stream, and returns the number of bytes
func read(stream: FileStream, buffer: *void, length: uint)->uint
{
    debug.assert(stream.isOpen())
    debug.assert(stream.canRead)

    var dwRead: DWORD = 0
    debug.assert(length <= swag.MaxU32)
    ReadFile(stream.handle, buffer, cast(u32) length, &dwRead, null)
    return cast(uint) dwRead
}

// Close the given file stream
func close(stream: *FileStream)
{
    if stream.handle
    {
        CloseHandle(stream.handle)
        stream.handle = null
    }
}

// Returns the given file stream length on disk
#[swag.haserror]
func getSize(stream: FileStream)->u64
{
    debug.assert(stream.isOpen())
    debug.assert(stream.canSeek)

    hFile := cast(HANDLE) stream.handle
    high  := 0'u32
    low   := GetFileSize(hFile, &high)
    if low == INVALID_FILE_SIZE
    {
        error.setErrorWin32(GetLastError())
        return 0
    }

    return low + (cast(u64) high << 32)
}

// Returns the current seek position of the given file stream
#[swag.haserror]
func getPosition(stream: FileStream)->u64
{
    debug.assert(stream.isOpen())
    debug.assert(stream.canSeek)

    hFile := cast(HANDLE) stream.handle
    high  := 0
    low   := SetFilePointer(hFile, 0, &high, FILE_CURRENT)
    if low == INVALID_SET_FILE_POINTER
    {
        error.setErrorWin32(GetLastError())
        return 0
    }

    return low + (cast(u64) high << 32)
}

// Set the current seek position of the given file stream
#[swag.haserror]
func setPosition(stream: FileStream, origin: SeekOrigin, seek: u64)
{
    debug.assert(stream.isOpen())
    debug.assert(stream.canSeek)

    hFile := cast(HANDLE) stream.handle
    low   := cast(LONG) (seek & 0xFFFFFFFF)
    high  := cast(LONG) (seek >> 32)

    var seekMode: DWORD
    using SeekOrigin
    switch origin
    {
        case Begin:      seekMode = FILE_BEGIN
        case Current:    seekMode = FILE_CURRENT
        case End:        seekMode = FILE_END
    }

    success := SetFilePointer(hFile, low, &high, seekMode)
    if success == INVALID_SET_FILE_POINTER
    {
        error.setErrorWin32(GetLastError())
        return
    }
}
