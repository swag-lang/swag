public namespace std.file;

struct FileInfo
{
    fullname:       String
    attributes =    FileAttributes.None
    creationTime:   time.DateTime
    lastAccessTime: time.DateTime
    lastWriteTime:  time.DateTime
    size:           u64
}
