public namespace std.file;

#[swag.enumflags]
enum FileAttribute
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