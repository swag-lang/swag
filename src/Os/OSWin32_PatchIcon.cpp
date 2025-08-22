#include "pch.h"
#ifdef WIN32
#include "Os/Os.h"
#pragma optimize("", off)

namespace
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
    // FIXED: Corrected GRPICONENTRY structure
    struct GRPICONENTRY
    {
        BYTE  width;
        BYTE  height;
        BYTE  colourCount;
        BYTE  reserved;
        WORD  planes;     // Changed from BYTE to WORD
        WORD  bitCount;   // Changed from BYTE to WORD  
        DWORD bytesInRes; // This is the full DWORD size
        WORD  id;         // Resource ID
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

#pragma pack(push, 1)
    struct VersionInfoHeader
    {
        WORD  wLength;
        WORD  wValueLength;
        WORD  wType;
        WCHAR szKey[16]; // "VS_VERSION_INFO"
    };

    struct StringInfoHeader
    {
        WORD  wLength;
        WORD  wValueLength;
        WORD  wType;
        WCHAR szKey[1]; // Variable length key
    };
#pragma pack(pop)

    struct VS_FIXEDFILEINFO_CUSTOM
    {
        DWORD dwSignature;
        DWORD dwStrucVersion;
        DWORD dwFileVersionMS;
        DWORD dwFileVersionLS;
        DWORD dwProductVersionMS;
        DWORD dwProductVersionLS;
        DWORD dwFileFlagsMask;
        DWORD dwFileFlags;
        DWORD dwFileOS;
        DWORD dwFileType;
        DWORD dwFileSubtype;
        DWORD dwFileDateMS;
        DWORD dwFileDateLS;
    };

    // Helper function to align pointer to 4-byte boundary
    BYTE* alignPointer(BYTE* ptr, size_t alignment = 4)
    {
        const uintptr_t addr    = reinterpret_cast<uintptr_t>(ptr);
        const uintptr_t aligned = (addr + alignment - 1) & ~(alignment - 1);
        return reinterpret_cast<BYTE*>(aligned);
    }

    // Helper function to write a wide string and return new position
    BYTE* writeWideString(BYTE* ptr, const std::wstring& str)
    {
        const size_t len = (str.length() + 1) * sizeof(WCHAR);
        memcpy(ptr, str.c_str(), len);
        return ptr + len;
    }

    // FIXED: Helper function to create version information structure
    std::vector<BYTE> createVersionInfo(const std::wstring& appName, const std::wstring& appDescription)
    {
        // Use the provided name/description or fallback values
        const std::wstring productName      = !appName.empty() ? appName : L"Application";
        const std::wstring fileDescription  = !appDescription.empty() ? appDescription : productName;
        const std::wstring fileVersion      = L"1.0.0.0";
        const std::wstring productVersion   = L"1.0.0.0";
        const std::wstring companyName      = L"";
        const std::wstring copyright        = L"";
        const std::wstring internalName     = productName;
        const std::wstring originalFilename = productName + L".exe";

        // Calculate required size more accurately
        size_t estimatedSize = 0;
        estimatedSize += sizeof(VersionInfoHeader);
        estimatedSize += sizeof(VS_FIXEDFILEINFO_CUSTOM);
        estimatedSize += 200; // StringFileInfo header + padding
        estimatedSize += 200; // StringTable header + padding

        // Add size for each string entry (header + key + value + padding)
        auto addStringSize = [&](const std::wstring& key, const std::wstring& value) {
            estimatedSize += sizeof(StringInfoHeader) - sizeof(WCHAR); // Header minus placeholder
            estimatedSize += (key.length() + 1) * sizeof(WCHAR);       // Key
            estimatedSize += (value.length() + 1) * sizeof(WCHAR);     // Value
            estimatedSize += 32;                                       // Padding for alignment
        };

        addStringSize(L"ProductName", productName);
        addStringSize(L"FileDescription", fileDescription);
        addStringSize(L"FileVersion", fileVersion);
        addStringSize(L"ProductVersion", productVersion);
        addStringSize(L"InternalName", internalName);
        addStringSize(L"OriginalFilename", originalFilename);
        if (!companyName.empty())
            addStringSize(L"CompanyName", companyName);
        if (!copyright.empty())
            addStringSize(L"LegalCopyright", copyright);

        std::vector<BYTE> versionData(estimatedSize, 0);
        BYTE*             ptr      = versionData.data();
        const BYTE*       startPtr = ptr;

        // Root VS_VERSION_INFO header
        auto rootHeader          = reinterpret_cast<VersionInfoHeader*>(ptr);
        rootHeader->wValueLength = sizeof(VS_FIXEDFILEINFO_CUSTOM);
        rootHeader->wType        = 0; // Binary
        wcscpy_s(rootHeader->szKey, L"VS_VERSION_INFO");
        ptr += sizeof(VersionInfoHeader);

        // Align to 4-byte boundary
        ptr = alignPointer(ptr, 4);

        // VS_FIXEDFILEINFO - FIXED VALUES
        const auto fixedInfo          = reinterpret_cast<VS_FIXEDFILEINFO_CUSTOM*>(ptr);
        fixedInfo->dwSignature        = 0xFEEF04BD;
        fixedInfo->dwStrucVersion     = 0x00010000;
        fixedInfo->dwFileVersionMS    = MAKELONG(0, 1); // Version 1.0.0.0
        fixedInfo->dwFileVersionLS    = MAKELONG(0, 0);
        fixedInfo->dwProductVersionMS = MAKELONG(0, 1);
        fixedInfo->dwProductVersionLS = MAKELONG(0, 0);
        fixedInfo->dwFileFlagsMask    = VS_FFI_FILEFLAGSMASK;
        fixedInfo->dwFileFlags        = 0;
        fixedInfo->dwFileOS           = VOS_NT_WINDOWS32;
        fixedInfo->dwFileType         = VFT_APP;
        fixedInfo->dwFileSubtype      = VFT2_UNKNOWN;
        fixedInfo->dwFileDateMS       = 0;
        fixedInfo->dwFileDateLS       = 0;
        ptr += sizeof(VS_FIXEDFILEINFO_CUSTOM);

        // Align to a 4-byte boundary
        ptr = alignPointer(ptr, 4);

        // StringFileInfo header
        const BYTE* stringFileInfoStart    = ptr;
        const auto  stringFileInfoHeader   = reinterpret_cast<StringInfoHeader*>(ptr);
        stringFileInfoHeader->wValueLength = 0; // No direct value
        stringFileInfoHeader->wType        = 1; // Text
        ptr += sizeof(StringInfoHeader) - sizeof(WCHAR);
        ptr = writeWideString(ptr, L"StringFileInfo");
        ptr = alignPointer(ptr, 4);

        // StringTable header (using language ID 040904b0 = US English, Unicode)
        const BYTE* stringTableStart    = ptr;
        const auto  stringTableHeader   = reinterpret_cast<StringInfoHeader*>(ptr);
        stringTableHeader->wValueLength = 0; // No direct value
        stringTableHeader->wType        = 1; // Text
        ptr += sizeof(StringInfoHeader) - sizeof(WCHAR);
        ptr = writeWideString(ptr, L"040904b0"); // US English, Unicode
        ptr = alignPointer(ptr, 4);

        // Helper lambda to write string entries with proper alignment
        auto writeStringEntry = [&](const std::wstring& key, const std::wstring& value) -> bool {
            if (value.empty())
                return false; // Skip empty values

            const BYTE* entryStart    = ptr;
            const auto  entryHeader   = reinterpret_cast<StringInfoHeader*>(ptr);
            entryHeader->wValueLength = static_cast<WORD>((value.length() + 1) * sizeof(WCHAR));
            entryHeader->wType        = 1; // Text
            ptr += sizeof(StringInfoHeader) - sizeof(WCHAR);

            // Write key
            ptr = writeWideString(ptr, key);
            ptr = alignPointer(ptr, 4);

            // Write value  
            ptr = writeWideString(ptr, value);
            ptr = alignPointer(ptr, 4);

            // Update entry length
            entryHeader->wLength = static_cast<WORD>(ptr - entryStart);
            return true;
        };

        // Write string entries - ORDER MATTERS
        writeStringEntry(L"CompanyName", companyName.empty() ? L" " : companyName); // Use space if empty
        writeStringEntry(L"FileDescription", fileDescription);
        writeStringEntry(L"FileVersion", fileVersion);
        writeStringEntry(L"InternalName", internalName);
        writeStringEntry(L"LegalCopyright", copyright.empty() ? L" " : copyright); // Use space if empty
        writeStringEntry(L"OriginalFilename", originalFilename);
        writeStringEntry(L"ProductName", productName);
        writeStringEntry(L"ProductVersion", productVersion);

        // Update StringTable length
        stringTableHeader->wLength = static_cast<WORD>(ptr - stringTableStart);

        // Update StringFileInfo length  
        stringFileInfoHeader->wLength = static_cast<WORD>(ptr - stringFileInfoStart);

        // Update root header length
        rootHeader->wLength = static_cast<WORD>(ptr - startPtr);

        // Resize vector to actual used size
        versionData.resize(ptr - startPtr);

        return versionData;
    }
}

namespace OS
{
    bool patchExecutable(const std::wstring& filename,
                         const std::wstring& icoFileName,
                         const std::wstring& appName,
                         const std::wstring& appDescription,
                         Utf8&               error) // Changed from Utf8& to std::string&
    {
        if (icoFileName.empty() && appName.empty() && appDescription.empty())
            return true;

        const auto handle = BeginUpdateResourceW(filename.c_str(), FALSE);
        if (!handle)
        {
            error = "cannot begin resource update";
            return false;
        }

        bool result = true;

        // Icon processing
        if (!icoFileName.empty())
        {
            const auto file = CreateFileW(icoFileName.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
            if (file == INVALID_HANDLE_VALUE)
            {
                error = "cannot open icon file";
                EndUpdateResourceW(handle, TRUE);
                return false;
            }

            DWORD       bytes;
            ICONVAL     icon;
            ICONHEADER& header = icon.header;
            if (!ReadFile(file, &header, 3 * sizeof(WORD), &bytes, nullptr))
            {
                CloseHandle(file);
                error = "cannot read icon header";
                EndUpdateResourceW(handle, TRUE);
                return false;
            }

            if (header.reserved != 0 || header.type != 1)
            {
                CloseHandle(file);
                error = "reserved header is not 0 or image type is not icon";
                EndUpdateResourceW(handle, TRUE);
                return false;
            }

            header.entries.resize(header.count);
            if (!ReadFile(file, header.entries.data(), header.count * sizeof(ICONENTRY), &bytes, nullptr))
            {
                CloseHandle(file);
                error = "cannot read icon metadata";
                EndUpdateResourceW(handle, TRUE);
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
                    EndUpdateResourceW(handle, TRUE);
                    return false;
                }
            }

            CloseHandle(file);

            // Create a group icon header
            icon.grpHeader.resize(3 * sizeof(WORD) + header.count * sizeof(GRPICONENTRY));
            const auto pGrpHeader = reinterpret_cast<GRPICONHEADER*>(icon.grpHeader.data());
            pGrpHeader->reserved  = 0;
            pGrpHeader->type      = 1;
            pGrpHeader->count     = header.count;

            // FIXED: Correct assignment with proper data types
            for (WORD i = 0; i < header.count; ++i)
            {
                GRPICONENTRY* entry = pGrpHeader->entries + i;
                entry->width        = header.entries[i].width;
                entry->height       = header.entries[i].height;
                entry->colourCount  = header.entries[i].colorCount;
                entry->reserved     = header.entries[i].reserved;
                entry->planes       = header.entries[i].planes;
                entry->bitCount     = header.entries[i].bitCount;
                entry->bytesInRes   = header.entries[i].bytesInRes;
                entry->id           = i + 1;
            }

            // Update icon resources
            if (!UpdateResourceW(handle, RT_GROUP_ICON, MAKEINTRESOURCEW(1), MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), icon.grpHeader.data(), static_cast<DWORD>(icon.grpHeader.size())))
            {
                result = false;
                error  = "failed to update group icon resource";
            }

            for (size_t i = 0; i < icon.header.count && result; ++i)
            {
                if (!UpdateResourceW(handle, RT_ICON, MAKEINTRESOURCEW(i + 1), MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), icon.images[i].data(), static_cast<DWORD>(icon.images[i].size())))
                {
                    result = false;
                    error  = "failed to update icon resource";
                    break;
                }
            }
        }

        // ENHANCED: Add version information with better error reporting
        if (result && (!appName.empty() || !appDescription.empty()))
        {
            std::vector<BYTE> versionInfo = createVersionInfo(appName, appDescription);
            if (!versionInfo.empty())
            {
                // Try with US English first, then fall back to neutral
                if (!UpdateResourceW(handle, RT_VERSION, MAKEINTRESOURCEW(1), MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), versionInfo.data(), static_cast<DWORD>(versionInfo.size())))
                {
                    // Fallback to neutral language
                    if (!UpdateResourceW(handle, RT_VERSION, MAKEINTRESOURCEW(1), MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), versionInfo.data(), static_cast<DWORD>(versionInfo.size())))
                    {
                        result = false;
                        error  = "failed to update version resource";
                    }
                }
            }
        }

        const BOOL endResult = EndUpdateResourceW(handle, result ? FALSE : TRUE);
        if (!endResult && result)
        {
            error  = "failed to commit resource updates";
            result = false;
        }

        return result;
    }
}

#endif
