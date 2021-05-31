#global public
using Core, Core.Drawing

struct Brush
{
    color:       Color = Argb.White
    textureType: TextureType = SolidColor
    texture:     Texture
    uvMode:    UVMode = Fill
    uvRect:      Math.Rectangle
}