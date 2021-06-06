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

        x := left
        y := top
        visit ref: arr
        {
            p1 := Vector2{x, y}
            p2 := Vector2{x + cast(f32) font.size, y}
            p3 := Vector2{x, y + cast(f32) font.size}
            p4 := Vector2{x + cast(f32) font.size, y + cast(f32) font.size}
            tri0 := pushTriangle(p1, p2, p3, color)
            tri1 := pushTriangle(p3, p2, p4, color)
            x += cast(f32) font.size
        }
    }
}