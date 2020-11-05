#if std.WINDOWS;
using win32
public namespace std.file;

// Returns attributes associated to the given filename
// Returns FileAttribute.None in case of error
func getAttribute(fullname: string)->FileAttribute
{
    using FileAttribute
    res := GetFileAttributesA(@dataof(fullname))
    if res == 0xFFFFFFFF return None

    var result: FileAttribute = acast 0
    if res & FILE_ATTRIBUTE_READONLY
        result |= ReadOnly
    if res & FILE_ATTRIBUTE_HIDDEN
        result |= Hidden
    if res & FILE_ATTRIBUTE_SYSTEM
        result |= System
    if res & FILE_ATTRIBUTE_DIRECTORY
        result |= Directory
    if res & FILE_ATTRIBUTE_ARCHIVE
        result |= Archive
    if res & FILE_ATTRIBUTE_DEVICE
        result |= Device
    if res & FILE_ATTRIBUTE_NORMAL
        result |= Normal
    if res & FILE_ATTRIBUTE_TEMPORARY
        result |= Temporary
    if res & FILE_ATTRIBUTE_SPARSE_FILE
        result |= SparseFile
    if res & FILE_ATTRIBUTE_REPARSE_POINT
        result |= ReparsePoint
    if res & FILE_ATTRIBUTE_COMPRESSED
        result |= Compressed
    if res & FILE_ATTRIBUTE_OFFLINE
        result |= Offline
    if res & FILE_ATTRIBUTE_NOT_CONTENT_INDEXED
        result |= NotContentIndexed
    if res & FILE_ATTRIBUTE_ENCRYPTED
        result |= Encrypted

    return result
}

// Returns true if the given file exists
func exists(fullname: string)->bool
{
    if fullname == null return false
    return GetFileAttributesA(@dataof(fullname)) == 0xFFFFFFFF ? false : true
}

// Open a new file stream
func open(fullname: string, mode: FileMode, access: FileAccess, share: FileShare)->Stream
{
    var stream: Stream

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
        return stream

    stream.name    = fullname
    stream.handle  = cast(FileHandle) hFile
    stream.canSeek = true

    return stream
}

// Creates a new file stream for reading
func openRead(fullname: string)->Stream
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
func setPosition(stream: Stream, origin: SeekOrigin, seek: u64)->bool
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

    return SetFilePointer(hFile, low, &high, seekMode) != INVALID_SET_FILE_POINTER
}
