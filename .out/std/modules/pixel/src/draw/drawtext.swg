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
        drawTexture(left, top, font.atlases[0].texture)
    }
}