#if std.WINDOWS;
using win32
public namespace std.file;

// Returns various file informations
func getInfo(fullname: string)->FileInfo
{
    var result:     retval
    var attrDatas:  WIN32_FILE_ATTRIBUTE_DATA
    res := GetFileAttributesExA(@dataof(fullname), GET_FILEEX_INFO_LEVELS.GetFileExInfoStandard, &attrDatas)
    if res == 0 return result

    result.attributes = FileAttribute.fromDWORD(attrDatas.dwFileAttributes)
    result.size = attrDatas.nFileSizeLow + (cast(u64) attrDatas.nFileSizeHigh << 32)

    var fileTime: SYSTEMTIME
    FileTimeToSystemTime(&attrDatas.ftCreationTime, &fileTime)
    result.creationTime.fromSYSTEMTIME(fileTime)
    FileTimeToSystemTime(&attrDatas.ftLastAccessTime, &fileTime)
    result.lastAccessTime.fromSYSTEMTIME(fileTime)
    FileTimeToSystemTime(&attrDatas.ftLastWriteTime, &fileTime)
    result.lastWriteTime.fromSYSTEMTIME(fileTime)

    return result
}
