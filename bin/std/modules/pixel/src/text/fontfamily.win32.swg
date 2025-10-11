#global #if #os == Swag.TargetOs.Windows
using Core, Win32, Gdi32

public struct FontFamilyStyleEnumerate
{
    name:     String
    face:     LOGFONTW
}

public struct FontFamilyEnumerate
{
    name:      String
    faces:     [@countof(FontFamilyStyle)] FontFamilyStyleEnumerate
}

impl FontFamily
{
    func enumSubFamily(fa: const *LOGFONTW, _tm: const *TEXTMETRICW, _fontType: DWORD, lParam: LPARAM)->BOOL
    {
        let len = Utf16.lengthZeroTerminated(&fa.lfFaceName[0])
        if !len do
            return TRUE
        let x = Utf8.fromUtf16(@mkslice(&fa.lfFaceName[0], len))

        let ret = cast(*FontFamilyEnumerate) cast(u64) lParam
        var idx = cast(u64) FontFamilyStyle.Regular

        if fa.lfWeight > FW_REGULAR
        {
            if fa.lfItalic == 255 do
                idx = cast(u64) FontFamilyStyle.BoldItalic
            else do
                idx = cast(u64) FontFamilyStyle.Bold
        }
        elif fa.lfItalic == 255 do
            idx = cast(u64) FontFamilyStyle.Italic

        ret.faces[idx].name = x
        ret.faces[idx].face = dref fa
        return TRUE
    }

    func enumFamily(fa: const *LOGFONTW, _tm: const *TEXTMETRICW, fontType: DWORD, lParam: LPARAM)->BOOL
    {
        if fontType & TRUETYPE_FONTTYPE
        {
            let len = Utf16.lengthZeroTerminated(&fa.lfFaceName[0])
            if !len do
                return TRUE
            let x = Utf8.fromUtf16(@mkslice(&fa.lfFaceName[0], len))

            // ???
            if x[0] == '@' do
                return TRUE

            // Already here ?
            let ret = cast(*Array'FontFamilyEnumerate) cast(u64) lParam
            foreach here in dref ret
            {
                if here.name == x do
                    return TRUE
            }

            var v: FontFamilyEnumerate
            v.name = x

            let hdc = GetDC(null)
            defer ReleaseDC(null, hdc)
            EnumFontFamiliesExW(hdc, fa, @mkcallback(&enumSubFamily), cast(LPARAM) cast(u64) &v, 0)

            ret.add(v)
        }

        return TRUE
    }

    // Enumerate the font families available in the os
    public func enumerateFromOs()->Array'FontFamilyEnumerate
    {
        var result: retval

        let hdc = GetDC(null)
        defer ReleaseDC(null, hdc)

        var logFont: LOGFONTW
        logFont.lfCharSet = DEFAULT_CHARSET
        EnumFontFamiliesExW(hdc, &logFont, @mkcallback(&enumFamily), cast(LPARAM) cast(u64) &result, 0)

        result.sort(func(a, b)
        {
            return a.name <=> b.name
        })
        return result
    }

    // Get a FontFamilyEnumerate from a font family name
    public func getFromOs(fontFamily: string)->FontFamilyEnumerate
    {
        let hdc = GetDC(null)
        defer ReleaseDC(null, hdc)

        var arr: Array'FontFamilyEnumerate
        let v    = Utf16.toZeroTerminated(fontFamily)
        EnumFontFamiliesW(hdc, v.buffer, @mkcallback(&enumFamily), cast(LPARAM) cast(u64) &arr)

        if arr.isEmpty() do
            return {}
        return arr[0]
    }

    // Creates a typeface from a FontFamilyEnumerate and FontFamilyStyle
    public func createTypeFace(enumerate: FontFamilyEnumerate, style: FontFamilyStyle)->#null *TypeFace throw
    {
        var logFont = enumerate.faces[cast(u64) style]
        if style == .Regular
        {
            if logFont.face.lfWidth == 0 do
                logFont = enumerate.faces[cast(u64) FontFamilyStyle.Bold]
        }
        elif style == .Bold
        {
            if logFont.face.lfWidth == 0 do
                logFont = enumerate.faces[cast(u64) FontFamilyStyle.Regular]
        }
        elif style == .Italic
        {
            if logFont.face.lfWidth == 0 do
                logFont = enumerate.faces[cast(u64) FontFamilyStyle.BoldItalic]
            if logFont.face.lfWidth == 0 do
                logFont = enumerate.faces[cast(u64) FontFamilyStyle.Regular]
            if logFont.face.lfWidth == 0 do
                logFont = enumerate.faces[cast(u64) FontFamilyStyle.Bold]
        }
        elif style == .BoldItalic
        {
            if logFont.face.lfWidth == 0 do
                logFont = enumerate.faces[cast(u64) FontFamilyStyle.Italic]
            if logFont.face.lfWidth == 0 do
                logFont = enumerate.faces[cast(u64) FontFamilyStyle.Bold]
            if logFont.face.lfWidth == 0 do
                logFont = enumerate.faces[cast(u64) FontFamilyStyle.Regular]
        }

        if logFont.face.lfWidth == 0
        {
            for i in @countof(FontFamilyStyle)
            {
                logFont = enumerate.faces[i]
                if logFont.face.lfWidth != 0 do
                    break
            }

            if logFont.face.lfWidth == 0 do
                return null
        }

        let hf = try CreateFontIndirectW(logFont.face)
        defer catch DeleteObject(cast() hf)

        let fontName = Format.toString("%_%", logFont.name, cast(u32) style)
        let typeFace = try TypeFace.createFromHFONT(fontName, hf)

        if (style == .Italic or style == .BoldItalic) and !typeFace.isItalic() do
            typeFace.forceItalic = true
        if (style == .Bold or style == .BoldItalic) and !typeFace.isBold() do
            typeFace.forceBold = true

        return typeFace
    }
}
