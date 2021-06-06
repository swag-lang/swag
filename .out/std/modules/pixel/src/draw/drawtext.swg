#global public
using Core, Drawing, Core.Math

impl Painter
{
    // Draw a texture image
    func drawText(using self, left, top: f32, text: string, font: *Font, color: Color)
    {
        var arr: Array'(const *GlyphRef)

        Utf8.visitRunes(|c| text)
        {
            arr.add(font.getGlyphRef(c))
        }

        updateFont(font)
        drawTexture(left, top + 50, font.atlases[0].texture)

        discard newCommand(CommandId.ResetTexture)

        x := left
        y := top
        visit ref: arr
        {
            cmd := newCommand(CommandId.BindTexture1, true)
            params := &cmd.params.texture1
            params.texture = font.atlases[ref.atlasIndex].texture

            p1  := Vector2{x, y}
            p2  := Vector2{x + font.size, y}
            p3  := Vector2{x, y + font.size}
            p4  := Vector2{x + font.size, y + font.size}

            uv1 := Vector2{ref.uv.x, ref.uv.y}
            uv2 := Vector2{ref.uv.x + ref.uv.width, ref.uv.y}
            uv3 := Vector2{ref.uv.x, ref.uv.y + ref.uv.height}
            uv4 := Vector2{ref.uv.x + ref.uv.width, ref.uv.y + ref.uv.height}

            discard pushTriangle(p1, p2, p3, uv1, uv2, uv3, color)
            discard pushTriangle(p3, p2, p4, uv3, uv2, uv4, color)
            x += font.size
        }

        discard newCommand(CommandId.ResetTexture, true)
    }
}