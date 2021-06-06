#global public
using Core, Drawing, Core.Math

impl Painter
{
    // Draw a texture image
    func drawText(using self, left, top: f32, text: string, font: *Font, color: Color)
    {
        Utf8.visitRunes(|c| text)
        {
            discard font.getGlyphRef(c)
        }

        updateFont(font)
        drawTexture(100, 100, font.atlases[0].texture)
    }
}