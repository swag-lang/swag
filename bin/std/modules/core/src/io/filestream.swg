#global public
#global namespace File

typealias FileHandle = const *void

#[Swag.NoCopy]
struct FileStream
{
    name:         String
    handle:       FileHandle
    canRead:      bool
    canSeek:      bool
    canWrite:     bool
}

impl FileStream
{
    // Returns true if the stream is valid
    func isOpen(using const self)->bool
    {
        return handle != null
    }

    // Read from the given file stream, and returns 'true' if all bytes have been read
    mtd(T) readValue(buffer: *T)->bool
    {
        return read(buffer, @sizeof(T)) == @sizeof(T)
    }
}
