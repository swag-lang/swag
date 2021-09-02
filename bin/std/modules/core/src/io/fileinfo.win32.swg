#global if WINDOWS
#global public
#global namespace File
using Win32

// Returns various file informations
func getInfo(fullname: string)->FileInfo throw
{
    var result:     retval
    var attrDatas:  WIN32_FILE_ATTRIBUTE_DATA

    winPath := utf16.toZeroTerminated(fullname)
    try GetFileAttributesExW(@dataof(winPath), GET_FILEEX_INFO_LEVELS.GetFileExInfoStandard, &attrDatas)

    result.fullname = fullname
    result.attributes = FileAttributes.fromDWORD(attrDatas.dwFileAttributes)
    result.size = attrDatas.nFileSizeLow + (cast(u64) attrDatas.nFileSizeHigh << 32)

    var fileTime: SYSTEMTIME
    catch FileTimeToSystemTime(&attrDatas.ftCreationTime, &fileTime)
    result.creationTime.setSYSTEMTIME(fileTime)
    catch FileTimeToSystemTime(&attrDatas.ftLastAccessTime, &fileTime)
    result.lastAccessTime.setSYSTEMTIME(fileTime)
    catch FileTimeToSystemTime(&attrDatas.ftLastWriteTime, &fileTime)
    result.lastWriteTime.setSYSTEMTIME(fileTime)

    return result
}
