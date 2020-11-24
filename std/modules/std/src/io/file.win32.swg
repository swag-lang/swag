#if std.WINDOWS;
public namespace std.file;
using win32

// Returns true if the given file exists
func exists(fullname: string)->bool
{
    if fullname == null return false
    attributes := GetFileAttributesA(@dataof(fullname))
    if attributes == 0xFFFFFFFF return false
    if attributes & FILE_ATTRIBUTE_DIRECTORY return false
    return true
}

// Open a new file stream
func open(fullname: string, mode: FileMode, access: FileAccess, share: FileShare)-> {stream: Stream, err: error.ErrorId}
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
        result.stream.canRead = true
        wAccess |= GENERIC_READ
    }

    if access & FileAccess.Write
    {
        result.stream.canWrite = true
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

    hFile := CreateFileA(@dataof(fullname), wAccess, wShare, null, wMode, FILE_ATTRIBUTE_NORMAL, null);
    if hFile == INVALID_HANDLE_VALUE
    {
        error.setLastErrorId()
        result.err = error.last()
        return result
    }

    result.stream.name    = fullname
    result.stream.handle  = cast(FileHandle) hFile
    result.stream.canSeek = true

    return result
}

// Creates a new file stream for reading
func openRead(fullname: string)-> {stream: Stream, err: error.ErrorId}
{
    return open(fullname, FileMode.Open, FileAccess.Read, FileShare.Read)
}

// Read from the given file stream, and returns the number of bytes
func read(stream: Stream, buffer: *void, length: u32)->u32
{
    debug.assert(stream.isOpen())
    debug.assert(stream.canRead)

    var dwRead: DWORD = 0
    ReadFile(stream.handle, buffer, length, &dwRead, null)
    return dwRead
}

// Close the given file stream
func close(stream: *Stream)
{
    if stream.handle
    {
        CloseHandle(stream.handle)
        stream.handle = null
    }
}

// Returns the given file stream length on disk
func getSize(stream: Stream)->u64
{
    debug.assert(stream.isOpen())
    debug.assert(stream.canSeek)

    hFile := cast(HANDLE) stream.handle
    high  := 0'u32
    low   := GetFileSize(hFile, &high)
    return low + (cast(u64) high << 32)
}

// Returns the current seek position of the given file stream
func getPosition(stream: Stream)->u64
{
    debug.assert(stream.isOpen())
    debug.assert(stream.canSeek)

    hFile := cast(HANDLE) stream.handle
    high  := 0
    low   := SetFilePointer(hFile, 0, &high, FILE_CURRENT)
    return low + (cast(u64) high << 32)
}

// Set the current seek position of the given file stream
func setPosition(stream: Stream, origin: SeekOrigin, seek: u64) -> error.ErrorId
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
        error.setLastErrorId()
        return error.last()
    }

    return null
}
