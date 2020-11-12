public namespace std.file;
using time

struct FileInfo
{
    fullname:       String
    attributes =    FileAttributes.None
    creationTime:   DateTime
    lastAccessTime: DateTime
    lastWriteTime:  DateTime
    size:           u64
}
