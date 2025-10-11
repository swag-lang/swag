#global public
using Core

impl Painter
{
    // Draw a texture image
    #[Swag.Overload]
    mtd drawTexture(x, y, width, height: f32, texture: Texture, color: Color = Argb.White)
    {
        if width == 0 or height == 0 or color.a == 0 do
            return

        let brush: Brush{color: color, type: BrushType.Texture, texture: texture, uvMode: .Fill}
        .fillRect(x, y, width, height, brush)
    }

    #[Swag.Overload]
    mtd drawTexture(rect: Math.Rectangle, texture: Texture, color: Color = Argb.White)
    {
        .drawTexture(rect.x, rect.y, rect.width, rect.height, texture, color)
    }

    #[Swag.Overload]
    mtd drawTexture(x, y: f32, texture: Texture, color: Color = Argb.White)
    {
        if color.a == 0 do
            return

        let brush: Brush{color: color, type: BrushType.Texture, texture: texture, uvMode: .Fill}
        .fillRect(x, y, cast(f32) texture.width, cast(f32) texture.height, brush)
    }

    // Draw a portion of an image
    #[Swag.Overload]
    mtd drawTexture(x, y, width, height: f32, srcRect: Math.Rectangle, texture: Texture, color: Color = Argb.White)
    {
        if srcRect.isEmpty() or width == 0 or height == 0 or color.a == 0 do
            return

        let brush: Brush{color: color, type: BrushType.Texture, texture: texture, uvMode: .FillSubRect, uvRect: srcRect}
        .fillRect(x, y, width, height, brush)
    }

    #[Swag.Overload]
    mtd drawTexture(dstRect, srcRect: Math.Rectangle, texture: Texture, color: Color = Argb.White)
    {
        .drawTexture(dstRect.x, dstRect.y, dstRect.width, dstRect.height, srcRect, texture, color)
    }

    #[Swag.Overload]
    mtd drawTexture(x, y: f32, srcRect: Math.Rectangle, texture: Texture, color: Color = Argb.White)
    {
        if srcRect.isEmpty() or color.a == 0 do
            return

        let brush: Brush{color: color, type: BrushType.Texture, texture: texture, uvMode: .FillSubRect, uvRect: srcRect}
        .fillRect(x, y, srcRect.width, srcRect.height, brush)
    }
}
