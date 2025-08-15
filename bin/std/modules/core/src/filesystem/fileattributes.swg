#global public
#global namespace File

#[Swag.EnumFlags]
// Attributes that define file or directory characteristics
enum FileAttributes
{
    Zero              = 0     // No attributes set
    ReadOnly                  // File is read-only
    Hidden                    // File is hidden
    System                    // File is part of the operating system
    Directory                 // Entry is a directory
    Archive                   // File is marked for backup or removal
    Device                    // Reserved for system use (rarely used)
    Normal                    // File has no other attributes set
    Temporary                 // File is used for temporary storage
    SparseFile                // File is a sparse file
    ReparsePoint              // File contains a reparse point (e.g., symbolic link)
    Compressed                // File is compressed
    Offline                   // File data is not available immediately (e.g., archived)
    NotContentIndexed         // File is excluded from content indexing
    Encrypted                 // File is encrypted
}
