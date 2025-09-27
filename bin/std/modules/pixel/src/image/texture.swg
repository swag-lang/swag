#global public
using Ogl, Core

struct Texture
{
    handle:            TextureHandle = InvalidTextureHandle
    pf:                PixelFormat = undefined
    width, height:     s32
    valid:             bool
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
    mtd const getByteSize()->s32 => .width * .height * .pf.bpp() / 8
    mtd const isValid()->bool    => .handle != InvalidTextureHandle

    mtd clear()
    {
        .handle = InvalidTextureHandle
    }
}
