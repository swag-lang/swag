#global public
using Core.Drawing

struct Brush
{
    color:      Color
    texture:    Texture
    wrapMode:   WrapMode = Clamp
}