#global #if WINDOWS
#global public
#global namespace File
using Win32

impl enum FileAttributes
{
    internal func fromDWORD(attributes: DWORD)->FileAttributes
    {
        using FileAttributes
        if attributes == 0xFFFFFFFF:
            return Zero

        var result: FileAttributes = Zero
        if attributes & FILE_ATTRIBUTE_READONLY:
            result |= ReadOnly
        if attributes & FILE_ATTRIBUTE_HIDDEN:
            result |= Hidden
        if attributes & FILE_ATTRIBUTE_SYSTEM:
            result |= System
        if attributes & FILE_ATTRIBUTE_DIRECTORY:
            result |= Directory
        if attributes & FILE_ATTRIBUTE_ARCHIVE:
            result |= Archive
        if attributes & FILE_ATTRIBUTE_DEVICE:
            result |= Device
        if attributes & FILE_ATTRIBUTE_NORMAL:
            result |= Normal
        if attributes & FILE_ATTRIBUTE_TEMPORARY:
            result |= Temporary
        if attributes & FILE_ATTRIBUTE_SPARSE_FILE:
            result |= SparseFile
        if attributes & FILE_ATTRIBUTE_REPARSE_POINT:
            result |= ReparsePoint
        if attributes & FILE_ATTRIBUTE_COMPRESSED:
            result |= Compressed
        if attributes & FILE_ATTRIBUTE_OFFLINE:
            result |= Offline
        if attributes & FILE_ATTRIBUTE_NOT_CONTENT_INDEXED:
            result |= NotContentIndexed
        if attributes & FILE_ATTRIBUTE_ENCRYPTED:
            result |= Encrypted
        return result
    }
}

// Returns attributes associated to the given filename
func getAttributes(fullName: string)->FileAttributes throw
{
    var winPath    = Utf16.toZeroTerminated(fullName)
    let attributes = catch GetFileAttributesW(@dataof(winPath))
    Errors.transformWinErrors()
    return FileAttributes.fromDWORD(attributes)
}
