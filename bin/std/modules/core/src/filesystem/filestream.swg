#global public
#global namespace File

// Opaque handle to a file, used internally by FileStream
alias FileHandle = const *void

// Represents an open file stream for reading, writing, or seeking
#[Swag.NoCopy]
struct FileStream
{
    name:         String         // File name associated with the stream
    handle:       FileHandle     // Platform-specific file handle
    canRead:      bool           // True if the stream supports reading
    canSeek:      bool           // True if the stream supports seeking
    canWrite:     bool           // True if the stream supports writing
}

impl FileStream
{
    // Returns true if the stream is currently open and valid
    func isOpen(const me)->bool
    {
        return .handle != null
    }

    // Reads a value of type 'T' from the stream into 'buffer'; returns true if read was successful
    mtd(T) readValue(buffer: *T)->bool
    {
        return .read(buffer, #sizeof(T)) == #sizeof(T)
    }
}
