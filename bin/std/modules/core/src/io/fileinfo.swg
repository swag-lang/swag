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
    mtd const isDirectory() => attributes.has(.Directory)
}