#include "pch.h"
#ifdef WIN32
#include "Backend/Runtime.h"
#include "Os/Os.h"

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
    // Correct GRPICONENTRY structure for RT_GROUP_ICON
    struct GRPICONENTRY
    {
        BYTE  width;
        BYTE  height;
        BYTE  colourCount;
        BYTE  reserved;
        WORD  planes;     // WORD per Win32 GRPICONDIR
        WORD  bitCount;   // WORD per Win32 GRPICONDIR
        DWORD bytesInRes; // DWORD size of the RT_ICON data
        WORD  id;         // Resource ID of the RT_ICON
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
        WCHAR szKey[1]; // Variable-length key follows this field
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

    // Align pointer to an N-byte boundary (default: 4)
    BYTE* alignPointer(BYTE* ptr, size_t alignment = 4)
    {
        const uintptr_t addr    = reinterpret_cast<uintptr_t>(ptr);
        const uintptr_t aligned = (addr + alignment - 1) & ~(alignment - 1);
        return reinterpret_cast<BYTE*>(aligned);
    }

    // Write a NUL-terminated wide string and return the new position
    BYTE* writeWideString(BYTE* ptr, const std::wstring& str)
    {
        const size_t lenBytes = (str.length() + 1) * sizeof(WCHAR);
        memcpy(ptr, str.c_str(), lenBytes);
        return ptr + lenBytes;
    }

    // Build a minimal but valid VS_VERSION_INFO block:
    // - VS_FIXEDFILEINFO
    // - StringFileInfo / 040904B0 with common fields
    // - VarFileInfo / Translation (0409, 1200)
    std::vector<BYTE> createVersionInfo(const Path& moduleFileName, const BuildCfg* buildCfg)
    {
        // Pre-allocate a generous buffer (we’ll shrink to fit).
        std::vector<BYTE> versionData(4096, 0);
        BYTE*             ptr      = versionData.data();
        const BYTE*       startPtr = ptr;

        // --- Root: VS_VERSION_INFO ---
        auto* rootHeader         = reinterpret_cast<VersionInfoHeader*>(ptr);
        rootHeader->wLength      = 0;                                                  // backfilled later
        rootHeader->wValueLength = static_cast<WORD>(sizeof(VS_FIXEDFILEINFO_CUSTOM)); // BYTES for root
        rootHeader->wType        = 0;                                                  // Binary
        wcscpy_s(rootHeader->szKey, L"VS_VERSION_INFO");
        ptr += sizeof(VersionInfoHeader);

        // Align then write VS_FIXEDFILEINFO
        ptr                           = alignPointer(ptr, 4);
        auto* fixedInfo               = reinterpret_cast<VS_FIXEDFILEINFO_CUSTOM*>(ptr);
        fixedInfo->dwSignature        = 0xFEEF04BD;
        fixedInfo->dwStrucVersion     = 0x00010000;
        fixedInfo->dwFileVersionMS    = MAKELONG(buildCfg->moduleRevision, buildCfg->moduleVersion);
        fixedInfo->dwFileVersionLS    = MAKELONG(0, buildCfg->moduleBuildNum);
        fixedInfo->dwProductVersionMS = MAKELONG(buildCfg->moduleRevision, buildCfg->moduleVersion);
        fixedInfo->dwProductVersionLS = MAKELONG(0, buildCfg->moduleBuildNum);
        fixedInfo->dwFileFlagsMask    = VS_FFI_FILEFLAGSMASK;
        fixedInfo->dwFileFlags        = 0;
        fixedInfo->dwFileOS           = VOS_NT_WINDOWS32;
        fixedInfo->dwFileType         = VFT_APP;
        fixedInfo->dwFileSubtype      = VFT2_UNKNOWN;
        fixedInfo->dwFileDateMS       = 0;
        fixedInfo->dwFileDateLS       = 0;
        ptr += sizeof(VS_FIXEDFILEINFO_CUSTOM);
        ptr = alignPointer(ptr, 4);

        // Helper to start a "string-style" header block (key written separately)
        auto beginKeyedBlock = [&](const wchar_t* key, WORD type, /*out*/ StringInfoHeader*& hdr, const BYTE*& blockStart) {
            blockStart        = ptr;
            hdr               = reinterpret_cast<StringInfoHeader*>(ptr);
            hdr->wLength      = 0;    // backfilled
            hdr->wValueLength = 0;    // depends on block type
            hdr->wType        = type; // 1=text, 0=binary
            ptr += sizeof(StringInfoHeader) - sizeof(WCHAR);
            ptr = writeWideString(ptr, key);
            ptr = alignPointer(ptr, 4);
        };

        // Helper to end a keyed block by backfilling its wLength
        auto endKeyedBlock = [&](StringInfoHeader* hdr, const BYTE* blockStart) {
            hdr->wLength = static_cast<WORD>(ptr - blockStart);
        };

        // Helper to write a String entry (key/value)
        auto writeStringEntry = [&](const std::wstring& key, const std::wstring& value) {
            if (value.empty())
                return; // skip empties

            const BYTE* entryStart = ptr;
            auto*       entry      = reinterpret_cast<StringInfoHeader*>(ptr);
            entry->wLength         = 0; // backfilled
            // IMPORTANT: For String entries, wValueLength is in WCHARs (not bytes)
            entry->wValueLength = static_cast<WORD>(value.length() + 1);
            entry->wType        = 1; // text
            ptr += sizeof(StringInfoHeader) - sizeof(WCHAR);

            // key
            ptr = writeWideString(ptr, key);
            ptr = alignPointer(ptr, 4);

            // value (NUL-terminated wide string)
            ptr = writeWideString(ptr, value);
            ptr = alignPointer(ptr, 4);

            entry->wLength = static_cast<WORD>(ptr - entryStart);
        };

        // --- StringFileInfo ---
        StringInfoHeader* sfiHdr = nullptr;
        const BYTE*       sfiStart;
        beginKeyedBlock(L"StringFileInfo", 1, sfiHdr, sfiStart);

        // --- StringTable "040904B0" (US English, Unicode) ---
        StringInfoHeader* stHdr = nullptr;
        const BYTE*       stStart;
        beginKeyedBlock(L"040904B0", 1, stHdr, stStart);

        // Common string fields
        const Utf8 appName{buildCfg->resAppName};
        const Utf8 appDescription{buildCfg->resAppDescription};
        const Utf8 appCompany{buildCfg->resAppCompany};
        const Utf8 appCopyright{buildCfg->resAppCopyright};
        const Utf8 appVersion = form("%d.%d.%d.0", buildCfg->moduleVersion, buildCfg->moduleRevision, buildCfg->moduleBuildNum);

        const std::wstring productName      = !appName.empty() ? appName.toWString() : L"Application";
        const std::wstring fileDescription  = !appDescription.empty() ? appDescription.toWString() : productName;
        const std::wstring fileVersion      = appVersion.toWString();
        const std::wstring productVersion   = appVersion.toWString();
        const std::wstring companyName      = appCompany.toWString();
        const std::wstring copyright        = appCopyright.toWString();
        const std::wstring originalFilename = moduleFileName.filename().toWString();

        writeStringEntry(L"CompanyName", companyName);
        writeStringEntry(L"FileDescription", fileDescription);
        writeStringEntry(L"FileVersion", fileVersion);
        writeStringEntry(L"InternalName", productName);
        writeStringEntry(L"LegalCopyright", copyright);
        writeStringEntry(L"OriginalFilename", originalFilename);
        writeStringEntry(L"ProductName", productName);
        writeStringEntry(L"ProductVersion", productVersion);

        // Close StringTable and StringFileInfo
        endKeyedBlock(stHdr, stStart);
        endKeyedBlock(sfiHdr, sfiStart);

        // --- VarFileInfo ---
        StringInfoHeader* vfiHdr = nullptr;
        const BYTE*       vfiStart;
        // VarFileInfo is a text-keyed block with binary value children; wType here is typically 1 (per samples),
        // while child 'Translation' uses wType=0 and wValueLength in BYTES.
        beginKeyedBlock(L"VarFileInfo", 1, vfiHdr, vfiStart);

        // Child: "Translation" with binary value (LANG=0x0409, CP=1200 (0x04B0))
        const BYTE* varStart   = ptr;
        auto*       varEntry   = reinterpret_cast<StringInfoHeader*>(ptr);
        varEntry->wLength      = 0; // backfilled
        varEntry->wValueLength = 4; // BYTES (two WORDs)
        varEntry->wType        = 0; // binary
        ptr += sizeof(StringInfoHeader) - sizeof(WCHAR);
        ptr = writeWideString(ptr, L"Translation");
        ptr = alignPointer(ptr, 4);

        // value: two WORDs -> language, codepage
        *reinterpret_cast<WORD*>(ptr)     = 0x0409; // US English
        *reinterpret_cast<WORD*>(ptr + 2) = 0x04B0; // Unicode (1200)
        ptr += 4;
        ptr = alignPointer(ptr, 4);

        varEntry->wLength = static_cast<WORD>(ptr - varStart);

        // Close VarFileInfo
        endKeyedBlock(vfiHdr, vfiStart);

        // Backfill root length and shrink buffer to fit
        auto* rootHdr    = reinterpret_cast<VersionInfoHeader*>(const_cast<BYTE*>(startPtr));
        rootHdr->wLength = static_cast<WORD>(ptr - startPtr);

        versionData.resize(ptr - startPtr);
        return versionData;
    }
}

namespace OS
{
    bool patchExecutable(const Path& moduleFileName, const BuildCfg* buildCfg, Utf8& error)
    {
        // Early exit if nothing to do
        if (buildCfg->resAppIcoFileName.count == 0 &&
            buildCfg->resAppName.count == 0 &&
            buildCfg->resAppDescription.count == 0)
            return true;

        const auto handle = BeginUpdateResourceW(moduleFileName.toWString().c_str(), FALSE);
        if (!handle)
        {
            error = "cannot begin resource update";
            return false;
        }

        bool result = true;

        // ---- ICON PATCHING ----
        const Utf8         appIcoFileName8{buildCfg->resAppIcoFileName};
        const std::wstring appIcoFileName = appIcoFileName8.toWString();
        if (!appIcoFileName.empty())
        {
            const auto file = CreateFileW(appIcoFileName.c_str(),
                                          GENERIC_READ,
                                          FILE_SHARE_READ,
                                          nullptr,
                                          OPEN_EXISTING,
                                          FILE_ATTRIBUTE_NORMAL,
                                          nullptr);
            if (file == INVALID_HANDLE_VALUE)
            {
                error = "cannot open icon file";
                EndUpdateResourceW(handle, TRUE);
                return false;
            }

            DWORD       bytes = 0;
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

            // Build RT_GROUP_ICON
            icon.grpHeader.resize(3 * sizeof(WORD) + header.count * sizeof(GRPICONENTRY));
            auto* pGrpHeader     = reinterpret_cast<GRPICONHEADER*>(icon.grpHeader.data());
            pGrpHeader->reserved = 0;
            pGrpHeader->type     = 1;
            pGrpHeader->count    = header.count;

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
                entry->id           = static_cast<WORD>(i + 1);
            }

            // Update group icon and individual icons
            if (!UpdateResourceW(handle,
                                 RT_GROUP_ICON,
                                 MAKEINTRESOURCEW(1),
                                 MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),
                                 icon.grpHeader.data(),
                                 static_cast<DWORD>(icon.grpHeader.size())))
            {
                result = false;
                error  = "failed to update group icon resource";
            }

            for (size_t i = 0; i < icon.header.count && result; ++i)
            {
                if (!UpdateResourceW(handle,
                                     RT_ICON,
                                     MAKEINTRESOURCEW(static_cast<UINT>(i + 1)),
                                     MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),
                                     icon.images[i].data(),
                                     static_cast<DWORD>(icon.images[i].size())))
                {
                    result = false;
                    error  = "failed to update icon resource";
                    break;
                }
            }
        }

        // ---- VERSION INFO PATCHING ----
        if (result)
        {
            std::vector<BYTE> versionInfo = createVersionInfo(moduleFileName, buildCfg);
            if (!versionInfo.empty())
            {
                // Try US English first, then neutral as a fallback.
                if (!UpdateResourceW(handle,
                                     RT_VERSION,
                                     MAKEINTRESOURCEW(1),
                                     MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
                                     versionInfo.data(),
                                     static_cast<DWORD>(versionInfo.size())))
                {
                    if (!UpdateResourceW(handle,
                                         RT_VERSION,
                                         MAKEINTRESOURCEW(1),
                                         MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),
                                         versionInfo.data(),
                                         static_cast<DWORD>(versionInfo.size())))
                    {
                        result = false;
                        error  = "failed to update version resource";
                    }
                }
            }
            else
            {
                result = false;
                error  = "failed to build version resource";
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
