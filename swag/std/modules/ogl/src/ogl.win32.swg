#global public
#if #os == "windows";
#import "win32"
#import "gdi32"
#foreignlib "opengl32"
using win32

alias HGLRC = const *void

#[swag.foreign("opengl32")]
{
    func wglCreateContext(hDc: HDC)->HGLRC;
    func wglMakeCurrent(hDc: HDC, hRc: HGLRC)->BOOL;
    func wglDeleteContext(hRc: HGLRC)->BOOL;
    func wglGetProcAddress(arg1: LPCSTR)->const *void;
}