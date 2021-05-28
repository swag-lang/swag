#global public
using Core.Drawing

struct Brush
{
    color:      Color = Argb.White
    texture:    Texture
    wrapMode:   WrapMode = Clamp
}