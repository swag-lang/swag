#include "pch.h"
#ifdef WIN32
#include "ResUpdateWin32.h"

#pragma warning(push, 0)

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

bool ResUpdateWin32::setIcon(const WCHAR* path)
{
    const LANGID&             langId     = 1033;
    const UINT                iconBundle = 0;
    std::unique_ptr<ICONVAL>& pIcon      = iconBundleMap[langId].iconBundles[iconBundle];
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

bool ResUpdateWin32::commit()
{
    if (module == nullptr)
        return false;
    FreeLibrary(module);
    module = nullptr;

    //ScopedResourceUpdater ru(filename.c_str(), false);
    auto handle = BeginUpdateResourceW(filename.c_str(), TRUE);
    
    //if (ru.get() == nullptr)
    //    return false;

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
            if (!icon.grpHeader.empty())
            {
                if (!UpdateResourceW(handle, RT_GROUP_ICON, MAKEINTRESOURCEW(bundleId), langId, icon.grpHeader.data(), icon.grpHeader.size()))
                    break;

                for (size_t i = 0; i < icon.header.count; ++i)
                {
                    if (!UpdateResourceW(handle, RT_ICON, MAKEINTRESOURCEW(i + 1), langId, icon.images[i].data(), icon.images[i].size()))
                        break;
                }

                for (size_t i = icon.header.count; i < maxIconId; ++i)
                {
                    if (!UpdateResourceW(handle, RT_ICON, MAKEINTRESOURCEW(i + 1), langId, nullptr, 0))
                        break;
                }
            }
        }
    }

    const BOOL bResult  = EndUpdateResourceW(handle, FALSE);
    return bResult ? true : false;
}

#endif