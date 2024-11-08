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

struct ResUpdateWin32
{
    bool    patchIcon(const std::wstring& filename, const std::wstring& path);
    Utf8    error;
    ICONVAL icon;
};
