#global if WINDOWS
#global public
#global namespace file
using win32

// Returns true if the given file exists
func exists(fullname: string)->bool
{
    if fullname == null return false
    winPath := path.toWin16(fullname)
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

    winPath := path.toWin16(fullname)
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

// Read from the given file stream, and returns the number of bytes
func read(stream: FileStream, buffer: *void, length: uint)->uint
{
    debug.assert(stream.isOpen())
    debug.assert(stream.canRead)

    var dwRead: DWORD = 0
    debug.assert(length <= swag.U32.Max)
    rlen := ReadFile(stream.handle, buffer, cast(u32) length, &dwRead, null)
    return cast(uint) dwRead
}

// Write to the given file stream, and returns the number of written bytes
func write(stream: FileStream, buffer: const *void, length: uint)->uint throw
{
    debug.assert(stream.isOpen())
    debug.assert(stream.canWrite)

    var dwWrite: DWORD = 0
    try WriteFile(stream.handle, buffer, cast(u32) length, &dwWrite, null)
    return cast(uint) dwWrite
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
func getSize(stream: FileStream)->u64 throw
{
    debug.assert(stream.isOpen())
    debug.assert(stream.canSeek)

    hFile := cast(HANDLE) stream.handle
    high  := 0'u32
    low   := try GetFileSize(hFile, &high)
    return low + (cast(u64) high << 32)
}

// Returns the current seek position of the given file stream
func getPosition(stream: FileStream)->u64 throw
{
    debug.assert(stream.isOpen())
    debug.assert(stream.canSeek)

    hFile := cast(HANDLE) stream.handle
    high  := 0
    low   := try SetFilePointer(hFile, 0, &high, FILE_CURRENT)
    return low + (cast(u64) high << 32)
}

// Set the current seek position of the given file stream
func setPosition(stream: FileStream, origin: SeekOrigin, seek: u64) throw
{
    debug.assert(stream.isOpen())
    debug.assert(stream.canSeek)

    hFile := cast(HANDLE) stream.handle
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
