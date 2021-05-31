#global public
using Core, Drawing, Core.Math

impl Painter
{
    // Draw a texture image
    func drawTexture(using self, texture: Texture, left, top, width, height: f32)
    {
        sharedSolidBrush.color = Argb.White
        sharedSolidBrush.textureType = .Custom
        sharedSolidBrush.texture = texture
        sharedSolidBrush.wrapMode = .Clamp
        fillRect(left, top, width, height, sharedSolidBrush)
    }
}