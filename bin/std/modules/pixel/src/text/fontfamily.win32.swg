#global if #os == Swag.TargetOs.Windows
using Core, Win32, Gdi32

public struct FontFamilyEnumerate
{
}

impl FontFamily
{
    #[Swag.Callback]
    func enumFamily(fa: const *LOGFONTW, tm: const *TEXTMETRICW, a: DWORD, lParam: LPARAM)
    {
        len := Utf16.lengthZeroTerminated(&fa.lfFaceName[0])
        if !len
            return
        x := Utf8.fromUtf16(@mkslice(&fa.lfFaceName[0], len))
        ret := cast(*Array'String) cast(uint) lParam
        if ret.count < 17
        {
            Console.print(x, "\n")
            ret.add(x)
        }
    }

    // Enumerate the font families available in the os
    public func enumerateFromOs()->Array'String
    {
        var result: retval

        hdc := GetDC(null)
        EnumFontFamiliesW(hdc, null, @mkcallback(&enumFamily), cast(LPARAM) cast(uint) &result)
        ReleaseDC(null, hdc)

        return result
    }
}