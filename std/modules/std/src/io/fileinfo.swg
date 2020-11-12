public namespace std.file;

struct FileInfo
{
    fullname:       String
    attributes =    FileAttribute.None
    creationTime:   time.DateTime
    lastAccessTime: time.DateTime
    lastWriteTime:  time.DateTime
    size:           u64
}
