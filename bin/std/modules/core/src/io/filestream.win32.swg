#global if WINDOWS
#global public
#global namespace File
using Win32

impl FileStream
{
    func opDrop(using self)
    {
        close()
    }

    // Read from the given file stream, and returns the number of bytes
    mtd read(buffer: *void, length: u64)->u64
    {
        Debug.assert(isOpen())
        Debug.assert(canRead)

        var dwRead: DWORD = 0
        Debug.assert(length <= Swag.U32.Max)
        discard ReadFile(handle, buffer, cast(u32) length, &dwRead, null)
        return cast(u64) dwRead
    }

    // Write to the given file stream, and returns the number of written bytes
    mtd write(buffer: const *void, length: u64)->u64 throw
    {
        Debug.assert(isOpen())
        Debug.assert(canWrite)

        var dwWrite: DWORD = 0
        try WriteFile(handle, buffer, cast(u32) length, &dwWrite, null)
        return cast(u64) dwWrite
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

        var hFile = cast(HANDLE) handle
        var high = 0'u32
        var low = try GetFileSize(hFile, &high)
        return low + (cast(u64) high << 32)
    }

    // Returns the current seek position of the given file stream
    mtd getPosition()->u64 throw
    {
        Debug.assert(isOpen())
        Debug.assert(canSeek)

        var hFile = cast(HANDLE) handle
        var high = 0
        var low = try SetFilePointer(hFile, 0, &high, FILE_CURRENT)
        return low + (cast(u64) high << 32)
    }

    // Set the current seek position of the given file stream
    mtd setPosition(origin: SeekOrigin, seek: u64) throw
    {
        Debug.assert(isOpen())
        Debug.assert(canSeek)

        var hFile = cast(HANDLE) handle
        var low = cast(LONG) (seek & 0xFFFFFFFF)
        var high = cast(LONG) (seek >> 32)

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