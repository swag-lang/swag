public namespace std.file;

alias FileHandle = *void

struct Stream
{
    name:       String
    handle:     FileHandle
    canRead:    bool
    canSeek:    bool
    canWrite:   bool
}

impl Stream
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