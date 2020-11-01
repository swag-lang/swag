#if std.WINDOWS;
namespace std.file;
using win32

// Returns true if the given file exists
public func exists(path: string)->bool
{
    return GetFileAttributesA(@dataof(path)) == 0xFFFFFFFF ? false : true
}

public func openRead(path: string)->Stream
{
    var stream: Stream

    hFile := CreateFileA(@dataof(path), GENERIC_READ, FILE_SHARE_READ, null, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, null);
    if hFile == INVALID_HANDLE_VALUE
        return stream

    stream.handle = cast(FileHandle) hFile
    return stream
}

public func readAllText(path: string)->String
{
    var result: String

    stream := openRead(path)
    if !stream.isOpen() return result

    setPosition(stream, FILE_END, 0)
    len := cast(u32) getPosition(stream)
    setPosition(stream, FILE_BEGIN, 0)

    result.reserve(len)
    result.length = len

    var dwRead: DWORD = 0
    ReadFile(stream.handle, result.buffer, len, &dwRead, null)

    close(stream)
    return result
}

// Close the given stream
func close(stream: Stream)
{
    if stream.handle
        CloseHandle(stream.handle)
}

func getPosition(stream: Stream)->u64
{
    debug.assert(stream.isOpen())

    hFile := cast(HANDLE) stream.handle
    high := 0
    low := SetFilePointer(hFile, 0, &high, FILE_CURRENT)
    return low + (cast(u64) high << 32)
}

func setPosition(stream: Stream, seekMode: DWORD, seek: u64)->bool
{
    debug.assert(stream.isOpen())

    hFile := cast(HANDLE) stream.handle
    low  := cast(LONG) (seek & 0xFFFFFFFF)
    high := cast(LONG) (seek >> 32)
    return SetFilePointer(hFile, low, &high, seekMode) != INVALID_SET_FILE_POINTER
}
