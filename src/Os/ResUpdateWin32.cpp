#include "pch.h"
#ifdef WIN32
#include "ResUpdateWin32.h"
#include <atlstr.h>

#pragma warning(push, 0)

struct ScopedResourceUpdater
{
           ScopedResourceUpdater(const WCHAR* filename, bool deleteOld);
    ~      ScopedResourceUpdater();
    HANDLE get() const;
    bool   commit();
    bool   endUpdate(bool doesCommit) const;

    HANDLE handle;
    bool   commited = false;
};

#pragma pack(push, 2)
struct GRPICONENTRY
{
    BYTE width;
    BYTE height;
    BYTE colourCount;
    BYTE reserved;
    BYTE planes;
    BYTE bitCount;
    WORD bytesInRes;
    WORD bytesInRes2;
    WORD reserved2;
    WORD id;
};

struct GRPICONHEADER
{
    WORD         reserved;
    WORD         type;
    WORD         count;
    GRPICONENTRY entries[1];
};
#pragma pack(pop)

#pragma pack(push, 1)
struct VS_VERSION_HEADER
{
    WORD wLength;
    WORD wValueLength;
    WORD wType;
};

struct VS_VERSION_STRING
{
    VS_VERSION_HEADER Header;
    WCHAR             szKey[1];
};

struct VS_VERSION_ROOT_INFO
{
    WCHAR            szKey[16];
    WORD             Padding1[1];
    VS_FIXEDFILEINFO Info;
};

struct VS_VERSION_ROOT
{
    VS_VERSION_HEADER    Header;
    VS_VERSION_ROOT_INFO Info;
};
#pragma pack(pop)

// The default en-us LANGID.
LANGID kLangEnUs          = 1033;
LANGID kCodePageEnUs      = 1200;
UINT   kDefaultIconBundle = 0;

template<typename T>
T round1(T value, int modula = 4)
{
    return value + ((value % modula > 0) ? (modula - value % modula) : 0);
}

std::wstring ReadFileToString(const wchar_t* filename)
{
    std::wifstream wif(filename);
    wif.imbue(std::locale(std::locale::empty(), new std::codecvt_utf8<wchar_t>));
    std::wstringstream wss;
    wss << wif.rdbuf();
    return wss.str();
}

class ScopedFile
{
public:
    ScopedFile(const WCHAR* path) :
        file_(CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr))
    {
    }
    ~ScopedFile() { CloseHandle(file_); }

    operator HANDLE() { return file_; }

private:
    HANDLE file_;
};

struct VersionStampValue
{
    WORD                           valueLength = 0; // stringfileinfo, stringtable: 0; string: Value size in WORD; var: Value size in bytes
    WORD                           type        = 0; // 0: binary data; 1: text data
    std::wstring                   key;             // stringtable: 8-digit hex stored as UTF-16 (hiword: hi6: sublang, lo10: majorlang; loword: code page); must include zero words to align next member on 32-bit boundary
    std::vector<BYTE>              value;           // string: zero-terminated string; var: array of language & code page ID pairs
    std::vector<VersionStampValue> children;

    size_t            GetLength() const;
    std::vector<BYTE> Serialize() const;
};

VersionInfo::VersionInfo()
{
    fillDefaultData();
}

VersionInfo::VersionInfo(HMODULE hModule, WORD languageId)
{
    HRSRC hRsrc = FindResourceExW(hModule, RT_VERSION, MAKEINTRESOURCEW(1), languageId);

    if (hRsrc == nullptr)
    {
        throw std::system_error(GetLastError(), std::system_category());
    }

    HGLOBAL hGlobal = LoadResource(hModule, hRsrc);
    if (hGlobal == nullptr)
    {
        throw std::system_error(GetLastError(), std::system_category());
    }

    void* p = LockResource(hGlobal);
    if (p == nullptr)
    {
        throw std::system_error(GetLastError(), std::system_category());
    }

    DWORD size = SizeofResource(hModule, hRsrc);
    if (size == 0)
    {
        throw std::system_error(GetLastError(), std::system_category());
    }

    deserializeVersionInfo(static_cast<BYTE*>(p), size);
    fillDefaultData();
}

bool VersionInfo::hasFixedFileInfo() const
{
    return fixedFileInfo.dwSignature == 0xFEEF04BD;
}

VS_FIXEDFILEINFO& VersionInfo::getFixedFileInfo()
{
    return fixedFileInfo;
}

const VS_FIXEDFILEINFO& VersionInfo::getFixedFileInfo() const
{
    return fixedFileInfo;
}

void VersionInfo::setFixedFileInfo(const VS_FIXEDFILEINFO& value)
{
    fixedFileInfo = value;
}

std::vector<BYTE> VersionInfo::serialize() const
{
    VersionStampValue versionInfo;
    versionInfo.key  = L"VS_VERSION_INFO";
    versionInfo.type = 0;

    if (hasFixedFileInfo())
    {
        auto size               = sizeof(VS_FIXEDFILEINFO);
        versionInfo.valueLength = size;

        auto& dst = versionInfo.value;
        dst.resize(size);

        memcpy(&dst[0], &getFixedFileInfo(), size);
    }

    {
        VersionStampValue stringFileInfo;
        stringFileInfo.key         = L"StringFileInfo";
        stringFileInfo.type        = 1;
        stringFileInfo.valueLength = 0;

        for (const auto& iTable : stringTables)
        {
            VersionStampValue stringTableRaw;
            stringTableRaw.type        = 1;
            stringTableRaw.valueLength = 0;

            {
                auto&              translate = iTable.encoding;
                std::wstringstream ss;
                ss << std::hex << std::setw(8) << std::setfill(L'0') << (translate.wLanguage << 16 | translate.wCodePage);
                stringTableRaw.key = ss.str();
            }

            for (const auto& iString : iTable.strings)
            {
                const auto& stringValue          = iString.second;
                auto        strLenNullTerminated = stringValue.length() + 1;

                VersionStampValue stringRaw;
                stringRaw.type        = 1;
                stringRaw.key         = iString.first;
                stringRaw.valueLength = strLenNullTerminated;

                auto  size = strLenNullTerminated * sizeof(WCHAR);
                auto& dst  = stringRaw.value;
                dst.resize(size);

                auto src = stringValue.c_str();

                memcpy(&dst[0], src, size);

                stringTableRaw.children.push_back(std::move(stringRaw));
            }

            stringFileInfo.children.push_back(std::move(stringTableRaw));
        }

        versionInfo.children.push_back(std::move(stringFileInfo));
    }

    {
        VersionStampValue varFileInfo;
        varFileInfo.key         = L"VarFileInfo";
        varFileInfo.type        = 1;
        varFileInfo.valueLength = 0;

        {
            VersionStampValue varRaw;
            varRaw.key  = L"Translation";
            varRaw.type = 0;

            {
                auto  newValueSize = sizeof(DWORD);
                auto& dst          = varRaw.value;
                dst.resize(supportedTranslations.size() * newValueSize);

                for (auto iVar = 0; iVar < supportedTranslations.size(); ++iVar)
                {
                    auto& translate = supportedTranslations[iVar];
                    auto  var       = DWORD(translate.wCodePage) << 16 | translate.wLanguage;
                    memcpy(&dst[iVar * newValueSize], &var, newValueSize);
                }

                varRaw.valueLength = varRaw.value.size();
            }

            varFileInfo.children.push_back(std::move(varRaw));
        }

        versionInfo.children.push_back(std::move(varFileInfo));
    }

    return std::move(versionInfo.Serialize());
}

void VersionInfo::fillDefaultData()
{
    if (stringTables.empty())
    {
        Translate enUsTranslate = {kLangEnUs, kCodePageEnUs};
        stringTables.push_back({enUsTranslate});
        supportedTranslations.push_back(enUsTranslate);
    }
    if (!hasFixedFileInfo())
    {
        fixedFileInfo             = {0};
        fixedFileInfo.dwSignature = 0xFEEF04BD;
        fixedFileInfo.dwFileType  = VFT_APP;
    }
}

void VersionInfo::deserializeVersionInfo(const BYTE* pData, size_t size)
{
    auto pVersionInfo      = reinterpret_cast<const VS_VERSION_ROOT*>(pData);
    WORD fixedFileInfoSize = pVersionInfo->Header.wValueLength;

    if (fixedFileInfoSize > 0)
        setFixedFileInfo(pVersionInfo->Info.Info);

    const BYTE* fixedFileInfoEndOffset    = reinterpret_cast<const BYTE*>(&pVersionInfo->Info.szKey) + (wcslen(pVersionInfo->Info.szKey) + 1) * sizeof(WCHAR) + fixedFileInfoSize;
    const BYTE* pVersionInfoChildren      = reinterpret_cast<const BYTE*>(round1(reinterpret_cast<ptrdiff_t>(fixedFileInfoEndOffset)));
    size_t      versionInfoChildrenOffset = pVersionInfoChildren - pData;
    size_t      versionInfoChildrenSize   = pVersionInfo->Header.wLength - versionInfoChildrenOffset;

    const auto childrenEndOffset = pVersionInfoChildren + versionInfoChildrenSize;
    const auto resourceEndOffset = pData + size;
    for (auto p = pVersionInfoChildren; p < childrenEndOffset && p < resourceEndOffset;)
    {
        auto pKey                 = reinterpret_cast<const VS_VERSION_STRING*>(p)->szKey;
        auto versionInfoChildData = getChildrenData(p);
        if (wcscmp(pKey, L"StringFileInfo") == 0)
        {
            deserializeVersionStringFileInfo(versionInfoChildData.first, versionInfoChildData.second, stringTables);
        }
        else if (wcscmp(pKey, L"VarFileInfo") == 0)
        {
            deserializeVarFileInfo(versionInfoChildData.first, supportedTranslations);
        }

        p += round1(reinterpret_cast<const VS_VERSION_STRING*>(p)->Header.wLength);
    }
}

VersionStringTable VersionInfo::deserializeVersionStringTable(const BYTE* tableData)
{
    auto strings            = getChildrenData(tableData);
    auto stringTable        = reinterpret_cast<const VS_VERSION_STRING*>(tableData);
    auto end_ptr            = const_cast<WCHAR*>(stringTable->szKey + (8 * sizeof(WCHAR)));
    auto langIdCodePagePair = static_cast<DWORD>(wcstol(stringTable->szKey, &end_ptr, 16));

    VersionStringTable tableEntry;

    // unicode string of 8 hex digits
    tableEntry.encoding.wLanguage = langIdCodePagePair >> 16;
    tableEntry.encoding.wCodePage = langIdCodePagePair;

    for (auto posStrings = 0U; posStrings < strings.second;)
    {
        const auto stringEntry = reinterpret_cast<const VS_VERSION_STRING* const>(strings.first + posStrings);
        const auto stringData  = getChildrenData(strings.first + posStrings);
        tableEntry.strings.push_back(std::pair<std::wstring, std::wstring>(stringEntry->szKey, std::wstring(reinterpret_cast<const WCHAR* const>(stringData.first), stringEntry->Header.wValueLength)));

        posStrings += round1(stringEntry->Header.wLength);
    }

    return tableEntry;
}

void VersionInfo::deserializeVersionStringFileInfo(const BYTE* offset, size_t length, std::vector<VersionStringTable>& stringTables)
{
    for (auto posStringTables = 0U; posStringTables < length;)
    {
        auto stringTableEntry = deserializeVersionStringTable(offset + posStringTables);
        stringTables.push_back(stringTableEntry);
        posStringTables += round1(reinterpret_cast<const VS_VERSION_STRING*>(offset + posStringTables)->Header.wLength);
    }
}

void VersionInfo::deserializeVarFileInfo(const unsigned char* offset, std::vector<Translate>& translations)
{
    const auto translatePairs = getChildrenData(offset);

    const auto top = reinterpret_cast<const DWORD* const>(translatePairs.first);
    for (auto pTranslatePair = top; pTranslatePair < top + translatePairs.second; pTranslatePair += sizeof(DWORD))
    {
        auto      codePageLangIdPair = *pTranslatePair;
        Translate translate;
        translate.wLanguage = codePageLangIdPair;
        translate.wCodePage = codePageLangIdPair >> 16;
        translations.push_back(translate);
    }
}

OffsetLengthPair VersionInfo::getChildrenData(const BYTE* entryData)
{
    auto entry          = reinterpret_cast<const VS_VERSION_STRING*>(entryData);
    auto headerOffset   = entryData;
    auto headerSize     = sizeof(VS_VERSION_HEADER);
    auto keySize        = (wcslen(entry->szKey) + 1) * sizeof(WCHAR);
    auto childrenOffset = round1(headerSize + keySize);

    auto pChildren    = headerOffset + childrenOffset;
    auto childrenSize = entry->Header.wLength - childrenOffset;
    return OffsetLengthPair(pChildren, childrenSize);
}

size_t VersionStampValue::GetLength() const
{
    size_t bytes = sizeof(VS_VERSION_HEADER);
    bytes += static_cast<size_t>(key.length() + 1) * sizeof(WCHAR);
    if (!value.empty())
        bytes = round1(bytes) + value.size();
    for (const auto& child : children)
        bytes = round1(bytes) + static_cast<size_t>(child.GetLength());
    return bytes;
}

std::vector<BYTE> VersionStampValue::Serialize() const
{
    std::vector<BYTE> data = std::vector<BYTE>(GetLength());

    size_t offset = 0;

    VS_VERSION_HEADER header = {static_cast<WORD>(data.size()), valueLength, type};
    memcpy(&data[offset], &header, sizeof(header));
    offset += sizeof(header);

    auto keySize = static_cast<size_t>(key.length() + 1) * sizeof(WCHAR);
    memcpy(&data[offset], key.c_str(), keySize);
    offset += keySize;

    if (!value.empty())
    {
        offset = round1(offset);
        memcpy(&data[offset], &value[0], value.size());
        offset += value.size();
    }

    for (const auto& child : children)
    {
        offset                        = round1(offset);
        size_t            childLength = child.GetLength();
        std::vector<BYTE> src         = child.Serialize();
        memcpy(&data[offset], &src[0], childLength);
        offset += childLength;
    }

    return std::move(data);
}

ResUpdateWin32::ResUpdateWin32() :
    module(nullptr)
{
}

ResUpdateWin32::~ResUpdateWin32()
{
    if (module != nullptr)
    {
        FreeLibrary(module);
        module = nullptr;
    }
}

bool ResUpdateWin32::load(const WCHAR* filename)
{
    wchar_t abspath[MAX_PATH] = {0};
    if (_wfullpath(abspath, filename, MAX_PATH))
        module = LoadLibraryExW(abspath, nullptr, DONT_RESOLVE_DLL_REFERENCES | LOAD_LIBRARY_AS_DATAFILE);
    else
        module = LoadLibraryExW(filename, nullptr, DONT_RESOLVE_DLL_REFERENCES | LOAD_LIBRARY_AS_DATAFILE);

    if (module == nullptr)
    {
        return false;
    }

    this->filename = filename;

    EnumResourceNamesW(module, RT_STRING, onEnumResourceName, reinterpret_cast<LONG_PTR>(this));
    EnumResourceNamesW(module, RT_VERSION, onEnumResourceName, reinterpret_cast<LONG_PTR>(this));
    EnumResourceNamesW(module, RT_GROUP_ICON, onEnumResourceName, reinterpret_cast<LONG_PTR>(this));
    EnumResourceNamesW(module, RT_ICON, onEnumResourceName, reinterpret_cast<LONG_PTR>(this));
    EnumResourceNamesW(module, RT_MANIFEST, onEnumResourceManifest, reinterpret_cast<LONG_PTR>(this));
    EnumResourceNamesW(module, RT_RCDATA, onEnumResourceName, reinterpret_cast<LONG_PTR>(this));

    return true;
}

bool ResUpdateWin32::setExecutionLevel(const WCHAR* value)
{
    executionLevel = value;
    return true;
}

bool ResUpdateWin32::isExecutionLevelSet()
{
    return !executionLevel.empty();
}

bool ResUpdateWin32::setApplicationManifest(const WCHAR* value)
{
    applicationManifestPath = value;
    return true;
}

bool ResUpdateWin32::isApplicationManifestSet()
{
    return !applicationManifestPath.empty();
}

bool ResUpdateWin32::setVersionString(WORD languageId, const WCHAR* name, const WCHAR* value)
{
    std::wstring nameStr(name);
    std::wstring valueStr(value);

    auto& stringTables = versionStampMap[languageId].stringTables;
    for (auto j = stringTables.begin(); j != stringTables.end(); ++j)
    {
        auto& stringPairs = j->strings;
        for (auto k = stringPairs.begin(); k != stringPairs.end(); ++k)
        {
            if (k->first == nameStr)
            {
                k->second = valueStr;
                return true;
            }
        }

        // Not found, append one for all tables.
        stringPairs.push_back(VersionString(nameStr, valueStr));
    }

    return true;
}

bool ResUpdateWin32::setVersionString(const WCHAR* name, const WCHAR* value)
{
    LANGID langId = versionStampMap.empty() ? kLangEnUs
                                            : versionStampMap.begin()->first;
    return setVersionString(langId, name, value);
}

const WCHAR* ResUpdateWin32::getVersionString(WORD languageId, const WCHAR* name)
{
    std::wstring nameStr(name);

    const auto& stringTables = versionStampMap[languageId].stringTables;
    for (const auto& j : stringTables)
    {
        const auto& stringPairs = j.strings;
        for (const auto& k : stringPairs)
        {
            if (k.first == nameStr)
            {
                return k.second.c_str();
            }
        }
    }

    return nullptr;
}

const WCHAR* ResUpdateWin32::getVersionString(const WCHAR* name)
{
    if (versionStampMap.empty())
    {
        return nullptr;
    }
    else
    {
        return getVersionString(versionStampMap.begin()->first, name);
    }
}

bool ResUpdateWin32::setProductVersion(WORD languageId, UINT id, unsigned short v1, unsigned short v2, unsigned short v3, unsigned short v4)
{
    VersionInfo& versionInfo = versionStampMap[languageId];
    if (!versionInfo.hasFixedFileInfo())
    {
        return false;
    }

    VS_FIXEDFILEINFO& root = versionInfo.getFixedFileInfo();

    root.dwProductVersionMS = v1 << 16 | v2;
    root.dwProductVersionLS = v3 << 16 | v4;

    return true;
}

bool ResUpdateWin32::setProductVersion(unsigned short v1, unsigned short v2, unsigned short v3, unsigned short v4)
{
    LANGID langId = versionStampMap.empty() ? kLangEnUs
                                            : versionStampMap.begin()->first;
    return setProductVersion(langId, 1, v1, v2, v3, v4);
}

bool ResUpdateWin32::setFileVersion(WORD languageId, UINT id, unsigned short v1, unsigned short v2, unsigned short v3, unsigned short v4)
{
    VersionInfo& versionInfo = versionStampMap[languageId];
    if (!versionInfo.hasFixedFileInfo())
    {
        return false;
    }

    VS_FIXEDFILEINFO& root = versionInfo.getFixedFileInfo();

    root.dwFileVersionMS = v1 << 16 | v2;
    root.dwFileVersionLS = v3 << 16 | v4;
    return true;
}

bool ResUpdateWin32::setFileVersion(unsigned short v1, unsigned short v2, unsigned short v3, unsigned short v4)
{
    LANGID langId = versionStampMap.empty() ? kLangEnUs
                                            : versionStampMap.begin()->first;
    return setFileVersion(langId, 1, v1, v2, v3, v4);
}

bool ResUpdateWin32::changeString(WORD languageId, UINT id, const WCHAR* value)
{
    StringTable& table = stringTableMap[languageId];

    UINT blockId = id / 16;
    if (table.find(blockId) == table.end())
    {
        // Fill the table until we reach the block.
        for (size_t i = table.size(); i <= blockId; ++i)
        {
            table[i] = std::vector<std::wstring>(16);
        }
    }

    assert(table[blockId].size() == 16);
    UINT blockIndex            = id % 16;
    table[blockId][blockIndex] = value;

    return true;
}

bool ResUpdateWin32::changeString(UINT id, const WCHAR* value)
{
    LANGID langId = stringTableMap.empty() ? kLangEnUs
                                           : stringTableMap.begin()->first;
    return changeString(langId, id, value);
}

bool ResUpdateWin32::changeRcData(UINT id, const WCHAR* pathToResource)
{
    auto rcDataLngPairIt = std::find_if(rcDataLngMap.begin(), rcDataLngMap.end(), [=](const auto& rcDataLngPair) {
        return rcDataLngPair.second.find(id) != rcDataLngPair.second.end();
    });

    if (rcDataLngPairIt == rcDataLngMap.end())
    {
        fprintf(stderr, "Cannot find RCDATA with id '%u'\n", id);
        return false;
    }

    wchar_t    abspath[MAX_PATH] = {0};
    const auto filePath          = _wfullpath(abspath, pathToResource, MAX_PATH) ? abspath : pathToResource;
    ScopedFile newRcDataFile(filePath);
    if (newRcDataFile == INVALID_HANDLE_VALUE)
    {
        fprintf(stderr, "Cannot open new data file '%ws'\n", filePath);
        return false;
    }

    const auto dwFileSize = GetFileSize(newRcDataFile, nullptr);
    if (dwFileSize == INVALID_FILE_SIZE)
    {
        fprintf(stderr, "Cannot get file size for '%ws'\n", filePath);
        return false;
    }

    auto& rcData = rcDataLngPairIt->second[id];
    rcData.clear();
    rcData.resize(dwFileSize);

    DWORD dwBytesRead{0};
    if (!ReadFile(newRcDataFile, rcData.data(), dwFileSize, &dwBytesRead, nullptr))
    {
        fprintf(stderr, "Cannot read file '%ws'\n", filePath);
        return false;
    }

    return true;
}

const WCHAR* ResUpdateWin32::getString(WORD languageId, UINT id)
{
    StringTable& table = stringTableMap[languageId];

    UINT blockId = id / 16;
    if (table.find(blockId) == table.end())
    {
        // Fill the table until we reach the block.
        for (size_t i = table.size(); i <= blockId; ++i)
        {
            table[i] = std::vector<std::wstring>(16);
        }
    }

    assert(table[blockId].size() == 16);
    UINT blockIndex = id % 16;

    return table[blockId][blockIndex].c_str();
}

const WCHAR* ResUpdateWin32::getString(UINT id)
{
    LANGID langId = stringTableMap.empty() ? kLangEnUs
                                           : stringTableMap.begin()->first;
    return getString(langId, id);
}

bool ResUpdateWin32::setIcon(const WCHAR* path, const LANGID& langId, UINT iconBundle)
{
    std::unique_ptr<ICONVAL>& pIcon = iconBundleMap[langId].iconBundles[iconBundle];
    if (!pIcon)
        pIcon = std::make_unique<ICONVAL>();

    auto& icon = *pIcon;
    DWORD bytes;

    ScopedFile file(path);
    if (file == INVALID_HANDLE_VALUE)
    {
        error = "cannot open icon file";
        return false;
    }

    ICONHEADER& header = icon.header;
    if (!ReadFile(file, &header, 3 * sizeof(WORD), &bytes, nullptr))
    {
        error = "cannot read icon header";
        return false;
    }

    if (header.reserved != 0 || header.type != 1)
    {
        error = "reserved header is not 0 or image type is not icon";
        return false;
    }

    header.entries.resize(header.count);
    if (!ReadFile(file, header.entries.data(), header.count * sizeof(ICONENTRY), &bytes, nullptr))
    {
        error = "cannot read icon metadata";
        return false;
    }

    icon.images.resize(header.count);
    for (size_t i = 0; i < header.count; ++i)
    {
        icon.images[i].resize(header.entries[i].bytesInRes);
        SetFilePointer(file, header.entries[i].imageOffset, nullptr, FILE_BEGIN);
        if (!ReadFile(file, icon.images[i].data(), icon.images[i].size(), &bytes, nullptr))
        {
            error = "cannot read icon data";
            return false;
        }
    }

    icon.grpHeader.resize(3 * sizeof(WORD) + header.count * sizeof(GRPICONENTRY));
    GRPICONHEADER* pGrpHeader = reinterpret_cast<GRPICONHEADER*>(icon.grpHeader.data());
    pGrpHeader->reserved      = 0;
    pGrpHeader->type          = 1;
    pGrpHeader->count         = header.count;
    for (size_t i = 0; i < header.count; ++i)
    {
        GRPICONENTRY* entry = pGrpHeader->entries + i;
        entry->bitCount     = 0;
        entry->bytesInRes   = header.entries[i].bitCount;
        entry->bytesInRes2  = header.entries[i].bytesInRes;
        entry->colourCount  = header.entries[i].colorCount;
        entry->height       = header.entries[i].height;
        entry->id           = i + 1;
        entry->planes       = header.entries[i].planes;
        entry->reserved     = header.entries[i].reserved;
        entry->width        = header.entries[i].width;
        entry->reserved2    = 0;
    }

    return true;
}

bool ResUpdateWin32::setIcon(const WCHAR* path, const LANGID& langId)
{
    if (iconBundleMap[langId].iconBundles.empty())
    {
        return setIcon(path, langId, kDefaultIconBundle);
    }
    UINT iconBundle = iconBundleMap[langId].iconBundles.begin()->first;
    return setIcon(path, langId, iconBundle);
}

bool ResUpdateWin32::setIcon(const WCHAR* path)
{
    LANGID langId = iconBundleMap.empty() ? kLangEnUs
                                          : iconBundleMap.begin()->first;
    return setIcon(path, langId);
}

bool ResUpdateWin32::commit()
{
    if (module == nullptr)
    {
        return false;
    }
    FreeLibrary(module);
    module = nullptr;

    ScopedResourceUpdater ru(filename.c_str(), false);
    if (ru.get() == nullptr)
    {
        return false;
    }

    // update version info.
    for (const auto& i : versionStampMap)
    {
        LANGID            langId = i.first;
        std::vector<BYTE> out    = i.second.serialize();

        if (!UpdateResourceW(ru.get(), RT_VERSION, MAKEINTRESOURCEW(1), langId,
                             &out[0], static_cast<DWORD>(out.size())))
        {
            return false;
        }
    }

    // update the execution level
    if (applicationManifestPath.empty() && !executionLevel.empty())
    {
        // string replace with requested executionLevel
        std::wstring::size_type pos = 0u;
        while ((pos = manifestString.find(originalExecutionLevel, pos)) != std::string::npos)
        {
            manifestString.replace(pos, originalExecutionLevel.length(), executionLevel);
            pos += executionLevel.length();
        }

        // clean old padding and add new padding, ensuring that the size is a multiple of 4
        std::wstring::size_type padPos = manifestString.find(L"</assembly>");
        // trim anything after the </assembly>, 11 being the length of </assembly> (ie, remove old padding)
        std::wstring trimmedStr = manifestString.substr(0, padPos + 11);
        std::wstring padding    = L"\n<!--Padding to make filesize even multiple of 4 X -->";

        int offset = (trimmedStr.length() + padding.length()) % 4;
        // multiple X by the number in offset
        pos = 0u;
        for (int posCount = 0; posCount < offset; posCount = posCount + 1)
        {
            if ((pos = padding.find(L"X", pos)) != std::string::npos)
            {
                padding.replace(pos, 1, L"XX");
                pos += executionLevel.length();
            }
        }

        // convert the wchar back into char, so that it encodes correctly for Windows to read the XML.
        std::wstring                                              stringSectionW = trimmedStr + padding;
        std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
        std::string                                               stringSection = converter.to_bytes(stringSectionW);

        if (!UpdateResourceW(ru.get(), RT_MANIFEST, MAKEINTRESOURCEW(1),
                             kLangEnUs, // this is hardcoded at 1033, ie, en-us, as that is what RT_MANIFEST default uses
                             &stringSection.at(0), sizeof(char) * stringSection.size()))
        {
            return false;
        }
    }

    // load file contents and replace the manifest
    if (!applicationManifestPath.empty())
    {
        std::wstring fileContents = ReadFileToString(applicationManifestPath.c_str());

        // clean old padding and add new padding, ensuring that the size is a multiple of 4
        std::wstring::size_type padPos = fileContents.find(L"</assembly>");
        // trim anything after the </assembly>, 11 being the length of </assembly> (ie, remove old padding)
        std::wstring trimmedStr = fileContents.substr(0, padPos + 11);
        std::wstring padding    = L"\n<!--Padding to make filesize even multiple of 4 X -->";

        int offset = (trimmedStr.length() + padding.length()) % 4;
        // multiple X by the number in offset
        std::wstring::size_type pos = 0u;
        for (int posCount = 0; posCount < offset; posCount = posCount + 1)
        {
            if ((pos = padding.find(L"X", pos)) != std::string::npos)
            {
                padding.replace(pos, 1, L"XX");
                pos += executionLevel.length();
            }
        }

        // convert the wchar back into char, so that it encodes correctly for Windows to read the XML.
        std::wstring                                              stringSectionW = fileContents + padding;
        std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
        std::string                                               stringSection = converter.to_bytes(stringSectionW);

        if (!UpdateResourceW(ru.get(), RT_MANIFEST, MAKEINTRESOURCEW(1),
                             kLangEnUs, // this is hardcoded at 1033, ie, en-us, as that is what RT_MANIFEST default uses
                             &stringSection.at(0), sizeof(char) * stringSection.size()))
        {
            return false;
        }
    }

    // update string table.
    for (const auto& i : stringTableMap)
    {
        for (const auto& j : i.second)
        {
            std::vector<char> stringTableBuffer;
            if (!serializeStringTable(j.second, j.first, &stringTableBuffer))
            {
                return false;
            }

            if (!UpdateResourceW(ru.get(), RT_STRING, MAKEINTRESOURCEW(j.first + 1), i.first,
                                 &stringTableBuffer[0], static_cast<DWORD>(stringTableBuffer.size())))
            {
                return false;
            }
        }
    }

    for (const auto& rcDataLangPair : rcDataLngMap)
    {
        for (const auto& rcDataMap : rcDataLangPair.second)
        {
            if (!UpdateResourceW(ru.get(), RT_RCDATA, reinterpret_cast<LPWSTR>(rcDataMap.first),
                                 rcDataLangPair.first, (LPVOID) rcDataMap.second.data(), rcDataMap.second.size()))
            {
                return false;
            }
        }
    }

    for (const auto& iLangIconInfoPair : iconBundleMap)
    {
        auto langId    = iLangIconInfoPair.first;
        auto maxIconId = iLangIconInfoPair.second.maxIconId;
        for (const auto& iNameBundlePair : iLangIconInfoPair.second.iconBundles)
        {
            UINT                            bundleId = iNameBundlePair.first;
            const std::unique_ptr<ICONVAL>& pIcon    = iNameBundlePair.second;
            if (!pIcon)
                continue;

            auto& icon = *pIcon;
            // update icon.
            if (icon.grpHeader.size() > 0)
            {
                if (!UpdateResourceW(ru.get(), RT_GROUP_ICON, MAKEINTRESOURCEW(bundleId),
                                     langId, icon.grpHeader.data(), icon.grpHeader.size()))
                {
                    return false;
                }

                for (size_t i = 0; i < icon.header.count; ++i)
                {
                    if (!UpdateResourceW(ru.get(), RT_ICON, MAKEINTRESOURCEW(i + 1),
                                         langId, icon.images[i].data(), icon.images[i].size()))
                    {
                        return false;
                    }
                }

                for (size_t i = icon.header.count; i < maxIconId; ++i)
                {
                    if (!UpdateResourceW(ru.get(), RT_ICON, MAKEINTRESOURCEW(i + 1),
                                         langId, nullptr, 0))
                    {
                        return false;
                    }
                }
            }
        }
    }

    return ru.commit();
}

bool ResUpdateWin32::serializeStringTable(const StringValues& values, UINT blockId, std::vector<char>* out)
{
    // calc total size.
    // string table is pascal string list.
    size_t size = 0;
    for (size_t i = 0; i < 16; i++)
    {
        size += sizeof(WORD);
        size += values[i].length() * sizeof(WCHAR);
    }

    out->resize(size);

    // write.
    char* pDst = &(*out)[0];
    for (size_t i = 0; i < 16; i++)
    {
        WORD length = static_cast<WORD>(values[i].length());
        memcpy(pDst, &length, sizeof(length));
        pDst += sizeof(WORD);

        if (length > 0)
        {
            WORD bytes = length * sizeof(WCHAR);
            memcpy(pDst, values[i].c_str(), bytes);
            pDst += bytes;
        }
    }

    return true;
}

// static
BOOL CALLBACK ResUpdateWin32::onEnumResourceLanguage(HANDLE hModule, LPCWSTR lpszType, LPCWSTR lpszName, WORD wIDLanguage, LONG_PTR lParam)
{
    ResUpdateWin32* instance = reinterpret_cast<ResUpdateWin32*>(lParam);
    if (IS_INTRESOURCE(lpszName) && IS_INTRESOURCE(lpszType))
    {
        switch (reinterpret_cast<ptrdiff_t>(lpszType))
        {
            case reinterpret_cast<ptrdiff_t>(RT_VERSION):
            {
                try
                {
                    instance->versionStampMap[wIDLanguage] = VersionInfo(instance->module, wIDLanguage);
                }
                catch (const std::system_error& e)
                {
                    return false;
                }
                break;
            }
            case reinterpret_cast<ptrdiff_t>(RT_STRING):
            {
                UINT  id     = reinterpret_cast<ptrdiff_t>(lpszName) - 1;
                auto& vector = instance->stringTableMap[wIDLanguage][id];
                for (size_t k = 0; k < 16; k++)
                {
                    CStringW buf;

                    buf.LoadStringW(instance->module, id * 16 + k, wIDLanguage);
                    vector.push_back(buf.GetBuffer());
                }
                break;
            }
            case reinterpret_cast<ptrdiff_t>(RT_ICON):
            {
                UINT iconId    = reinterpret_cast<ptrdiff_t>(lpszName);
                UINT maxIconId = instance->iconBundleMap[wIDLanguage].maxIconId;
                if (iconId > maxIconId)
                    maxIconId = iconId;
                break;
            }
            case reinterpret_cast<ptrdiff_t>(RT_GROUP_ICON):
            {
                UINT iconId                                              = reinterpret_cast<ptrdiff_t>(lpszName);
                instance->iconBundleMap[wIDLanguage].iconBundles[iconId] = nullptr;
                break;
            }
            case reinterpret_cast<ptrdiff_t>(RT_RCDATA):
            {
                const auto moduleHandle = HMODULE(hModule);
                HRSRC      hResInfo     = FindResource(moduleHandle, lpszName, lpszType);
                DWORD      cbResource   = SizeofResource(moduleHandle, hResInfo);
                HGLOBAL    hResData     = LoadResource(moduleHandle, hResInfo);

                const auto* pResource                      = (const BYTE*) LockResource(hResData);
                const auto  resId                          = reinterpret_cast<ptrdiff_t>(lpszName);
                instance->rcDataLngMap[wIDLanguage][resId] = std::vector<BYTE>(pResource, pResource + cbResource);

                UnlockResource(hResData);
                FreeResource(hResData);
            }
            default:
                break;
        }
    }
    return TRUE;
}

// static
BOOL CALLBACK ResUpdateWin32::onEnumResourceName(HMODULE hModule, LPCWSTR lpszType, LPWSTR lpszName, LONG_PTR lParam)
{
    EnumResourceLanguagesW(hModule, lpszType, lpszName, (ENUMRESLANGPROCW) onEnumResourceLanguage, lParam);
    return TRUE;
}

// static
// courtesy of http://stackoverflow.com/questions/420852/reading-an-applications-manifest-file
BOOL CALLBACK ResUpdateWin32::onEnumResourceManifest(HMODULE hModule, LPCTSTR lpType, LPWSTR lpName, LONG_PTR lParam)
{
    ResUpdateWin32* instance   = reinterpret_cast<ResUpdateWin32*>(lParam);
    HRSRC           hResInfo   = FindResource(hModule, lpName, lpType);
    DWORD           cbResource = SizeofResource(hModule, hResInfo);

    HGLOBAL     hResData  = LoadResource(hModule, hResInfo);
    const BYTE* pResource = (const BYTE*) LockResource(hResData);

    // FIXME(zcbenz): Do a real UTF string convertion.
    int          len = strlen(reinterpret_cast<const char*>(pResource));
    std::wstring manifestStringLocal(pResource, pResource + len);

    // FIXME(zcbenz): Strip the BOM instead of doing string search.
    size_t start = manifestStringLocal.find(L"<?xml");
    if (start > 0)
    {
        manifestStringLocal = manifestStringLocal.substr(start);
    }

    // Support alternative formatting, such as using " vs ' and level="..." on another line
    size_t found = manifestStringLocal.find(L"requestedExecutionLevel");
    size_t level = manifestStringLocal.find(L"level=\"", found);
    size_t end   = manifestStringLocal.find(L"\"", level + 7);
    if (level < 0)
    {
        level = manifestStringLocal.find(L"level=\'", found);
        end   = manifestStringLocal.find(L"\'", level + 7);
    }

    instance->originalExecutionLevel = manifestStringLocal.substr(level + 7, end - level - 7);

    // also store original manifestString
    instance->manifestString = manifestStringLocal;

    UnlockResource(hResData);
    FreeResource(hResData);

    return TRUE; // Keep going
}

ScopedResourceUpdater::ScopedResourceUpdater(const WCHAR* filename, bool deleteOld) :
    handle(BeginUpdateResourceW(filename, deleteOld))
{
}

ScopedResourceUpdater::~ScopedResourceUpdater()
{
    if (!commited)
        (void) endUpdate(false);
}

HANDLE ScopedResourceUpdater::get() const
{
    return handle;
}

bool ScopedResourceUpdater::commit()
{
    commited = true;
    return endUpdate(true);
}

bool ScopedResourceUpdater::endUpdate(bool doesCommit) const
{
    const BOOL fDiscard = doesCommit ? FALSE : TRUE;
    const BOOL bResult  = EndUpdateResourceW(handle, fDiscard);
    return bResult ? true : false;
}

#endif
