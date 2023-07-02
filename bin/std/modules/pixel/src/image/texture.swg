#global public
using Ogl, Core

struct Texture
{
    handle:         TextureHandle
    pf:             PixelFormat = undefined
    width, height:  s32
    valid:          bool
}

enum InterpolationMode
{
    Pixel
    Linear
}

enum UVMode
{
    Fill
    FillSubRect
    Tile
}

impl Texture
{
    mtd const getByteSize() => width * height * pf.bpp() / 8
    mtd const isValid()     => handle != InvalidTextureHandle
}