#pragma once

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

using IconTable = std::map<UINT, std::unique_ptr<ICONVAL>>;
struct IconResInfo
{
    UINT      maxIconId = 0;
    IconTable iconBundles;
};
using IconTableMap = std::map<LANGID, IconResInfo>;

struct ResUpdateWin32
{
    bool         setIcon(const WCHAR* path);
    bool         commit(const std::wstring& filename);
    Utf8         error;
    IconTableMap iconBundleMap;
};
