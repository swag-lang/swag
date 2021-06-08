#global public
using Core, Drawing, Core.Math

impl Painter
{
    // Draw a texture image
    func drawTexture(using self, left, top, width, height: f32, texture: Texture)
    {
        sharedSolidBrush.color = Argb.White
        sharedSolidBrush.type = BrushType.Texture
        sharedSolidBrush.texture = texture
        sharedSolidBrush.uvMode = .Fill
        fillRect(left, top, width, height, sharedSolidBrush)
    }

    // Draw a texture image
    func drawTexture(using self, left, top: f32, texture: Texture)
    {
        sharedSolidBrush.color = Argb.White
        sharedSolidBrush.type = BrushType.Texture
        sharedSolidBrush.texture = texture
        sharedSolidBrush.uvMode = .Fill
        fillRect(left, top, cast(f32) texture.width, cast(f32) texture.height, sharedSolidBrush)
    }

    // Draw a portion of an image
    func drawTexture(using self, left, top, width, height: f32, srcRect: Rectangle, texture: Texture)
    {
        sharedSolidBrush.color = Argb.White
        sharedSolidBrush.type = BrushType.Texture
        sharedSolidBrush.texture = texture
        sharedSolidBrush.uvMode = .FillSubRect
        sharedSolidBrush.uvRect = srcRect
        fillRect(left, top, width, height, sharedSolidBrush)
    }

    // Draw a portion of an image
    func drawTexture(using self, left, top: f32, srcRect: Rectangle, texture: Texture)
    {
        sharedSolidBrush.color = Argb.White
        sharedSolidBrush.type = BrushType.Texture
        sharedSolidBrush.texture = texture
        sharedSolidBrush.uvMode = .FillSubRect
        sharedSolidBrush.uvRect = srcRect
        fillRect(left, top, srcRect.width, srcRect.height, sharedSolidBrush)
    }
}