namespace std.file;
using win32

#if std.WINDOWS
{
    // Returns true if the given file exists
    public func exists(path: string)->bool
    {
        return GetFileAttributesA(@dataof(path)) == 0xFFFFFFFF ? false : true
    }

    public func readAllText(path: string)->String
    {
        var result: String

        hFile := CreateFileA(@dataof(path), GENERIC_READ, FILE_SHARE_READ, null, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, null);
        if hFile == INVALID_HANDLE_VALUE
            return result

        setPosition(hFile, FILE_END, 0)
        len := cast(u32) getPosition(hFile)
        setPosition(hFile, FILE_BEGIN, 0)

        result.reserve(len)
        result.length = len

        var dwRead: DWORD = 0
        ReadFile(hFile, result.buffer, len, &dwRead, null)

        CloseHandle(hFile)
        return result
    }

    func getPosition(hFile: HANDLE)->u64
    {
        high := 0
        low := SetFilePointer(hFile, 0, &high, FILE_CURRENT)
        return low + (cast(u64) high << 32)
    }

    func setPosition(hFile: HANDLE, seekMode: DWORD, seek: u64)->bool
    {
        low  := cast(LONG) (seek & 0xFFFFFFFF)
        high := cast(LONG) (seek >> 32)
        return SetFilePointer(hFile, low, &high, seekMode) != INVALID_SET_FILE_POINTER
    }
}