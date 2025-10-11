#global #if WINDOWS
#global public
#global namespace File
using Win32

impl enum FileAttributes
{
    internal func fromDWORD(attributes: DWORD)->FileAttributes
    {
        using FileAttributes
        if attributes == 0xFFFFFFFF do
            return Zero

        var result: FileAttributes = Zero
        if attributes & FILE_ATTRIBUTE_READONLY do
            result |= ReadOnly
        if attributes & FILE_ATTRIBUTE_HIDDEN do
            result |= Hidden
        if attributes & FILE_ATTRIBUTE_SYSTEM do
            result |= System
        if attributes & FILE_ATTRIBUTE_DIRECTORY do
            result |= Directory
        if attributes & FILE_ATTRIBUTE_ARCHIVE do
            result |= Archive
        if attributes & FILE_ATTRIBUTE_DEVICE do
            result |= Device
        if attributes & FILE_ATTRIBUTE_NORMAL do
            result |= Normal
        if attributes & FILE_ATTRIBUTE_TEMPORARY do
            result |= Temporary
        if attributes & FILE_ATTRIBUTE_SPARSE_FILE do
            result |= SparseFile
        if attributes & FILE_ATTRIBUTE_REPARSE_POINT do
            result |= ReparsePoint
        if attributes & FILE_ATTRIBUTE_COMPRESSED do
            result |= Compressed
        if attributes & FILE_ATTRIBUTE_OFFLINE do
            result |= Offline
        if attributes & FILE_ATTRIBUTE_NOT_CONTENT_INDEXED do
            result |= NotContentIndexed
        if attributes & FILE_ATTRIBUTE_ENCRYPTED do
            result |= Encrypted
        return result
    }
}

// Returns attributes associated to the given filename
func getAttributes(fullName: string)->FileAttributes throw
{
    let winPath    = Utf16.toZeroTerminated(fullName)
    let attributes = catch GetFileAttributesW(@dataof(winPath))
    Errors.transformWinErrors()
    return FileAttributes.fromDWORD(attributes)
}
