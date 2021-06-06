#global public
using Core, Drawing, Core.Math

impl Painter
{
    func getTextExtent(using self, left, top: f32, text: string, font: *Font)->Rectangle
    {
        var arr: Array'(const *GlyphRef)

        arr.reserve(@countof(text))
        Utf8.visitRunes(|c| text)
        {
            arr.add(font.getGlyphRef(c))
        }

        var result: retval

        result.x = left
        result.y = top
        result.width = 0

        visit ref: arr
        {
            //result.y = Math.min(result.y, top - ref.shiftY)
            //result.height = Math.max(result.height, ref.height)
            //result.width += ref.advanceX
        }

        return result
    }

    // Draw a text
    func drawText(using self, left, top: f32, text: string, font: *Font, color: Color)
    {
        sharedSolidBrush.type = .SolidColor
        sharedSolidBrush.color = color
        drawText(left, top, text, font, sharedSolidBrush)
    }

    // Draw a text
    func drawText(using self, left, top: f32, text: string, font: *Font, brush: Brush)
    {
        var arr: Array'(const *GlyphRef)

        arr.reserve(@countof(text))
        Utf8.visitRunes(|c| text)
        {
            arr.add(font.getGlyphRef(c))
        }

        updateFont(font)

        rect := getTextExtent(left, top, text, font)
        setTexture0(@{rect.x, rect.y}, @{rect.x + rect.width, rect.y + rect.height}, brush)

        x := left
        y := top

        visit ref: arr
        {
            setTexture1(font.atlases[ref.atlasIndex].texture)

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

            discard pushTriangle(p1, p2, p3, uv1, uv2, uv3, brush.color)
            discard pushTriangle(p3, p2, p4, uv3, uv2, uv4, brush.color)

            x += ref.advanceX
        }
    }
}