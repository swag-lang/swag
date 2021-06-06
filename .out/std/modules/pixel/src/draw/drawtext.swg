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

        sharedSolidBrush.type = .SolidColor
        sharedSolidBrush.color = color
        setTexture0(Vector2.Zero, Vector2.Zero, sharedSolidBrush)

        x := left
        y := top
        lastAtlas := -1

        visit ref: arr
        {
            if lastAtlas != ref.atlasIndex
            {
                lastAtlas = ref.atlasIndex
                cmd := newCommand(CommandId.BindTexture1, true)
                params := &cmd.params.texture1
                params.texture = font.atlases[ref.atlasIndex].texture
            }

            x1  := x + ref.shiftX
            y1  := y - ref.shiftY

            p1  := Vector2{x1, y1}
            p2  := Vector2{x1 + ref.width, y1}
            p3  := Vector2{x1, y1 + ref.height}
            p4  := Vector2{x1 + ref.width, y1 + ref.height}

            uv1 := Vector2{ref.uv.x, ref.uv.y}
            uv2 := Vector2{ref.uv.x + ref.uv.width, ref.uv.y}
            uv3 := Vector2{ref.uv.x, ref.uv.y + ref.uv.height}
            uv4 := Vector2{ref.uv.x + ref.uv.width, ref.uv.y + ref.uv.height}

            discard pushTriangle(p1, p2, p3, uv1, uv2, uv3, color)
            discard pushTriangle(p3, p2, p4, uv3, uv2, uv4, color)

            x += ref.advanceX
        }
    }
}