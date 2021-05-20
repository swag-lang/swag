#global public
#global namespace file

#[Swag.enumflags]
enum FileAttributes
{
    None = 0
    ReadOnly
    Hidden
    System
    Directory
    Archive
    Device
    Normal
    Temporary
    SparseFile
    ReparsePoint
    Compressed
    Offline
    NotContentIndexed
    Encrypted
}