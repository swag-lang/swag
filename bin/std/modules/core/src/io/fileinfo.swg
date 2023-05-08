#global public
#global namespace File
using Time

struct FileInfo
{
    fullname:       String
    attributes =    FileAttributes.Zero
    creationTime:   DateTime
    lastAccessTime: DateTime
    lastWriteTime:  DateTime
    size:           u64
}

impl FileInfo
{
    #[Swag.Inline]
    mtdc isDirectory() => attributes.has(.Directory)
}