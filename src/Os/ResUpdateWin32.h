#pragma once

#define RU_VS_COMMENTS          L"Comments"
#define RU_VS_COMPANY_NAME      L"CompanyName"
#define RU_VS_FILE_DESCRIPTION  L"FileDescription"
#define RU_VS_FILE_VERSION      L"FileVersion"
#define RU_VS_INTERNAL_NAME     L"InternalName"
#define RU_VS_LEGAL_COPYRIGHT   L"LegalCopyright"
#define RU_VS_LEGAL_TRADEMARKS  L"LegalTrademarks"
#define RU_VS_ORIGINAL_FILENAME L"OriginalFilename"
#define RU_VS_PRIVATE_BUILD     L"PrivateBuild"
#define RU_VS_PRODUCT_NAME      L"ProductName"
#define RU_VS_PRODUCT_VERSION   L"ProductVersion"
#define RU_VS_SPECIAL_BUILD     L"SpecialBuild"

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

struct ICONHEADER
{
    WORD                   reserved;
    WORD                   type;
    WORD                   count;
    std::vector<ICONENTRY> entries;
};

struct ICONVAL
{
    ICONHEADER                     header;
    std::vector<std::vector<BYTE>> images;
    std::vector<BYTE>              grpHeader;
};

struct Translate
{
    LANGID wLanguage;
    WORD   wCodePage;
};

using VersionString    = std::pair<std::wstring, std::wstring>;
using OffsetLengthPair = std::pair<const BYTE* const, const size_t>;
struct VersionStringTable
{
    Translate                  encoding;
    std::vector<VersionString> strings;
};

struct VersionInfo
{
    VersionInfo();
    VersionInfo(HMODULE hModule, WORD languageId);

    std::vector<BYTE>       serialize() const;
    bool                    hasFixedFileInfo() const;
    VS_FIXEDFILEINFO&       getFixedFileInfo();
    const VS_FIXEDFILEINFO& getFixedFileInfo() const;
    void                    setFixedFileInfo(const VS_FIXEDFILEINFO& value);

    void               fillDefaultData();
    void               deserializeVersionInfo(const BYTE* pData, size_t size);
    VersionStringTable deserializeVersionStringTable(const BYTE* tableData);
    void               deserializeVersionStringFileInfo(const BYTE* offset, size_t length, std::vector<VersionStringTable>& stringTables);
    void               deserializeVarFileInfo(const unsigned char* offset, std::vector<Translate>& translations);
    OffsetLengthPair   getChildrenData(const BYTE* entryData);

    std::vector<VersionStringTable> stringTables;
    std::vector<Translate>          supportedTranslations;
    VS_FIXEDFILEINFO                fixedFileInfo;
};

struct ResUpdateWin32
{
    using StringValues    = std::vector<std::wstring>;
    using StringTable     = std::map<UINT, StringValues>;
    using StringTableMap  = std::map<WORD, StringTable>;
    using VersionStampMap = std::map<LANGID, VersionInfo>;
    using IconTable       = std::map<UINT, std::unique_ptr<ICONVAL>>;
    using RcDataValue     = std::vector<BYTE>;
    using RcDataMap       = std::map<ptrdiff_t, RcDataValue>;
    using RcDataLangMap   = std::map<LANGID, RcDataMap>;

    struct IconResInfo
    {
        UINT      maxIconId = 0;
        IconTable iconBundles;
    };

    using IconTableMap = std::map<LANGID, IconResInfo>;

     ResUpdateWin32();
    ~ResUpdateWin32();

    bool         load(const WCHAR* filename);
    bool         setVersionString(WORD languageId, const WCHAR* name, const WCHAR* value);
    bool         setVersionString(const WCHAR* name, const WCHAR* value);
    const WCHAR* getVersionString(WORD languageId, const WCHAR* name);
    const WCHAR* getVersionString(const WCHAR* name);
    bool         setProductVersion(WORD languageId, UINT id, unsigned short v1, unsigned short v2, unsigned short v3, unsigned short v4);
    bool         setProductVersion(unsigned short v1, unsigned short v2, unsigned short v3, unsigned short v4);
    bool         setFileVersion(WORD languageId, UINT id, unsigned short v1, unsigned short v2, unsigned short v3, unsigned short v4);
    bool         setFileVersion(unsigned short v1, unsigned short v2, unsigned short v3, unsigned short v4);
    bool         changeString(WORD languageId, UINT id, const WCHAR* value);
    bool         changeString(UINT id, const WCHAR* value);
    bool         changeRcData(UINT id, const WCHAR* pathToResource);
    const WCHAR* getString(WORD languageId, UINT id);
    const WCHAR* getString(UINT id);
    bool         setIcon(const WCHAR* path, const LANGID& langId, UINT iconBundle);
    bool         setIcon(const WCHAR* path, const LANGID& langId);
    bool         setIcon(const WCHAR* path);
    bool         setExecutionLevel(const WCHAR* value);
    bool         isExecutionLevelSet();
    bool         setApplicationManifest(const WCHAR* value);
    bool         isApplicationManifestSet();
    bool         commit();

    bool                 serializeStringTable(const StringValues& values, UINT blockId, std::vector<char>* out);
    static BOOL CALLBACK onEnumResourceManifest(HMODULE hModule, LPCWSTR lpszType, LPWSTR lpszName, LONG_PTR lParam);
    static BOOL CALLBACK onEnumResourceLanguage(HANDLE hModule, LPCWSTR lpszType, LPCWSTR lpszName, WORD wIDLanguage, LONG_PTR lParam);

    Utf8            error;
    HMODULE         module;
    std::wstring    filename;
    std::wstring    executionLevel;
    std::wstring    originalExecutionLevel;
    std::wstring    applicationManifestPath;
    std::wstring    manifestString;
    VersionStampMap versionStampMap;
    StringTableMap  stringTableMap;
    IconTableMap    iconBundleMap;
    RcDataLangMap   rcDataLngMap;
};
