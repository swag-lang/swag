#global if #os == Swag.TargetOs.Windows
using Core, Win32, Gdi32

public struct FontFamilyEnumerate
{
}

impl FontFamily
{
    #[Swag.Callback]
    func enumFamily(fa: const *LOGFONTW, tm: const *TEXTMETRICW, fontType: DWORD, lParam: LPARAM)->BOOL
    {
        if fontType & TRUETYPE_FONTTYPE
        {
            len := Utf16.lengthZeroTerminated(&fa.lfFaceName[0])
            if !len
                return TRUE
            x := Utf8.fromUtf16(@mkslice(&fa.lfFaceName[0], len))

            // ???
            if x[0] == "@"'u8
                return TRUE

            ret := cast(*Array'String) cast(uint) lParam
            ret.add(x)
        }

        return TRUE
    }

    // Enumerate the font families available in the os
    public func enumerateFromOs()->Array'String
    {
        var result: retval

        hdc := GetDC(null)
        EnumFontFamiliesW(hdc, null, @mkcallback(&enumFamily), cast(LPARAM) cast(uint) &result)
        ReleaseDC(null, hdc)

        result.sort()
        return result
    }
}