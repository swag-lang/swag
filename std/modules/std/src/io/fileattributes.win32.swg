#if std.WINDOWS;
public namespace std.file;
using win32

impl enum FileAttributes
{
    protected func fromDWORD(attributes: DWORD)->FileAttributes
    {
        using FileAttributes
        var result: FileAttributes = autocast 0
        if attributes & FILE_ATTRIBUTE_READONLY
            result |= ReadOnly
        if attributes & FILE_ATTRIBUTE_HIDDEN
            result |= Hidden
        if attributes & FILE_ATTRIBUTE_SYSTEM
            result |= System
        if attributes & FILE_ATTRIBUTE_DIRECTORY
            result |= Directory
        if attributes & FILE_ATTRIBUTE_ARCHIVE
            result |= Archive
        if attributes & FILE_ATTRIBUTE_DEVICE
            result |= Device
        if attributes & FILE_ATTRIBUTE_NORMAL
            result |= Normal
        if attributes & FILE_ATTRIBUTE_TEMPORARY
            result |= Temporary
        if attributes & FILE_ATTRIBUTE_SPARSE_FILE
            result |= SparseFile
        if attributes & FILE_ATTRIBUTE_REPARSE_POINT
            result |= ReparsePoint
        if attributes & FILE_ATTRIBUTE_COMPRESSED
            result |= Compressed
        if attributes & FILE_ATTRIBUTE_OFFLINE
            result |= Offline
        if attributes & FILE_ATTRIBUTE_NOT_CONTENT_INDEXED
            result |= NotContentIndexed
        if attributes & FILE_ATTRIBUTE_ENCRYPTED
            result |= Encrypted
        return result
    }
}

// Returns attributes associated to the given filename
// Returns FileAttribute.None in case of error
func getAttribute(fullname: string)->FileAttributes
{
    res := GetFileAttributesA(@dataof(fullname))
    if res == 0xFFFFFFFF return FileAttributes.None
    return FileAttributes.fromDWORD(res)
}