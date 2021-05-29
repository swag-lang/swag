#global public
using Core.Drawing

enum BrushTextureStyle
{
    None
    Custom
    Dot
}

struct Brush
{
    color:          Color = Argb.White
    textureStyle:   BrushTextureStyle = None
    texture:        Texture
    wrapMode:       WrapMode = Clamp
}