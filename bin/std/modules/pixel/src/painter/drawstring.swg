#global public
using Core, Core.Math

enum StringHorzAlignment
{
    Center
    Left
    Right
    Justify
}

enum StringVertAlignment
{
    Center
    Top
    Bottom
}

enum UnderlineStyle
{
    None
    Underline
    Strikeout
}

#[Swag.EnumFlags]
enum StringFormatFlags
{
    Default
    WordWrap
    Clip
}

struct StringFormat
{
    horzAlignment:  StringHorzAlignment = Left
    vertAlignment:  StringVertAlignment = Top
    underline:      UnderlineStyle = None
    lineGap:        f32
    flags:          StringFormatFlags = Default
}

struct StringMetrics
{
    using rect: Math.Rectangle
    baseLine:   f32
}

impl Painter
{
    private func getGlyphs(text: string, font: *Font)->Array'(const *GlyphDesc)
    {
        var result: retval
        result.reserve(@countof(text))
        Utf8.visitRunes(|c| text)
        {
            result.add(font.getGlyphDesc(c))
        }

        return result
    }

    // Returns the bounding rectangle if a given text, at a given position
    mtd measureString(text: string, font: *Font)->StringMetrics
    {
        return measureString(0, 0, text, font)
    }

    mtd measureString(left, top: f32, text: string, font: *Font)->StringMetrics
    {
        var result: retval

        y := top + font.ascent
        result.x = left
        result.y = y
        bottom := top

        arr := getGlyphs(text, font)
        visit ref: arr
        {
            result.y = Math.min(result.y, y - ref.shiftY)
            bottom = Math.max(bottom, y - ref.shiftY + ref.height)
            result.baseLine = Math.max(result.baseLine, ref.shiftY)
            result.width += ref.advanceX
        }

        result.height = bottom - result.y
        return result
    }

    // Draw a string
    mtd drawString(left, top: f32, text: string, font: *Font, color: Color, under = UnderlineStyle.None)
    {
        if @countof(text) == 0 or color.a == 0
            return
        sharedSolidBrush.type = .SolidColor
        sharedSolidBrush.color = color
        drawString(left, top, text, font, sharedSolidBrush, under)
    }

    // Draw a string
    mtd drawString(left, top: f32, text: string, font: *Font, brush: Brush, under = UnderlineStyle.None)
    {
        if @countof(text) == 0 or brush.color.a == 0
            return

        arr := getGlyphs(text, font)
        if arr.isEmpty() return

        x := Math.round(left)
        y := Math.round(top + font.ascent)

        // Compute text extent only if necessary
        var boundRect: StringMetrics = ?
        if under != .None or brush.type != .SolidColor
            boundRect = measureString(x, y, text, font)

        // Set brush main texture
        if brush.type != .SolidColor
            setTexture0(@{x, y - boundRect.baseLine}, @{x + boundRect.width, y + boundRect.height}, brush)
        else
            resetTexture0()

        // Underline
        if under != .None
        {
            pen := Pen.createSolid(brush.color, font.underlineSize)
            pen.beginCapStyle = .None
            pen.endCapStyle = .None

            canSetTexture0 = false
            switch under
            {
            case Strikeout:
                yUnder := y - (boundRect.baseLine / 2)
                drawLine(x, yUnder, x + boundRect.width, yUnder, pen)
            case Underline:
                yUnder := y - font.underlinePos
                drawLine(x, yUnder, x + boundRect.width, yUnder, pen)
            }

            canSetTexture0 = true
        }

        updateFont(font)
        visit ref: arr
        {
            setTextureFont(font, ref.atlasIndex)

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