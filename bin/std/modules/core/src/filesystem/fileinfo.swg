#global public
#global namespace File
using Time

// Represents metadata and properties of a file or directory
struct FileInfo
{
    fullname:           String                    // Full path of the file or directory
    attributes          = FileAttributes.Zero     // Attributes describing the file (e.g., read-only, hidden)
    creationTime:       DateTime                  // Time the file or directory was created
    lastAccessTime:     DateTime                  // Time the file or directory was last accessed
    lastWriteTime:      DateTime                  // Time the file or directory was last modified
    size:               u64                       // Size of the file in bytes (0 for directories)
}

impl FileInfo
{
    // Returns true if the file info represents a directory
    #[Swag.Inline]
    mtd const isDirectory() => .attributes.has(.Directory)
}
