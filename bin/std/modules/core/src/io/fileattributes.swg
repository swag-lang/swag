#global public
#global namespace File

#[Swag.EnumFlags]
enum FileAttributes
{
    Zero              = 0
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
