using Core, Math

public struct RichStringFormat
{
    font:           FontFamily
    palette:        [4] Color
    horzAlignment:  StringHorzAlignment = Left
    vertAlignment:  StringVertAlignment = Top
    lineGap:        s32
}

impl Painter
{
    // Recompute the layout for a given rectangle
    mtd computeLayout(pos: Math.Rectangle, text: *RichString, format: RichStringFormat)
    {
        text.layoutRect = pos

        struct OneLine
        {
            start, end: s32
            boundRect:  Math.Rectangle
        }

        // Divide in lines
        var lines: Array'OneLine
        var line:  OneLine
        startLine := 0

        visit *c, idx: text.chunks
        {
            // User eol
            if c.isEol
            {
                line.start = startLine
                line.end  = cast(s32) idx
                lines.add(line)
                startLine = line.end + 1
                line.boundRect.clear()
                continue
            }

            // Attributes
            c.color = format.palette[c.colorIdx]
            switch c.fontStyle
            {
            case Regular:
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
            line.boundRect.width += c.boundRect.width
            line.boundRect.y      = Math.max(line.boundRect.y, c.boundRect.y)
            line.boundRect.height = Math.max(line.boundRect.height, c.boundRect.height)
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
        case Top:       break
        case Bottom:    y = pos.bottom() - totalHeight
        case Center:    y = pos.vertCenter() - (totalHeight * 0.5)
        }

        // Layout
        visit *line: lines
        {
            x1 := x
            y1 := y

            switch format.horzAlignment
            {
            case Left:   break
            case Right:  x1 = pos.right() - line.boundRect.width
            case Center: x1 = pos.horzCenter() - line.boundRect.width * 0.5
            }

            loop idx: line.start..<line.end
            {
                c := text.chunks[idx]
                c.pos.x = x1; x1 += c.boundRect.width
                c.pos.y = y1
            }

            y += format.font.regular.height + format.lineGap
        }
    }

    // Draw the string. The layout will be computed at the first call, and each
    // time 'pos' has changed
    public mtd drawRichString(pos: Rectangle, text: *RichString, format: RichStringFormat)
    {
        if text.layoutRect != pos
            computeLayout(pos, text, format)

        visit *c: text.chunks
        {
            if c.isSpace continue
            drawString(c.pos.x, c.pos.y, c.slice, c.font, c.color, c.under)
        }
    }
}