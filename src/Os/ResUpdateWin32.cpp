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
    module = LoadLibraryExW(filename, nullptr, DONT_RESOLVE_DLL_REFERENCES | LOAD_LIBRARY_AS_DATAFILE);
    if (module == nullptr)
        return false;
    this->filename = filename;
    return true;
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
        return setIcon(path, langId, kDefaultIconBundle);
    const UINT iconBundle = iconBundleMap[langId].iconBundles.begin()->first;
    return setIcon(path, langId, iconBundle);
}

bool ResUpdateWin32::setIcon(const WCHAR* path)
{
    const LANGID langId = iconBundleMap.empty() ? kLangEnUs : iconBundleMap.begin()->first;
    return setIcon(path, langId);
}

bool ResUpdateWin32::commit()
{
    if (module == nullptr)
        return false;
    FreeLibrary(module);
    module = nullptr;

    ScopedResourceUpdater ru(filename.c_str(), false);
    if (ru.get() == nullptr)
        return false;

    for (const auto& iLangIconInfoPair : iconBundleMap)
    {
        const auto langId    = iLangIconInfoPair.first;
        const auto maxIconId = iLangIconInfoPair.second.maxIconId;
        for (const auto& iNameBundlePair : iLangIconInfoPair.second.iconBundles)
        {
            const UINT                      bundleId = iNameBundlePair.first;
            const std::unique_ptr<ICONVAL>& pIcon    = iNameBundlePair.second;
            if (!pIcon)
                continue;

            auto& icon = *pIcon;
            if (icon.grpHeader.size() > 0)
            {
                if (!UpdateResourceW(ru.get(), RT_GROUP_ICON, MAKEINTRESOURCEW(bundleId), langId, icon.grpHeader.data(), icon.grpHeader.size()))
                    return false;

                for (size_t i = 0; i < icon.header.count; ++i)
                {
                    if (!UpdateResourceW(ru.get(), RT_ICON, MAKEINTRESOURCEW(i + 1), langId, icon.images[i].data(), icon.images[i].size()))
                        return false;
                }

                for (size_t i = icon.header.count; i < maxIconId; ++i)
                {
                    if (!UpdateResourceW(ru.get(), RT_ICON, MAKEINTRESOURCEW(i + 1), langId, nullptr, 0))
                        return false;
                }
            }
        }
    }

    return ru.commit();
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
