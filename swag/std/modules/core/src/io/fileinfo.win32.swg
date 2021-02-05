#global if WINDOWS
#global public
#global namespace file
using win32

// Returns various file informations
func getInfo(fullname: string) -> {infos: FileInfo, err: ErrorId}
{
    var result:     retval
    var attrDatas:  WIN32_FILE_ATTRIBUTE_DATA

    winPath := path.toWin16(fullname)
    res := GetFileAttributesExW(@dataof(winPath), GET_FILEEX_INFO_LEVELS.GetFileExInfoStandard, &attrDatas)
    if res == 0
    {
        error.setLastErrorId()
        result.err = error.last()
        return result
    }

    result.infos.fullname = fullname
    result.infos.attributes = FileAttributes.fromDWORD(attrDatas.dwFileAttributes)
    result.infos.size = attrDatas.nFileSizeLow + (cast(u64) attrDatas.nFileSizeHigh << 32)

    var fileTime: SYSTEMTIME
    FileTimeToSystemTime(&attrDatas.ftCreationTime, &fileTime)
    result.infos.creationTime.setSYSTEMTIME(fileTime)
    FileTimeToSystemTime(&attrDatas.ftLastAccessTime, &fileTime)
    result.infos.lastAccessTime.setSYSTEMTIME(fileTime)
    FileTimeToSystemTime(&attrDatas.ftLastWriteTime, &fileTime)
    result.infos.lastWriteTime.setSYSTEMTIME(fileTime)

    return result
}
