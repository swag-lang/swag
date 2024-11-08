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

    ~ResUpdateWin32();

    bool load(const WCHAR* filename);
    bool setIcon(const WCHAR* path, const LANGID& langId, UINT iconBundle);
    bool setIcon(const WCHAR* path, const LANGID& langId);
    bool setIcon(const WCHAR* path);
    bool commit();

    Utf8         error;
    HMODULE      module = nullptr;
    std::wstring filename;
    IconTableMap iconBundleMap;
};
