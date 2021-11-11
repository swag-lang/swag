#global public
using Core, Core.Math

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
    left:       f32
    top:        f32
    width:      f32
    height:     f32
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
        result.left = left
        result.top = y
        bottom := top

        arr := getGlyphs(text, font)
        visit ref: arr
        {
            result.top = Math.min(result.top, y - ref.shiftY)
            bottom = Math.max(bottom, y - ref.shiftY + ref.height)
            result.baseLine = Math.max(result.baseLine, ref.shiftY)
            result.width += ref.advanceX
        }

        result.height = bottom - result.top
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

    // Draw a string
    mtd drawString(pos: Rectangle, text: string, font: *Font, color: Color, format: StringFormat, outRect: *Rectangle = null)
    {
        if @countof(text) == 0 or color.a == 0
            return
        sharedSolidBrush.type = .SolidColor
        sharedSolidBrush.color = color
        drawString(pos, text, font, sharedSolidBrush, format, outRect)
    }

    // Draw a string
    mtd drawString(pos: Rectangle, text: string, font: *Font, brush: Brush, format: StringFormat, outRect: *Rectangle = null)
    {
        if @countof(text) == 0 or brush.color.a == 0
            return

        struct OneStr
        {
            str:    string
            width:  f32
        }

        var arr:    Array'OneStr
        var oneStr: OneStr

        // Divide the text in lines
        lastLine := 0'uint
        testX := 0.0
        firstBlankIdx := Swag.U32.Max
        lastBlankIdx := Swag.U32.Max
        widthAtFirstBlank := 0.0
        widthAtLastBlank := 0.0
        lastIsBlank := false
        Utf8.visitRunes(|c, ci, bi| text)
        {
            if c == "\n"'rune
            {
                oneStr.str = @mkstring(@dataof(text) + lastLine, bi - lastLine)
                oneStr.width = testX
                arr.emplace(&oneStr)
                lastLine = bi + 1
                testX = 0
                firstBlankIdx = Swag.U32.Max
                widthAtFirstBlank = 0
                lastIsBlank = true
                continue
            }

            desc := font.getGlyphDesc(c)

            if format.flags & .WordWrap
            {
                // Store blank informations
                if Unicode.isLatin1(c) and Latin1.isSpace(cast(u8) c)
                {
                    if !lastIsBlank
                    {
                        firstBlankIdx = cast(u32) bi
                        widthAtFirstBlank = testX
                    }

                    lastBlankIdx = cast(u32) bi
                    testX += desc.advanceX
                    widthAtLastBlank = testX
                    lastIsBlank = true
                    continue
                }

                lastIsBlank = false

                // Need to wrap if next character position is outside the box
                if testX + desc.advanceX > pos.width
                {
                    if firstBlankIdx != Swag.U32.Max
                    {
                        oneStr.str = @mkstring(@dataof(text) + lastLine, firstBlankIdx - lastLine)
                        oneStr.width = widthAtFirstBlank
                        arr.emplace(&oneStr)

                        lastLine = lastBlankIdx + 1
                        testX -= widthAtLastBlank
                        firstBlankIdx = Swag.U32.Max
                        lastBlankIdx = Swag.U32.Max
                        widthAtFirstBlank = 0
                    }
                }
            }

            testX += desc.advanceX
        }

        if lastLine != @countof(text)
        {
            oneStr.str = @mkstring(@dataof(text) + lastLine, @countof(text) - lastLine)
            oneStr.width = testX
            arr.emplace(&oneStr)
        }

        // Draw each line
        x := pos.x
        y := pos.y

        // Height of total text
        // Special case when there's only one line, in case of vertical center
        var totalHeight: f32 = ?
        var yOffset: f32
        if arr.count == 1
        {
            boundRect := measureString(0, 0, arr[0].str, font)
            totalHeight = boundRect.height
            yOffset = boundRect.top
        }
        else
        {
            totalHeight = cast(f32) (arr.count * font.height)
            totalHeight += cast(f32) (arr.count - 1) * format.lineGap
        }

        switch format.vertAlignment
        {
        case Top:       y -= yOffset
        case Bottom:    y = pos.bottom() - totalHeight - yOffset
        case Center:    y = pos.vertCenter() - (totalHeight * 0.5) - yOffset
        }

        // Clipping
        if format.flags & .Clip
        {
            pushClippingRect(pos)
            if clippingRect.isEmpty()
            {
                popClippingRect()
                return
            }
        }

        if outRect
        {
            outRect.x = Swag.F32.Max
            outRect.y = y
            outRect.width = 0
            outRect.height = 0
        }

        visit *line: arr
        {
            x1 := x
            y1 := y

            switch format.horzAlignment
            {
            case Left:      break
            case Right:     x1 = pos.right() - line.width
            case Center:    x1 = pos.horzCenter() - line.width * 0.5
            }

            if outRect
            {
                outRect.x = Math.min(outRect.x, x1)
                measure := measureString(x1, y1, line.str, font)
                outRect.width = Math.max(outRect.width, measure.width)
            }

            drawString(x1, y1, line.str, font, brush, format.underline)
            y += font.height + format.lineGap

            if outRect
                outRect.height = y - outRect.y
        }

        // Restore clipping to its previous value
        if format.flags & .Clip
            popClippingRect()
    }
}