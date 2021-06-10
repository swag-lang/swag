#global public
using Core, Drawing, Core.Math

enum StringHorzAlignment
{
    Center
    Left
    Right
}

enum StringVertAlignment
{
    Center
    Top
    Bottom
}

struct StringMetrics
{
    left:       f32
    top:        f32
    width:      f32
    height:     f32
    baseLine:   f32
}

struct StringFormat
{
    horzAlignment: StringHorzAlignment = Left
    vertAlignment: StringVertAlignment = Bottom
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
    func measureString(using self, left, top: f32, text: string, font: *Font)->StringMetrics
    {
        var result: retval

        result.left = left
        result.top = top
        bottom := top

        arr := getGlyphs(text, font)
        visit ref: arr
        {
            result.top = Math.min(result.top, top - ref.shiftY)
            bottom = Math.max(bottom, top - ref.shiftY + ref.height)
            result.baseLine = Math.max(result.baseLine, ref.shiftY)
            result.width += ref.advanceX
        }

        result.height = bottom - result.top
        return result
    }

    // Draw a text
    func drawString(using self, left, top: f32, text: string, font: *Font, color: Color)
    {
        sharedSolidBrush.type = .SolidColor
        sharedSolidBrush.color = color
        drawString(left, top, text, font, sharedSolidBrush)
    }

    // Draw a text
    func drawString(using self, left, top: f32, text: string, font: *Font, brush: Brush)
    {
        arr := getGlyphs(text, font)
        if arr.isEmpty() return
        updateFont(font)

        x := left
        y := top
        boundRect := measureString(x, y, text, font)

        // Set brush main texture
        setTexture0(@{x, y - boundRect.baseLine}, @{x + boundRect.width, y + boundRect.height}, brush)

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

    func drawString(using self, pos: Rectangle, text: string, font: *Font, color: Color, format: StringFormat)
    {
        sharedSolidBrush.type = .SolidColor
        sharedSolidBrush.color = color
        drawString(pos, text, font, sharedSolidBrush, format)
    }

    func drawString(using self, pos: Rectangle, text: string, font: *Font, brush: Brush, format: StringFormat)
    {
        struct OneStr
        {
            str: string
            br:  StringMetrics
        }

        var arr: Array'string

        // Divide the text in lines
        lastLine := 0'uint
        testX := 0.0
        blank := Swag.U32.Max
        Utf8.visitRunes(|c, ci, bi| text)
        {
            if c == "\n"'rune
            {
                arr.add(@mkstring(@dataof(text), bi - lastLine))
                lastLine = bi + 1
                testX = 0
                blank = Swag.U32.Max
                continue
            }

            if Unicode.isLatin1(c) and Latin1.isSpace(cast(u8) c)
                blank = cast(u32) bi

            desc := font.getGlyphDesc(c)
            testX += desc.shiftX + desc.advanceX
            if testX > pos.x + pos.width
            {
            }

        }

        if lastLine != @countof(text)
            arr.add(@mkstring(@dataof(text) + lastLine, @countof(text) - lastLine))

        /*boundRect := measureString(x, y, text, font)
        switch format.horzAlignment
        {
        case Left:      break
        case Right:     x -= boundRect.width
        case Center:    x -= Math.round(boundRect.width / 2)
        }

        switch format.vertAlignment
        {
        case Top:       y += boundRect.height
        case Bottom:    break
        case Center:    y += Math.round(boundRect.baseLine / 2)
        }*/

        // Draw each line
        x := pos.x
        y := pos.y
        visit str: arr
        {
            x1 := x
            y1 := y

            boundRect := measureString(x1, y1, str, font)
            drawString(x1, y1, str, font, brush)
            y += boundRect.height
        }
    }
}