#if std.WINDOWS;
using win32
public namespace std.file;

// Returns true if the given file exists
func exists(fullname: string)->bool
{
    return GetFileAttributesA(@dataof(fullname)) == 0xFFFFFFFF ? false : true
}

// Creates a new file stream for reading
func openRead(fullname: string)->Stream
{
    var stream: Stream

    hFile := CreateFileA(@dataof(fullname), GENERIC_READ, FILE_SHARE_READ, null, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, null);
    if hFile == INVALID_HANDLE_VALUE
        return stream

    stream.name = fullname
    stream.handle = cast(FileHandle) hFile
    stream.canRead = true
    stream.canSeek = true

    return stream
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
func close(stream: Stream)
{
    if stream.handle
        CloseHandle(stream.handle)
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
    switch origin
    {
        case SeekOrigin.Begin:      seekMode = FILE_BEGIN
        case SeekOrigin.Current:    seekMode = FILE_CURRENT
        case SeekOrigin.End:        seekMode = FILE_END
    }

    return SetFilePointer(hFile, low, &high, seekMode) != INVALID_SET_FILE_POINTER
}
