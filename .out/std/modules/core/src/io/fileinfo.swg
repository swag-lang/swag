#global public
#global namespace File
using Time

struct FileInfo
{
    fullname:       String
    attributes =    FileAttributes.None
    creationTime:   DateTime
    lastAccessTime: DateTime
    lastWriteTime:  DateTime
    size:           u64
}
