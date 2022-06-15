#global if #os == Swag.TargetOs.Windows
using Core, Win32, Gdi32

public struct FontFamilyStyleEnumerate
{
    name:   String
    face:   LOGFONTW
}

public struct FontFamilyEnumerate
{
    name:   String
    faces: [@countof(FontFamilyStyle)] FontFamilyStyleEnumerate
}

impl FontFamily
{
    #[Swag.Callback]
    func enumSubFamily(fa: const *LOGFONTW, tm: const *TEXTMETRICW, fontType: DWORD, lParam: LPARAM)->BOOL
    {
        len := Utf16.lengthZeroTerminated(&fa.lfFaceName[0])
        if !len
            return TRUE
        x := Utf8.fromUtf16(@mkslice(&fa.lfFaceName[0], len))

        ret := cast(*FontFamilyEnumerate) cast(uint) lParam
        idx := cast(uint) FontFamilyStyle.Regular
        ret.faces[idx].name = x
        ret.faces[idx].face = dref fa
        return TRUE
    }

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

            ret := cast(*Array'FontFamilyEnumerate) cast(uint) lParam
            var v: FontFamilyEnumerate
            v.name = x

            hdc := GetDC(null)
            defer ReleaseDC(null, hdc)
            EnumFontFamiliesExW(hdc, fa, @mkcallback(&enumSubFamily), cast(LPARAM) cast(uint) &v, 0)

            ret.add(v)
        }

        return TRUE
    }

    // Enumerate the font families available in the os
    public func enumerateFromOs()->Array'FontFamilyEnumerate
    {
        var result: retval

        hdc := GetDC(null)
        defer ReleaseDC(null, hdc)
        EnumFontFamiliesW(hdc, null, @mkcallback(&enumFamily), cast(LPARAM) cast(uint) &result)

        result.sort(@(a, b) {
            return a.name <=> b.name
        })
        return result
    }
}