#include "pch.h"
#ifdef WIN32
#include "Os/Os.h"

namespace OS
{
    // ReSharper disable once CppInconsistentNaming
    struct ICONENTRY
    {
        BYTE  width;
        BYTE  height;
        BYTE  colorCount;
        BYTE  reserved;
        WORD  planes;
        WORD  bitCount;
        DWORD bytesInRes;
        DWORD imageOffset;
    };

    // ReSharper disable once CppInconsistentNaming
    struct ICONHEADER
    {
        WORD                   reserved;
        WORD                   type;
        WORD                   count;
        std::vector<ICONENTRY> entries;
    };

    // ReSharper disable once CppInconsistentNaming
    struct ICONVAL
    {
        ICONHEADER                     header;
        std::vector<std::vector<BYTE>> images;
        std::vector<BYTE>              grpHeader;
    };

#pragma pack(push, 2)
    // ReSharper disable once CppInconsistentNaming
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

    // ReSharper disable once CppInconsistentNaming
    struct GRPICONHEADER
    {
        WORD         reserved;
        WORD         type;
        WORD         count;
        GRPICONENTRY entries[1];
    };
#pragma pack(pop)

    bool patchIcon(const std::wstring& filename, const std::wstring& path, Utf8& error)
    {
        const auto file = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (file == INVALID_HANDLE_VALUE)
        {
            error = "cannot open icon file";
            return false;
        }

        DWORD       bytes;
        ICONVAL     icon;
        ICONHEADER& header = icon.header;
        if (!ReadFile(file, &header, 3 * sizeof(WORD), &bytes, nullptr))
        {
            CloseHandle(file);
            error = "cannot read icon header";
            return false;
        }

        if (header.reserved != 0 || header.type != 1)
        {
            CloseHandle(file);
            error = "reserved header is not 0 or image type is not icon";
            return false;
        }

        header.entries.resize(header.count);
        if (!ReadFile(file, header.entries.data(), header.count * sizeof(ICONENTRY), &bytes, nullptr))
        {
            CloseHandle(file);
            error = "cannot read icon metadata";
            return false;
        }

        icon.images.resize(header.count);
        for (size_t i = 0; i < header.count; ++i)
        {
            icon.images[i].resize(header.entries[i].bytesInRes);
            SetFilePointer(file, static_cast<LONG>(header.entries[i].imageOffset), nullptr, FILE_BEGIN);
            if (!ReadFile(file, icon.images[i].data(), static_cast<DWORD>(icon.images[i].size()), &bytes, nullptr))
            {
                CloseHandle(file);
                error = "cannot read icon data";
                return false;
            }
        }

        CloseHandle(file);

        icon.grpHeader.resize(3 * sizeof(WORD) + header.count * sizeof(GRPICONENTRY));
        const auto pGrpHeader = reinterpret_cast<GRPICONHEADER*>(icon.grpHeader.data());
        pGrpHeader->reserved  = 0;
        pGrpHeader->type      = 1;
        pGrpHeader->count     = header.count;
        for (WORD i = 0; i < header.count; ++i)
        {
            GRPICONENTRY* entry = pGrpHeader->entries + i;
            entry->bitCount     = 0;
            entry->bytesInRes   = header.entries[i].bitCount;
            entry->bytesInRes2  = static_cast<WORD>(header.entries[i].bytesInRes);
            entry->colourCount  = header.entries[i].colorCount;
            entry->height       = header.entries[i].height;
            entry->id           = i + 1;
            entry->planes       = static_cast<BYTE>(header.entries[i].planes);
            entry->reserved     = header.entries[i].reserved;
            entry->width        = header.entries[i].width;
            entry->reserved2    = 0;
        }

        const auto handle = BeginUpdateResourceW(filename.c_str(), TRUE);

        bool result0 = true;
        if (!UpdateResourceW(handle, RT_GROUP_ICON, MAKEINTRESOURCEW(0), 1033, icon.grpHeader.data(), static_cast<DWORD>(icon.grpHeader.size())))
            result0 = false;

        for (size_t i = 0; i < icon.header.count; ++i)
        {
            if (!UpdateResourceW(handle, RT_ICON, MAKEINTRESOURCEW(i + 1), 1033, icon.images[i].data(), static_cast<DWORD>(icon.images[i].size())))
                result0 = false;
        }

        const BOOL result1 = EndUpdateResourceW(handle, FALSE);
        return result0 && result1;
    }
}

#endif
