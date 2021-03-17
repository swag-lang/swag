#global export
#global foreignlib "opengl32"
#global if #os == "windows"
using win32

alias HGLRC = const *void
alias getExtensionAddress = wglGetProcAddress

#[swag.foreign("opengl32")]
{
    func wglCreateContext(hDc: HDC)->HGLRC;
    func wglMakeCurrent(hDc: HDC, hRc: HGLRC)->BOOL;
    func wglDeleteContext(hRc: HGLRC)->BOOL;
    func wglGetProcAddress(arg1: LPCSTR)->const *void;
}