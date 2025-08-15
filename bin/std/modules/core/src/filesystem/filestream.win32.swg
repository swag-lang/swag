#global marked
#global #if WINDOWS
#global public
#global namespace File
using Win32

impl FileStream
{
    mtd opDrop()
    {
        self.close()
    }

    // Read from the given file stream, and returns the number of bytes
    mtd read(buffer: *void, length: u64)->u64
    {
        Debug.assert(self.isOpen())
        Debug.assert(self.canRead)

        var dwRead: DWORD = 0
        Debug.assert(length <= Swag.U32.Max)
        discard ReadFile(self.handle, buffer, cast(u32) length, &dwRead, null)
        return cast(u64) dwRead
    }

    // Write to the given file stream, and returns the number of written bytes
    mtd write(buffer: const *void, length: u64)->u64 throw
    {
        Debug.assert(self.isOpen())
        Debug.assert(self.canWrite)

        var dwWrite: DWORD = 0
        WriteFile(self.handle, buffer, cast(u32) length, &dwWrite, null)
        Errors.transformWinErrors()
        return cast(u64) dwWrite
    }

    // Close the given file stream
    mtd close()
    {
        if self.handle
        {
            CloseHandle(self.handle)
            self.handle = null
        }
    }

    // Returns the given file stream length on disk
    mtd getSize()->u64 throw
    {
        Debug.assert(self.isOpen())
        Debug.assert(self.canSeek)

        let hFile = cast(HANDLE) self.handle
        var high  = 0'u32
        let low   = GetFileSize(hFile, &high)
        Errors.transformWinErrors()
        return low + (cast(u64) high << 32)
    }

    // Returns the current seek position of the given file stream
    mtd getPosition()->u64 throw
    {
        Debug.assert(self.isOpen())
        Debug.assert(self.canSeek)

        let hFile = cast(HANDLE) self.handle
        var high  = 0
        let low   = SetFilePointer(hFile, 0, &high, FILE_CURRENT)
        Errors.transformWinErrors()
        return low + (cast(u64) high << 32)
    }

    // Set the current seek position of the given file stream
    mtd setPosition(origin: SeekOrigin, seek: u64) throw
    {
        Debug.assert(self.isOpen())
        Debug.assert(self.canSeek)

        let hFile = cast(HANDLE) self.handle
        let low   = cast(LONG) (seek & 0xFFFFFFFF)
        var high  = cast(LONG) (seek >> 32)

        var seekMode: DWORD
        switch origin
        {
        case Begin:
            seekMode = FILE_BEGIN
        case Current:
            seekMode = FILE_CURRENT
        case End:
            seekMode = FILE_END
        }

        SetFilePointer(hFile, low, &high, seekMode)
        Errors.transformWinErrors()
    }

    // Skip 'bytes' from current position
    mtd skipPosition(bytes: u64) throw
    {
        self.setPosition(.Current, bytes)
    }
}
