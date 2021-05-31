#global public
using Core.Drawing

struct Brush
{
    color:       Color = Argb.White
    textureType: TextureType = SolidColor
    texture:     Texture
    wrapMode:    WrapMode = Clamp
}