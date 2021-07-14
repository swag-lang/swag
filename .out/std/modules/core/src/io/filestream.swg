#global public
#global namespace File

alias FileHandle = const *void

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
    func opDrop(using self)
    {
        Debug.assert(!isOpen(), "missing call to close()")
    }

    // Returns true if the stream is valid
    func isOpen(using const self)->bool
    {
        return handle != null
    }
}