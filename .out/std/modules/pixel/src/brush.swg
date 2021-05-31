#global public
using Core, Core.Drawing

struct Brush
{
    color:       Color = Argb.White
    textureType: TextureType = SolidColor
    texture:     Texture
    wrapMode:    WrapMode = Clamp
    uvRect:      Math.Rect
}