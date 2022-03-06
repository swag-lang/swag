#global public

struct Texture
{
    handle:         TextureHandle
    pf:             PixelFormat = ?
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