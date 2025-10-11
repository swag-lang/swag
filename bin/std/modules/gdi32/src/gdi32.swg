#global export
#global #if #os == Swag.TargetOs.Windows
#foreignlib("gdi32")
using Swag, Win32

#[Swag.Strict]
{
    alias HGDIOBJ = const *void
    alias HFONT   = const *void
    alias HPEN    = const *void
}

const GDI_ERROR = 0xFFFFFFFF

struct PIXELFORMATDESCRIPTOR
{
    nSize:               WORD
    nVersion:            WORD
    dwFlags:             DWORD
    iPixelType:          BYTE
    cColorBits:          BYTE
    cRedBits:            BYTE
    cRedShift:           BYTE
    cGreenBits:          BYTE
    cGreenShift:         BYTE
    cBlueBits:           BYTE
    cBlueShift:          BYTE
    cAlphaBits:          BYTE
    cAlphaShift:         BYTE
    cAccumBits:          BYTE
    cAccumRedBits:       BYTE
    cAccumGreenBits:     BYTE
    cAccumBlueBits:      BYTE
    cAccumAlphaBits:     BYTE
    cDepthBits:          BYTE
    cStencilBits:        BYTE
    cAuxBuffers:         BYTE
    iLayerType:          BYTE
    bReserved:           BYTE
    dwLayerMask:         DWORD
    dwVisibleMask:       DWORD
    dwDamageMask:        DWORD
}

const PFD_TYPE_RGBA       = 0
const PFD_TYPE_COLORINDEX = 1
const PFD_MAIN_PLANE      = 0
const PFD_OVERLAY_PLANE   = 1
const PFD_UNDERLAY_PLANE  = -1

const PFD_DOUBLEBUFFER          = 0x00000001
const PFD_STEREO                = 0x00000002
const PFD_DRAW_TO_WINDOW        = 0x00000004
const PFD_DRAW_TO_BITMAP        = 0x00000008
const PFD_SUPPORT_GDI           = 0x00000010
const PFD_SUPPORT_OPENGL        = 0x00000020
const PFD_GENERIC_FORMAT        = 0x00000040
const PFD_NEED_PALETTE          = 0x00000080
const PFD_NEED_SYSTEM_PALETTE   = 0x00000100
const PFD_SWAP_EXCHANGE         = 0x00000200
const PFD_SWAP_COPY             = 0x00000400
const PFD_SWAP_LAYER_BUFFERS    = 0x00000800
const PFD_GENERIC_ACCELERATED   = 0x00001000
const PFD_SUPPORT_DIRECTDRAW    = 0x00002000
const PFD_DIRECT3D_ACCELERATED  = 0x00004000
const PFD_SUPPORT_COMPOSITION   = 0x00008000
const PFD_DEPTH_DONTCARE        = 0x20000000
const PFD_DOUBLEBUFFER_DONTCARE = 0x40000000
const PFD_STEREO_DONTCARE       = 0x80000000

const SRCCOPY        = cast(DWORD) 0x00CC0020     /* dest = source                   */
const SRCPAINT       = cast(DWORD) 0x00EE0086     /* dest = source OR dest           */
const SRCAND         = cast(DWORD) 0x008800C6     /* dest = source AND dest          */
const SRCINVERT      = cast(DWORD) 0x00660046     /* dest = source XOR dest          */
const SRCERASE       = cast(DWORD) 0x00440328     /* dest = source AND (NOT dest )   */
const NOTSRCCOPY     = cast(DWORD) 0x00330008     /* dest = (NOT source)             */
const NOTSRCERASE    = cast(DWORD) 0x001100A6     /* dest = (NOT src) AND (NOT dest) */
const MERGECOPY      = cast(DWORD) 0x00C000CA     /* dest = (source AND pattern)     */
const MERGEPAINT     = cast(DWORD) 0x00BB0226     /* dest = (NOT source) OR dest     */
const PATCOPY        = cast(DWORD) 0x00F00021     /* dest = pattern                  */
const PATPAINT       = cast(DWORD) 0x00FB0A09     /* dest = DPSnoo                   */
const PATINVERT      = cast(DWORD) 0x005A0049     /* dest = pattern XOR dest         */
const DSTINVERT      = cast(DWORD) 0x00550009     /* dest = (NOT dest)               */
const BLACKNESS      = cast(DWORD) 0x00000042     /* dest = BLACK                    */
const WHITENESS      = cast(DWORD) 0x00FF0062     /* dest = WHITE                    */
const NOMIRRORBITMAP = cast(DWORD) 0x80000000     /* Do not Mirror the bitmap in this call */
const CAPTUREBLT     = cast(DWORD) 0x40000000     /* Include layered windows */

struct BITMAP
{
    bmType:           LONG
    bmWidth:          LONG
    bmHeight:         LONG
    bmWidthBytes:     LONG
    bmPlanes:         WORD
    bmBitsPixel:      WORD
    bmBits:           LPVOID
}

struct RGBQUAD
{
    rgbBlue:         BYTE
    rgbGreen:        BYTE
    rgbRed:          BYTE
    rgbReserved:     BYTE
}

const BI_RGB       = 0
const BI_RLE8      = 1
const BI_RLE4      = 2
const BI_BITFIELDS = 3
const BI_JPEG      = 4
const BI_PNG       = 5

const DIB_RGB_COLORS = 0
const DIB_PAL_COLORS = 1

struct BITMAPINFOHEADER
{
    biSize:              DWORD
    biWidth:             LONG
    biHeight:            LONG
    biPlanes:            WORD
    biBitCount:          WORD
    biCompression:       DWORD
    biSizeImage:         DWORD
    biXPelsPerMeter:     LONG
    biYPelsPerMeter:     LONG
    biClrUsed:           DWORD
    biClrImportant:      DWORD
}

struct BITMAPINFO
{
    bmiHeader:     BITMAPINFOHEADER
    bmiColors:     [1] RGBQUAD
}

const LF_FACESIZE       = 32
const RASTER_FONTTYPE   = 0x0001
const DEVICE_FONTTYPE   = 0x0002
const TRUETYPE_FONTTYPE = 0x0004

const FW_DONTCARE   = 0
const FW_THIN       = 100
const FW_EXTRALIGHT = 200
const FW_LIGHT      = 300
const FW_NORMAL     = 400
const FW_MEDIUM     = 500
const FW_SEMIBOLD   = 600
const FW_BOLD       = 700
const FW_EXTRABOLD  = 800
const FW_HEAVY      = 900
const FW_ULTRALIGHT = FW_EXTRALIGHT
const FW_REGULAR    = FW_NORMAL
const FW_DEMIBOLD   = FW_SEMIBOLD
const FW_ULTRABOLD  = FW_EXTRABOLD
const FW_BLACK      = FW_HEAVY

const WHITE_BRUSH         = 0
const LTGRAY_BRUSH        = 1
const GRAY_BRUSH          = 2
const DKGRAY_BRUSH        = 3
const BLACK_BRUSH         = 4
const NULL_BRUSH          = 5
const HOLLOW_BRUSH        = NULL_BRUSH
const WHITE_PEN           = 6
const BLACK_PEN           = 7
const NULL_PEN            = 8
const OEM_FIXED_FONT      = 10
const ANSI_FIXED_FONT     = 11
const ANSI_VAR_FONT       = 12
const SYSTEM_FONT         = 13
const DEVICE_DEFAULT_FONT = 14
const DEFAULT_PALETTE     = 15
const SYSTEM_FIXED_FONT   = 16

const ANSI_CHARSET        = 0
const DEFAULT_CHARSET     = 1
const SYMBOL_CHARSET      = 2
const SHIFTJIS_CHARSET    = 128
const HANGEUL_CHARSET     = 129
const HANGUL_CHARSET      = 129
const GB2312_CHARSET      = 134
const CHINESEBIG5_CHARSET = 136
const OEM_CHARSET         = 255
const JOHAB_CHARSET       = 130
const HEBREW_CHARSET      = 177
const ARABIC_CHARSET      = 178
const GREEK_CHARSET       = 161
const TURKISH_CHARSET     = 162
const VIETNAMESE_CHARSET  = 163
const THAI_CHARSET        = 222
const EASTEUROPE_CHARSET  = 238
const RUSSIAN_CHARSET     = 204
const MAC_CHARSET         = 77
const BALTIC_CHARSET      = 186

const PS_SOLID       = 0
const PS_DASH        = 1              /* -------  */
const PS_DOT         = 2              /* .......  */
const PS_DASHDOT     = 3              /* _._._._  */
const PS_DASHDOTDOT  = 4              /* _.._.._  */
const PS_NULL        = 5
const PS_INSIDEFRAME = 6
const PS_USERSTYLE   = 7
const PS_ALTERNATE   = 8
const PS_STYLE_MASK  = 0x0000000F

const TRANSPARENT = 1
const OPAQUE      = 2
const BKMODE_LAST = 2

const DRIVERVERSION   = 0
const TECHNOLOGY      = 2
const HORZSIZE        = 4
const VERTSIZE        = 6
const HORZRES         = 8
const VERTRES         = 10
const BITSPIXEL       = 12
const PLANES          = 14
const NUMBRUSHES      = 16
const NUMPENS         = 18
const NUMMARKERS      = 20
const NUMFONTS        = 22
const NUMCOLORS       = 24
const PDEVICESIZE     = 26
const CURVECAPS       = 28
const LINECAPS        = 30
const POLYGONALCAPS   = 32
const TEXTCAPS        = 34
const CLIPCAPS        = 36
const RASTERCAPS      = 38
const ASPECTX         = 40
const ASPECTY         = 42
const ASPECTXY        = 44
const LOGPIXELSX      = 88
const LOGPIXELSY      = 90
const SIZEPALETTE     = 104
const NUMRESERVED     = 106
const COLORRES        = 108
const PHYSICALWIDTH   = 110
const PHYSICALHEIGHT  = 111
const PHYSICALOFFSETX = 112
const PHYSICALOFFSETY = 113
const SCALINGFACTORX  = 114
const SCALINGFACTORY  = 115
const VREFRESH        = 116
const DESKTOPVERTRES  = 117
const DESKTOPHORZRES  = 118
const BLTALIGNMENT    = 119
const SHADEBLENDCAPS  = 120
const COLORMGMTCAPS   = 121

struct LOGFONTA
{
    lfHeight:             LONG
    lfWidth:              LONG
    lfEscapement:         LONG
    lfOrientation:        LONG
    lfWeight:             LONG
    lfItalic:             BYTE
    lfUnderline:          BYTE
    lfStrikeOut:          BYTE
    lfCharSet:            BYTE
    lfOutPrecision:       BYTE
    lfClipPrecision:      BYTE
    lfQuality:            BYTE
    lfPitchAndFamily:     BYTE
    lfFaceName:           [LF_FACESIZE] CHAR
}

struct LOGFONTW
{
    lfHeight:             LONG
    lfWidth:              LONG
    lfEscapement:         LONG
    lfOrientation:        LONG
    lfWeight:             LONG
    lfItalic:             BYTE
    lfUnderline:          BYTE
    lfStrikeOut:          BYTE
    lfCharSet:            BYTE
    lfOutPrecision:       BYTE
    lfClipPrecision:      BYTE
    lfQuality:            BYTE
    lfPitchAndFamily:     BYTE
    lfFaceName:           [LF_FACESIZE] WCHAR
}

const LF_FULLFACESIZE = 64
struct ENUMLOGFONTEXA
{
    elfLogFont:      LOGFONTA
    elfFullName:     [LF_FULLFACESIZE] BYTE
    elfStyle:        [LF_FACESIZE] BYTE
    elfScript:       [LF_FACESIZE] BYTE
}

struct ENUMLOGFONTEXW
{
    elfLogFont:      LOGFONTW
    elfFullName:     [LF_FULLFACESIZE] WCHAR
    elfStyle:        [LF_FACESIZE] WCHAR
    elfScript:       [LF_FACESIZE] WCHAR
}

struct TEXTMETRICA
{
    tmHeight:               LONG
    tmAscent:               LONG
    tmDescent:              LONG
    tmInternalLeading:      LONG
    tmExternalLeading:      LONG
    tmAveCharWidth:         LONG
    tmMaxCharWidth:         LONG
    tmWeight:               LONG
    tmOverhang:             LONG
    tmDigitizedAspectX:     LONG
    tmDigitizedAspectY:     LONG
    tmFirstChar:            BYTE
    tmLastChar:             BYTE
    tmDefaultChar:          BYTE
    tmBreakChar:            BYTE
    tmItalic:               BYTE
    tmUnderlined:           BYTE
    tmStruckOut:            BYTE
    tmPitchAndFamily:       BYTE
    tmCharSet:              BYTE
}

struct TEXTMETRICW
{
    tmHeight:               LONG
    tmAscent:               LONG
    tmDescent:              LONG
    tmInternalLeading:      LONG
    tmExternalLeading:      LONG
    tmAveCharWidth:         LONG
    tmMaxCharWidth:         LONG
    tmWeight:               LONG
    tmOverhang:             LONG
    tmDigitizedAspectX:     LONG
    tmDigitizedAspectY:     LONG
    tmFirstChar:            WCHAR
    tmLastChar:             WCHAR
    tmDefaultChar:          WCHAR
    tmBreakChar:            WCHAR
    tmItalic:               BYTE
    tmUnderlined:           BYTE
    tmStruckOut:            BYTE
    tmPitchAndFamily:       BYTE
    tmCharSet:              BYTE
}

struct NEWTEXTMETRICA
{
    tmHeight:               LONG
    tmAscent:               LONG
    tmDescent:              LONG
    tmInternalLeading:      LONG
    tmExternalLeading:      LONG
    tmAveCharWidth:         LONG
    tmMaxCharWidth:         LONG
    tmWeight:               LONG
    tmOverhang:             LONG
    tmDigitizedAspectX:     LONG
    tmDigitizedAspectY:     LONG
    tmFirstChar:            BYTE
    tmLastChar:             BYTE
    tmDefaultChar:          BYTE
    tmBreakChar:            BYTE
    tmItalic:               BYTE
    tmUnderlined:           BYTE
    tmStruckOut:            BYTE
    tmPitchAndFamily:       BYTE
    tmCharSet:              BYTE
    ntmFlags:               DWORD
    ntmSizeEM:              UINT
    ntmCellHeight:          UINT
    ntmAvgWidth:            UINT
}

struct NEWTEXTMETRICW
{
    tmHeight:               LONG
    tmAscent:               LONG
    tmDescent:              LONG
    tmInternalLeading:      LONG
    tmExternalLeading:      LONG
    tmAveCharWidth:         LONG
    tmMaxCharWidth:         LONG
    tmWeight:               LONG
    tmOverhang:             LONG
    tmDigitizedAspectX:     LONG
    tmDigitizedAspectY:     LONG
    tmFirstChar:            WCHAR
    tmLastChar:             WCHAR
    tmDefaultChar:          WCHAR
    tmBreakChar:            WCHAR
    tmItalic:               BYTE
    tmUnderlined:           BYTE
    tmStruckOut:            BYTE
    tmPitchAndFamily:       BYTE
    tmCharSet:              BYTE
    ntmFlags:               DWORD
    ntmSizeEM:              UINT
    ntmCellHeight:          UINT
    ntmAvgWidth:            UINT
}

alias FONTENUMPROCA = func(const *LOGFONTA, const *TEXTMETRICA, DWORD, LPARAM)->BOOL
alias FONTENUMPROCW = func(const *LOGFONTW, const *TEXTMETRICW, DWORD, LPARAM)->BOOL

#[Swag.Inline]
func RGB(r, g, b: s32)->COLORREF
{
    return r | g << 8 | b << 16
}

#[Foreign("Gdi32")]
{
    func GetPixel(hdc: HDC, x, y: s32)->COLORREF;
    func GetStockObject(i: s32)->HGDIOBJ;
}

#[Foreign("Gdi32"), Discardable, Overload]
{
    func GetObjectA(h: HANDLE, c: s32, pv: LPVOID)->s32;
    func GetObjectW(h: HANDLE, c: s32, pv: LPVOID)->s32;
    func SelectObject(hdc: HDC, h: HGDIOBJ)->HGDIOBJ;
    func GetDeviceCaps(hdc: HDC, index: s32)->s32;

    func EnumFontFamiliesA(hdc: HDC, lpLogfont: LPCSTR, lpProc: FONTENUMPROCA, lParam: LPARAM)->s32;
    func EnumFontFamiliesW(hdc: HDC, lpLogfont: LPCWSTR, lpProc: FONTENUMPROCW, lParam: LPARAM)->s32;
    func EnumFontFamiliesExA(hdc: HDC, lpLogfont: const *LOGFONTA, lpProc: FONTENUMPROCA, lParam: LPARAM, dwFlags: DWORD)->s32;
    func EnumFontFamiliesExW(hdc: HDC, lpLogfont: const *LOGFONTW, lpProc: FONTENUMPROCW, lParam: LPARAM, dwFlags: DWORD)->s32;

    func SetBkMode(hdc: HDC, mode: s32)->s32;
    func SetBkColor(hdc: HDC, color: COLORREF)->COLORREF;
    func SetTextColor(hdc: HDC, color: COLORREF)->COLORREF;

    func TextOutA(hdc: HDC, x: s32, y: s32, lpString: LPCSTR, c: s32)->BOOL;
    func TextOutW(hdc: HDC, x: s32, y: s32, lpString: LPCWSTR, c: s32)->BOOL;
    func MoveTo(hdc: HDC, x: s32, y: s32)->BOOL;
    func LineTo(hdc: HDC, x: s32, y: s32)->BOOL;
    func Rectangle(hdc: HDC, left, top, right, bottom: s32)->BOOL;
}

private namespace Gdi32Priv
{
    #[Foreign("Gdi32")]
    {
        func ChoosePixelFormat(hdc: HDC, ppfd: const *PIXELFORMATDESCRIPTOR)->s32;
        func SetPixelFormat(hdc: HDC, fmt: s32, ppfd: const *PIXELFORMATDESCRIPTOR)->BOOL;
        func SwapBuffers(arg1: HDC)->BOOL;
        func DescribePixelFormat(hdc: HDC, iPixelFormat: s32, nBytes: UINT, ppfd: *PIXELFORMATDESCRIPTOR)->s32;
        func CreateCompatibleDC(hdc: HDC)->HDC;
        func CreateCompatibleBitmap(hdc: HDC, cx, cy: s32)->HBITMAP;
        func DeleteDC(hdc: HDC)->BOOL;
        func DeleteObject(ho: HGDIOBJ)->BOOL;
        func BitBlt(hdc: HDC, x, y, cx, cy: s32, hdcSrc: HDC, x1, y1: s32, rop: DWORD)->BOOL;
        func GetObjectA(h: HANDLE, c: s32, pv: LPVOID)->s32;
        func GetObjectW(h: HANDLE, c: s32, pv: LPVOID)->s32;
        func GetBitmapBits(hbit: HBITMAP, cb: LONG, lpvBits: LPVOID)->LONG;
        func SetBitmapBits(hbm: HBITMAP, cb: DWORD, pvBits: LPVOID)->LONG;
        func GetFontData(hdc: HDC, dwTable: DWORD, dwOffset: DWORD, pvBuffer: LPVOID, cjBuffer: DWORD)->DWORD;
        func CreateFontIndirectA(lplf: const *LOGFONTA)->HFONT;
        func CreateFontIndirectW(lplf: const *LOGFONTW)->HFONT;
        func CreateSolidBrush(color: COLORREF)->HBRUSH;
        func CreatePen(iStyle: s32, cWidth: s32, color: COLORREF)->HPEN;
        func CreateDIBSection(hdc: HDC, pbmi: const *BITMAPINFO, usage: UINT, ppvBits: **void, hSection: HANDLE, offset: DWORD)->HBITMAP;
        func GetDIBits(hdc: HDC, hbm: HBITMAP, start: UINT, cLines: UINT, lpvBits: LPVOID, lpbmi: *BITMAPINFO, usage: UINT)->s32;
    }
}

#[Inline, Overload]
{
    func ChoosePixelFormat(hdc: HDC, ppfd: const *PIXELFORMATDESCRIPTOR)->s32 throw
    {
        let res = Gdi32Priv.ChoosePixelFormat(hdc, ppfd)
        if !res do
            throwWinError(GetLastError())
        return res
    }

    func SetPixelFormat(hdc: HDC, fmt: s32, ppfd: const *PIXELFORMATDESCRIPTOR) throw
    {
        if !Gdi32Priv.SetPixelFormat(hdc, fmt, ppfd) do
            throwWinError(GetLastError())
    }

    func SwapBuffers(arg1: HDC) throw
    {
        if !Gdi32Priv.SwapBuffers(arg1) do
            throwWinError(GetLastError())
    }

    #[Discardable]
    func DescribePixelFormat(hdc: HDC, iPixelFormat: s32, nBytes: UINT, ppfd: *PIXELFORMATDESCRIPTOR)->s32 throw
    {
        let res = Gdi32Priv.DescribePixelFormat(hdc, iPixelFormat, nBytes, ppfd)
        if !res do
            throwWinError(GetLastError())
        return res
    }

    func CreateCompatibleDC(hdc: HDC)->HDC throw
    {
        let res = Gdi32Priv.CreateCompatibleDC(hdc)
        if res == null do
            throwWinError(GetLastError())
        return res
    }

    func CreateCompatibleBitmap(hdc: HDC, cx, cy: s32)->HBITMAP throw
    {
        let res = Gdi32Priv.CreateCompatibleBitmap(hdc, cx, cy)
        if res == null do
            throwWinError(GetLastError())
        return res
    }

    func DeleteDC(hdc: HDC) throw
    {
        let res = Gdi32Priv.DeleteDC(hdc)
        if !res do
            throwWinError(GetLastError())
    }

    func DeleteObject(ho: HGDIOBJ) throw
    {
        let res = Gdi32Priv.DeleteObject(ho)
        if !res do
            throwWinError(GetLastError())
    }

    func BitBlt(hdc: HDC, x, y, cx, cy: s32, hdcSrc: HDC, x1, y1: s32, rop: DWORD) throw
    {
        let res = Gdi32Priv.BitBlt(hdc, x, y, cx, cy, hdcSrc, x1, y1, rop)
        if !res do
            throwWinError(GetLastError())
    }

    func GetObjectA(h: HANDLE, c: s32, pv: LPVOID) throw
    {
        let res = Gdi32Priv.GetObjectA(h, c, pv)
        if !res do
            throwWinError(GetLastError())
    }

    func GetObjectW(h: HANDLE, c: s32, pv: LPVOID) throw
    {
        let res = Gdi32Priv.GetObjectW(h, c, pv)
        if !res do
            throwWinError(GetLastError())
    }

    func GetBitmapBits(hbit: HBITMAP, cb: LONG, lpvBits: LPVOID) throw
    {
        let res = Gdi32Priv.GetBitmapBits(hbit, cb, lpvBits)
        if !res do
            throwWinError(GetLastError())
    }

    func SetBitmapBits(hbit: HBITMAP, cb: DWORD, lpvBits: LPVOID) throw
    {
        let res = Gdi32Priv.SetBitmapBits(hbit, cb, lpvBits)
        if !res do
            throwWinError(GetLastError())
    }

    func GetFontData(hdc: HDC, dwTable: DWORD, dwOffset: DWORD, pvBuffer: LPVOID, cjBuffer: DWORD)->DWORD throw
    {
        let res = Gdi32Priv.GetFontData(hdc, dwTable, dwOffset, pvBuffer, cjBuffer)
        if res == GDI_ERROR do
            throwWinError(GetLastError())
        return res
    }

    func CreateFontIndirectA(lplf: const *LOGFONTA)->HFONT throw
    {
        let res = Gdi32Priv.CreateFontIndirectA(lplf)
        if res == null do
            throwWinError(GetLastError())
        return res
    }

    func CreateFontIndirectW(lplf: const *LOGFONTW)->HFONT throw
    {
        let res = Gdi32Priv.CreateFontIndirectW(lplf)
        if res == null do
            throwWinError(GetLastError())
        return res
    }

    func CreateSolidBrush(color: COLORREF)->HBRUSH throw
    {
        let res = Gdi32Priv.CreateSolidBrush(color)
        if res == null do
            throwWinError(GetLastError())
        return res
    }

    func CreatePen(iStyle: s32, cWidth: s32, color: COLORREF)->HPEN throw
    {
        let res = Gdi32Priv.CreatePen(iStyle, cWidth, color)
        if res == null do
            throwWinError(GetLastError())
        return res
    }

    #[Discardable]
    func GetDIBits(hdc: HDC, hbm: HBITMAP, start: UINT, cLines: UINT, lpvBits: LPVOID, lpbmi: *BITMAPINFO, usage: UINT)->s32 throw
    {
        let res = Gdi32Priv.GetDIBits(hdc, hbm, start, cLines, lpvBits, lpbmi, usage)
        if res == 0 do
            throwWinError(GetLastError())
        return res
    }

    func CreateDIBSection(hdc: HDC, pbmi: const *BITMAPINFO, usage: UINT, ppvBits: **void, hSection: HANDLE, offset: DWORD)->HBITMAP throw
    {
        let res = Gdi32Priv.CreateDIBSection(hdc, pbmi, usage, ppvBits, hSection, offset)
        if res == null do
            throwWinError(GetLastError())
        return res
    }
}
