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

struct StringMetrics
{
    using rect: Math.Rectangle
    baseLine:   f32
}

struct RichStringFormat
{
    font:           FontFamily
    fontStyle:      FontFamilyStyle = Regular
    palette:        [4] Color
    horzAlignment:  StringHorzAlignment = Left
    vertAlignment:  StringVertAlignment = Top
    lineGap:        s32
    flags:          StringFormatFlags = Default
}

impl Painter
{
    private func getTempGlyphs(text: string, font: *Font)->Array'(const *GlyphDesc)
    {
        var result: retval
        result.allocator = @getcontext().tempAllocator
        result.reserve(@countof(text))
        Utf8.visitRunes(|c| text)
        {
            result.add(font.getGlyphDesc(c))
        }

        return result
    }

    // Get a rune size for a given font
    func measureRune(c: rune, font: *Font)->Math.Point
    {
        var result: retval
        desc := font.getGlyphDesc(c)
        result.x = desc.shiftX + desc.width
        result.y = desc.shiftY + desc.height
        return result
    }

    // Returns the bounding rectangle if a given text, at a given position
    func measureString(text: string, font: *Font)->StringMetrics
    {
        return measureString(0, 0, text, font)
    }

    // Returns the bounding rectangle if a given text, at a given position
    func measureString(left, top: f32, text: string, font: *Font)->StringMetrics
    {
        var result: retval

        y := top + font.ascent
        result.x = left
        result.y = y
        bottom := top

        arr := getTempGlyphs(text, font)
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

        arr := getTempGlyphs(text, font)
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
            pen := Pen.createSolid(brush.color, Math.max(1, font.underlineSize))
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

    // Recompute the layout for a given rectangle
    mtd computeLayout(pos: Math.Rectangle, text: *RichString, format: RichStringFormat)
    {
        Debug.assert(format.font.regular != null, "invalid default font")
        text.layoutRect = pos

        struct OneLine
        {
            start, end:     s32
            width:          f32
            countSpaces:    s32
            whiteSpaces:    f32
            minTop:         f32
            maxHeight:      f32
        }

        // Divide in lines
        var lines: Array'OneLine
        var line:  OneLine

        line.minTop = Swag.F32.Max
        startLine := 0
        lastWasSpace := false

        visit &c, idx: text.chunks
        {
            // Attributes
            fs := c.fontStyle
            if fs == .Default
                fs = format.fontStyle
            switch fs
            {
            case Regular, Default:
                c.font = format.font.regular
            case Bold:
                c.font = format.font.bold
            case Italic:
                c.font = format.font.italic
            case BoldItalic:
                c.font = format.font.boldItalic
            }

            // Measure
            c.boundRect = measureString(c.slice, c.font).rect

            // User eol
            if c.isEol
            {
                line.start = startLine
                line.end  = cast(s32) idx
                lines.add(line)
                startLine = line.end + 1
                line.width = 0
                line.minTop = Swag.F32.Max
                line.maxHeight = 0
                line.countSpaces = 0
                continue
            }

            // Wrap
            if format.flags & .WordWrap or format.horzAlignment == .Justify
            {
                curW := line.width
                if format.horzAlignment == .Justify
                    curW += line.whiteSpaces

                if curW + c.boundRect.width > pos.width
                {
                    line.start = startLine
                    line.end  = cast(s32) idx

                    // We do not want the current line to end with a space
                    if lastWasSpace and idx
                    {
                        line.countSpaces -= 1
                        if format.horzAlignment != .Justify
                            line.width -= text.chunks[idx - 1].boundRect.width
                        else
                            line.whiteSpaces -= text.chunks[idx - 1].boundRect.width
                        line.end -= 1
                    }

                    lines.add(line)
                    startLine  = cast(s32) idx
                    line.width = 0
                    line.minTop = Swag.F32.Max
                    line.maxHeight = 0
                    line.countSpaces = 0
                    line.whiteSpaces = 0

                    // We do not want the new line to start with a space
                    if c.isSpace
                        startLine += 1
                    else
                    {
                        line.width += c.boundRect.x + c.boundRect.width
                        line.minTop = Math.min(line.minTop, c.boundRect.y)
                        line.maxHeight = Math.max(line.maxHeight, c.boundRect.height)
                    }

                    continue
                }
            }

            lastWasSpace = c.isSpace
            if c.isSpace
            {
                line.countSpaces += 1
                if format.horzAlignment != .Justify
                    line.width += c.boundRect.x + c.boundRect.width
                else
                    line.whiteSpaces += c.boundRect.x + c.boundRect.width
            }
            else
                line.width += c.boundRect.x + c.boundRect.width
            line.minTop = Math.min(line.minTop, c.boundRect.y)
            line.maxHeight = Math.max(line.maxHeight, c.boundRect.height)
        }

        if startLine != text.chunks.count
        {
            line.start = startLine
            line.end   = cast(s32) text.chunks.count
            lines.add(line)
        }


        x := text.layoutRect.x
        y := text.layoutRect.y

        // Vertical
        totalHeight := cast(f32) (lines.count * format.font.regular.height)
        totalHeight += cast(f32) (lines.count - 1) * format.lineGap

        switch format.vertAlignment
        {
        case Bottom:    y = pos.bottom() - totalHeight
        case Center:    y = pos.vertCenter() - (totalHeight * 0.5)
        }

        text.boundRect.x = Swag.F32.Max
        text.boundRect.y = y
        text.boundRect.width = 0
        text.boundRect.height = 0

        // Layout
        visit &line, idxLine: lines
        {
            x1 := x
            y1 := y

            switch format.horzAlignment
            {
            case Left:   break
            case Right:  x1 = pos.right() - line.width
            case Center: x1 = pos.horzCenter() - line.width * 0.5
            }

            text.boundRect.x = Math.min(text.boundRect.x, x1)
            text.boundRect.width = Math.max(text.boundRect.width, line.width)

            loop idx: line.start..<line.end
            {
                c := text.chunks[idx]
                c.pos.x = x1
                if c.isSpace and format.horzAlignment == .Justify and idxLine != lines.count - 1
                    x1 += (pos.width - line.width) / line.countSpaces
                else
                    x1 += c.boundRect.width
                c.pos.y = y1
            }

            y += format.font.regular.height + format.lineGap
            text.boundRect.height += format.font.regular.height + format.lineGap
        }
    }

    // Draw the string. The layout will be computed at the first call, and each
    // time 'pos' has changed
    public mtd drawRichString(pos: Rectangle, text: *RichString, format: RichStringFormat, forceCompute = false)
    {
        if text.chunks.count == 0
            return

        // Recompute layout if rectangle has changed
        if text.layoutRect != pos or forceCompute
            computeLayout(pos, text, format)

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

        // Draw
        visit &c: text.chunks
        {
            drawString(c.pos.x, c.pos.y, c.slice, c.font, format.palette[c.colorIdx], c.under)
        }

        // Restore clipping to its previous value
        if format.flags & .Clip
            popClippingRect()
    }
}