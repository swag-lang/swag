#global public
#global namespace File

typealias FileHandle = const *void

#[Swag.NoCopy]
struct FileStream
{
    name:       String
    handle:     FileHandle
    canRead:    bool
    canSeek:    bool
    canWrite:   bool
}

impl FileStream
{
    // Returns true if the stream is valid
    func isOpen(using const self)->bool
    {
        return handle != null
    }
}