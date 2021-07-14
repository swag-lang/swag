#global export
#global foreignlib "Gdi32"
#global if #os == "windows"
using Swag, Win32

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

#[foreign("Gdi32")]
{
}

namespace gdi32priv
{
    #[foreign("Gdi32")]
    {
        func ChoosePixelFormat(hdc: HDC, ppfd: const *PIXELFORMATDESCRIPTOR)->s32;
        func SetPixelFormat(hdc: HDC, fmt: s32, ppfd: const *PIXELFORMATDESCRIPTOR)->BOOL;
        func SwapBuffers(arg1: HDC)->BOOL;
        func DescribePixelFormat(hdc: HDC, iPixelFormat: s32, nBytes: UINT, ppfd: *PIXELFORMATDESCRIPTOR)->s32;
    }
}

#[inline]
{
    func ChoosePixelFormat(hdc: HDC, ppfd: const *PIXELFORMATDESCRIPTOR)->s32 throw
    {
        res := gdi32priv.ChoosePixelFormat(hdc, ppfd)
        if !res
            try setError(GetLastError())
        return res
    }

    func SetPixelFormat(hdc: HDC, fmt: s32, ppfd: const *PIXELFORMATDESCRIPTOR) throw
    {
        if !gdi32priv.SetPixelFormat(hdc, fmt, ppfd)
            try setError(GetLastError())
    }

    func SwapBuffers(arg1: HDC) throw
    {
        if !gdi32priv.SwapBuffers(arg1)
            try setError(GetLastError())
    }

    #[Discardable]
    func DescribePixelFormat(hdc: HDC, iPixelFormat: s32, nBytes: UINT, ppfd: *PIXELFORMATDESCRIPTOR)->s32 throw
    {
        res := gdi32priv.DescribePixelFormat(hdc, iPixelFormat, nBytes, ppfd)
        if !res
            try setError(GetLastError())
        return res
    }
}