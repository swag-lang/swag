#global export
#global #if #os == Swag.TargetOs.Windows
#foreignlib "opengl32"
using Swag, Win32

typealias HGLRC = const *void
namealias getExtensionAddress = wglGetProcAddress

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
        if !res
            try setError(GetLastError())
        return res
    }

    func wglDeleteContext(hRc: HGLRC) throw
    {
        if !PrivOgl.wglDeleteContext(hRc)
            try setError(GetLastError())
    }

    func wglMakeCurrent(hDc: HDC, hRc: HGLRC) throw
    {
        if !PrivOgl.wglMakeCurrent(hDc, hRc)
            try setError(GetLastError())
    }

    func wglShareLists(hRc0, hRc1: HGLRC) throw
    {
        if !PrivOgl.wglShareLists(hRc0, hRc1)
            try setError(GetLastError())
    }
}