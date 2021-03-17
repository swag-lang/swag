#global if WINDOWS
#global public
#global namespace file
using win32

// Returns various file informations
func getInfo(fullname: string)->FileInfo throw
{
    var result:     retval
    var attrDatas:  WIN32_FILE_ATTRIBUTE_DATA

    winPath := path.toWin16(fullname)
    res := GetFileAttributesExW(@dataof(winPath), GET_FILEEX_INFO_LEVELS.GetFileExInfoStandard, &attrDatas)
    if res == 0
        try error.setErrorWin32(GetLastError())

    result.fullname = fullname
    result.attributes = FileAttributes.fromDWORD(attrDatas.dwFileAttributes)
    result.size = attrDatas.nFileSizeLow + (cast(u64) attrDatas.nFileSizeHigh << 32)

    var fileTime: SYSTEMTIME
    hw1 := FileTimeToSystemTime(&attrDatas.ftCreationTime, &fileTime)
    result.creationTime.setSYSTEMTIME(fileTime)
    hw2 := FileTimeToSystemTime(&attrDatas.ftLastAccessTime, &fileTime)
    result.lastAccessTime.setSYSTEMTIME(fileTime)
    hw3 := FileTimeToSystemTime(&attrDatas.ftLastWriteTime, &fileTime)
    result.lastWriteTime.setSYSTEMTIME(fileTime)

    return result
}
