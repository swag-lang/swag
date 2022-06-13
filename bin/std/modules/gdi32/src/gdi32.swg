#global export
#global if #os == Swag.TargetOs.Windows
#foreignlib "gdi32"
using Swag, Win32

alias HBITMAP = const *void
alias HGDIOBJ = const *void

struct PIXELFORMATDESCRIPTOR
{
    nSize:          WORD
    nVersion:       WORD
    dwFlags:        DWORD
    iPixelType:     BYTE
    cColorBits:     BYTE
    cRedBits:       BYTE
    cRedShift:      BYTE
    cGreenBits:     BYTE
    cGreenShift:    BYTE
    cBlueBits:      BYTE
    cBlueShift:     BYTE
    cAlphaBits:     BYTE
    cAlphaShift:    BYTE
    cAccumBits:     BYTE
    cAccumRedBits:  BYTE
    cAccumGreenBits:BYTE
    cAccumBlueBits: BYTE
    cAccumAlphaBits:BYTE
    cDepthBits:     BYTE
    cStencilBits:   BYTE
    cAuxBuffers:    BYTE
    iLayerType:     BYTE
    bReserved:      BYTE
    dwLayerMask:    DWORD
    dwVisibleMask:  DWORD
    dwDamageMask:   DWORD
}

const PFD_TYPE_RGBA        = 0
const PFD_TYPE_COLORINDEX  = 1
const PFD_MAIN_PLANE       = 0
const PFD_OVERLAY_PLANE    = 1
const PFD_UNDERLAY_PLANE   = -1

const PFD_DOUBLEBUFFER            = 0x00000001
const PFD_STEREO                  = 0x00000002
const PFD_DRAW_TO_WINDOW          = 0x00000004
const PFD_DRAW_TO_BITMAP          = 0x00000008
const PFD_SUPPORT_GDI             = 0x00000010
const PFD_SUPPORT_OPENGL          = 0x00000020
const PFD_GENERIC_FORMAT          = 0x00000040
const PFD_NEED_PALETTE            = 0x00000080
const PFD_NEED_SYSTEM_PALETTE     = 0x00000100
const PFD_SWAP_EXCHANGE           = 0x00000200
const PFD_SWAP_COPY               = 0x00000400
const PFD_SWAP_LAYER_BUFFERS      = 0x00000800
const PFD_GENERIC_ACCELERATED     = 0x00001000
const PFD_SUPPORT_DIRECTDRAW      = 0x00002000
const PFD_DIRECT3D_ACCELERATED    = 0x00004000
const PFD_SUPPORT_COMPOSITION     = 0x00008000
const PFD_DEPTH_DONTCARE          = 0x20000000
const PFD_DOUBLEBUFFER_DONTCARE   = 0x40000000
const PFD_STEREO_DONTCARE         = 0x80000000

const SRCCOPY             = cast(DWORD)0x00CC0020 /* dest = source                   */
const SRCPAINT            = cast(DWORD)0x00EE0086 /* dest = source OR dest           */
const SRCAND              = cast(DWORD)0x008800C6 /* dest = source AND dest          */
const SRCINVERT           = cast(DWORD)0x00660046 /* dest = source XOR dest          */
const SRCERASE            = cast(DWORD)0x00440328 /* dest = source AND (NOT dest )   */
const NOTSRCCOPY          = cast(DWORD)0x00330008 /* dest = (NOT source)             */
const NOTSRCERASE         = cast(DWORD)0x001100A6 /* dest = (NOT src) AND (NOT dest) */
const MERGECOPY           = cast(DWORD)0x00C000CA /* dest = (source AND pattern)     */
const MERGEPAINT          = cast(DWORD)0x00BB0226 /* dest = (NOT source) OR dest     */
const PATCOPY             = cast(DWORD)0x00F00021 /* dest = pattern                  */
const PATPAINT            = cast(DWORD)0x00FB0A09 /* dest = DPSnoo                   */
const PATINVERT           = cast(DWORD)0x005A0049 /* dest = pattern XOR dest         */
const DSTINVERT           = cast(DWORD)0x00550009 /* dest = (NOT dest)               */
const BLACKNESS           = cast(DWORD)0x00000042 /* dest = BLACK                    */
const WHITENESS           = cast(DWORD)0x00FF0062 /* dest = WHITE                    */
const NOMIRRORBITMAP      = cast(DWORD)0x80000000 /* Do not Mirror the bitmap in this call */
const CAPTUREBLT          = cast(DWORD)0x40000000 /* Include layered windows */

struct BITMAP
{
    bmType:         LONG
    bmWidth:        LONG
    bmHeight:       LONG
    bmWidthBytes:   LONG
    bmPlanes:       WORD
    bmBitsPixel:    WORD
    bmBits:         LPVOID
}

const LF_FACESIZE = 32
const RASTER_FONTTYPE   = 0x0001
const DEVICE_FONTTYPE   = 0x0002
const TRUETYPE_FONTTYPE = 0x0004

struct LOGFONTA
{
    lfHeight:           LONG
    lfWidth:            LONG
    lfEscapement:       LONG
    lfOrientation:      LONG
    lfWeight:           LONG
    lfItalic:           BYTE
    lfUnderline:        BYTE
    lfStrikeOut:        BYTE
    lfCharSet:          BYTE
    lfOutPrecision:     BYTE
    lfClipPrecision:    BYTE
    lfQuality:          BYTE
    lfPitchAndFamily:   BYTE
    lfFaceName:         [LF_FACESIZE] CHAR
}

struct LOGFONTW
{
    lfHeight:           LONG
    lfWidth:            LONG
    lfEscapement:       LONG
    lfOrientation:      LONG
    lfWeight:           LONG
    lfItalic:           BYTE
    lfUnderline:        BYTE
    lfStrikeOut:        BYTE
    lfCharSet:          BYTE
    lfOutPrecision:     BYTE
    lfClipPrecision:    BYTE
    lfQuality:          BYTE
    lfPitchAndFamily:   BYTE
    lfFaceName:         [LF_FACESIZE] WCHAR
}

struct TEXTMETRICA
{
    tmHeight:           LONG
    tmAscent:           LONG
    tmDescent:          LONG
    tmInternalLeading:  LONG
    tmExternalLeading:  LONG
    tmAveCharWidth:     LONG
    tmMaxCharWidth:     LONG
    tmWeight:           LONG
    tmOverhang:         LONG
    tmDigitizedAspectX: LONG
    tmDigitizedAspectY: LONG
    tmFirstChar:        BYTE
    tmLastChar:         BYTE
    tmDefaultChar:      BYTE
    tmBreakChar:        BYTE
    tmItalic:           BYTE
    tmUnderlined:       BYTE
    tmStruckOut:        BYTE
    tmPitchAndFamily:   BYTE
    tmCharSet:          BYTE
};

struct TEXTMETRICW
{
    tmHeight:           LONG
    tmAscent:           LONG
    tmDescent:          LONG
    tmInternalLeading:  LONG
    tmExternalLeading:  LONG
    tmAveCharWidth:     LONG
    tmMaxCharWidth:     LONG
    tmWeight:           LONG
    tmOverhang:         LONG
    tmDigitizedAspectX: LONG
    tmDigitizedAspectY: LONG
    tmFirstChar:        WCHAR
    tmLastChar:         WCHAR
    tmDefaultChar:      WCHAR
    tmBreakChar:        WCHAR
    tmItalic:           BYTE
    tmUnderlined:       BYTE
    tmStruckOut:        BYTE
    tmPitchAndFamily:   BYTE
    tmCharSet:          BYTE
};

alias FONTENUMPROCA = func(const *LOGFONTA, const *TEXTMETRICA, DWORD, LPARAM)->BOOL;
alias FONTENUMPROCW = func(const *LOGFONTW, const *TEXTMETRICW, DWORD, LPARAM)->BOOL;

#[Foreign("Gdi32")]
{
    func GetPixel(hdc: HDC, x, y: s32)->COLORREF;
}

#[Foreign("Gdi32"), Swag.Discardable]
{
    func SelectObject(hdc: HDC, h: HGDIOBJ)->HGDIOBJ;
    func EnumFontFamiliesA(hdc: HDC, lpLogfont: LPCSTR, lpProc: FONTENUMPROCA, lParam: LPARAM)->s32;
    func EnumFontFamiliesW(hdc: HDC, lpLogfont: LPCWSTR, lpProc: FONTENUMPROCW, lParam: LPARAM)->s32;
    func EnumFontFamiliesExA(hdc: HDC, lpLogfont: const *LOGFONTA, lpProc: FONTENUMPROCA, lParam: LPARAM, dwFlags: DWORD)->s32;
    func EnumFontFamiliesExW(hdc: HDC, lpLogfont: const *LOGFONTW, lpProc: FONTENUMPROCW, lParam: LPARAM, dwFlags: DWORD)->s32;
}

namespace Gdi32Priv
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
    }
}

#[Inline]
{
    func ChoosePixelFormat(hdc: HDC, ppfd: const *PIXELFORMATDESCRIPTOR)->s32 throw
    {
        res := Gdi32Priv.ChoosePixelFormat(hdc, ppfd)
        if !res
            try setError(GetLastError())
        return res
    }

    func SetPixelFormat(hdc: HDC, fmt: s32, ppfd: const *PIXELFORMATDESCRIPTOR) throw
    {
        if !Gdi32Priv.SetPixelFormat(hdc, fmt, ppfd)
            try setError(GetLastError())
    }

    func SwapBuffers(arg1: HDC) throw
    {
        if !Gdi32Priv.SwapBuffers(arg1)
            try setError(GetLastError())
    }

    #[Discardable]
    func DescribePixelFormat(hdc: HDC, iPixelFormat: s32, nBytes: UINT, ppfd: *PIXELFORMATDESCRIPTOR)->s32 throw
    {
        res := Gdi32Priv.DescribePixelFormat(hdc, iPixelFormat, nBytes, ppfd)
        if !res
            try setError(GetLastError())
        return res
    }

    func CreateCompatibleDC(hdc: HDC)->HDC throw
    {
        res := Gdi32Priv.CreateCompatibleDC(hdc)
        if res == null
            try setError(GetLastError())
        return res
    }

    func CreateCompatibleBitmap(hdc: HDC, cx, cy: s32)->HBITMAP throw
    {
        res := Gdi32Priv.CreateCompatibleBitmap(hdc, cx, cy)
        if res == null
            try setError(GetLastError())
        return res
    }

    func DeleteDC(hdc: HDC) throw
    {
        res := Gdi32Priv.DeleteDC(hdc)
        if !res
            try setError(GetLastError())
    }

    func DeleteObject(ho: HGDIOBJ) throw
    {
        res := Gdi32Priv.DeleteObject(ho)
        if !res
            try setError(GetLastError())
    }

    func BitBlt(hdc: HDC, x, y, cx, cy: s32, hdcSrc: HDC, x1, y1: s32, rop: DWORD) throw
    {
        res := Gdi32Priv.BitBlt(hdc, x, y, cx, cy, hdcSrc, x1, y1, rop)
        if !res
            try setError(GetLastError())
    }

    func GetObjectA(h: HANDLE, c: s32, pv: LPVOID) throw
    {
        res := Gdi32Priv.GetObjectA(h, c, pv)
        if !res
            try setError(GetLastError())
    }

    func GetObjectW(h: HANDLE, c: s32, pv: LPVOID) throw
    {
        res := Gdi32Priv.GetObjectW(h, c, pv)
        if !res
            try setError(GetLastError())
    }

    func GetBitmapBits(hbit: HBITMAP, cb: LONG, lpvBits: LPVOID) throw
    {
        res := Gdi32Priv.GetBitmapBits(hbit, cb, lpvBits)
        if !res
            try setError(GetLastError())
    }
}