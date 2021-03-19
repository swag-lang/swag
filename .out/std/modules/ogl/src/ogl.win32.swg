#global export
#global foreignlib "opengl32"
#global if #os == "windows"
using swag, win32

alias HGLRC = const *void
alias getExtensionAddress = wglGetProcAddress

#[foreign("opengl32")]
{
    func wglCreateContext(hDc: HDC)->HGLRC;
    func wglDeleteContext(hRc: HGLRC)->BOOL;
    func wglGetProcAddress(arg1: LPCSTR)->const *void;
}

namespace oglpriv
{
    #[foreign("opengl32")]
    {
        func wglMakeCurrent(hDc: HDC, hRc: HGLRC)->BOOL;
    }
}

#[inline]
{
    func wglMakeCurrent(hDc: HDC, hRc: HGLRC) throw
    {
        if !oglpriv.wglMakeCurrent(hDc, hRc)
            try setError(GetLastError())
    }
}