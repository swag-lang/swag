#global public
#global namespace file

alias FileHandle = const *void

#[swag.nocopy]
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
        debug.assert(!isOpen(), "missing call to close()")
    }

    // Returns true if the stream is valid
    func isOpen(using const self)->bool
    {
        return handle != null
    }
}