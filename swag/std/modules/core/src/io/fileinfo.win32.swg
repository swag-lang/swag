#global if WINDOWS
#global public
#global namespace file
using win32

// Returns various file informations
func getInfo(fullname: string)->FileInfo
{
    var result:     retval
    var attrDatas:  WIN32_FILE_ATTRIBUTE_DATA
    res := GetFileAttributesExA(@dataof(fullname), GET_FILEEX_INFO_LEVELS.GetFileExInfoStandard, &attrDatas)
    if res == 0 return result

    result.fullname = fullname
    result.attributes = FileAttributes.fromDWORD(attrDatas.dwFileAttributes)
    result.size = attrDatas.nFileSizeLow + (cast(u64) attrDatas.nFileSizeHigh << 32)

    var fileTime: SYSTEMTIME
    FileTimeToSystemTime(&attrDatas.ftCreationTime, &fileTime)
    result.creationTime.setSYSTEMTIME(fileTime)
    FileTimeToSystemTime(&attrDatas.ftLastAccessTime, &fileTime)
    result.lastAccessTime.setSYSTEMTIME(fileTime)
    FileTimeToSystemTime(&attrDatas.ftLastWriteTime, &fileTime)
    result.lastWriteTime.setSYSTEMTIME(fileTime)

    return result
}
