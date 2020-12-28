#global if WINDOWS
#global public
#global namespace file
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
func open(fullname: string, mode: FileMode, access: FileAccess, share: FileShare)-> {stream: FileStream, err: ErrorId}
{
    var result: retval
    using result

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
        stream.canRead = true
        wAccess |= GENERIC_READ
    }

    if access & FileAccess.Write
    {
        stream.canWrite = true
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
        err = error.last()
        return result
    }

    stream.name    = fullname
    stream.handle  = cast(FileHandle) hFile
    stream.canSeek = true

    return result
}

// Creates a new file stream for reading
func openRead(fullname: string) -> {stream: FileStream, err: ErrorId}
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
func getSize(stream: FileStream) -> {size: u64, err: ErrorId}
{
    debug.assert(stream.isOpen())
    debug.assert(stream.canSeek)

    var result: retval
    hFile := cast(HANDLE) stream.handle
    high  := 0'u32
    low   := GetFileSize(hFile, &high)
    if low == INVALID_FILE_SIZE
    {
        error.setLastErrorId()
        result.err = error.last()
        return result
    }

    result.size = low + (cast(u64) high << 32)
    return result
}

// Returns the current seek position of the given file stream
func getPosition(stream: FileStream) -> {position: u64, err: ErrorId}
{
    debug.assert(stream.isOpen())
    debug.assert(stream.canSeek)

    var result: retval
    hFile := cast(HANDLE) stream.handle
    high  := 0
    low   := SetFilePointer(hFile, 0, &high, FILE_CURRENT)
    if low == INVALID_SET_FILE_POINTER
    {
        error.setLastErrorId()
        result.err = error.last()
        return result
    }

    result.position = low + (cast(u64) high << 32)
    return result
}

// Set the current seek position of the given file stream
func setPosition(stream: FileStream, origin: SeekOrigin, seek: u64) -> ErrorId
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
