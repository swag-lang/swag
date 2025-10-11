#global export
#global #if #os == Swag.TargetOs.Windows
#foreignlib("opengl32")
using Swag, Win32

alias HGLRC               = const *void
alias getExtensionAddress = wglGetProcAddress

#[Foreign("opengl32")]
{
    func wglGetProcAddress(arg1: LPCSTR)->const *void;
}

private namespace PrivOgl
{
    #[Foreign("opengl32")]
    {
        func wglCreateContext(hDc: HDC)->HGLRC;
        func wglDeleteContext(hRc: HGLRC)->BOOL;
        func wglMakeCurrent(hDc: HDC, hRc: HGLRC)->BOOL;
        func wglShareLists(hRc0, hRc1: HGLRC)->BOOL;
    }
}

#[Inline]
{
    func wglCreateContext(hDc: HDC)->HGLRC throw
    {
        let res = PrivOgl.wglCreateContext(hDc)
        if !res do
            throwWinError(GetLastError())
        return res
    }

    func wglDeleteContext(hRc: HGLRC) throw
    {
        if !PrivOgl.wglDeleteContext(hRc) do
            throwWinError(GetLastError())
    }

    func wglMakeCurrent(hDc: HDC, hRc: HGLRC) throw
    {
        if !PrivOgl.wglMakeCurrent(hDc, hRc) do
            throwWinError(GetLastError())
    }

    func wglShareLists(hRc0, hRc1: HGLRC) throw
    {
        if !PrivOgl.wglShareLists(hRc0, hRc1) do
            throwWinError(GetLastError())
    }
}
