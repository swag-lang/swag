public namespace file;

alias FileHandle = const *void

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
        file.close(self)
    }

    // Returns true if the stream is valid
    func isOpen(using self)->bool
    {
        return handle != null
    }
}