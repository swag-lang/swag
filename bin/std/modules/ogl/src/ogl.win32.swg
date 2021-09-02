#global export
#global foreignlib "opengl32"
#global if #os == "windows"
using Swag, Win32

alias HGLRC = const *void
alias getExtensionAddress = wglGetProcAddress

#[Foreign("opengl32")]
{
    func wglGetProcAddress(arg1: LPCSTR)->const *void;
}

namespace oglpriv
{
    #[Foreign("opengl32")]
    {
        func wglCreateContext(hDc: HDC)->HGLRC;
        func wglDeleteContext(hRc: HGLRC)->BOOL;
        func wglMakeCurrent(hDc: HDC, hRc: HGLRC)->BOOL;
    }
}

#[Inline]
{
    func wglCreateContext(hDc: HDC)->HGLRC throw
    {
        res := oglpriv.wglCreateContext(hDc)
        if !res
            try setError(GetLastError())
        return res
    }

    func wglDeleteContext(hRc: HGLRC) throw
    {
        if !oglpriv.wglDeleteContext(hRc)
            try setError(GetLastError())
    }

    func wglMakeCurrent(hDc: HDC, hRc: HGLRC) throw
    {
        if !oglpriv.wglMakeCurrent(hDc, hRc)
            try setError(GetLastError())
    }
}