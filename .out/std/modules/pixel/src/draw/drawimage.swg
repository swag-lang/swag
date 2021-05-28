#global public
using Core, Drawing, Core.Math

impl Painter
{
    func drawImage(using self, texture: Texture, left, top, width, height: f32)
    {
        sharedSolidBrush.color = Argb.White
        sharedSolidBrush.texture = texture
        fillRect(left, top, width, height, sharedSolidBrush)
    }
}