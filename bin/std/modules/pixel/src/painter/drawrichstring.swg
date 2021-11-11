using Core, Math

public struct RichStringFormat
{
    font:           FontFamily
    palette:        [4] Color
    horzAlignment:  StringHorzAlignment = Left
    vertAlignment:  StringVertAlignment = Top
    lineGap:        s32
    flags:          StringFormatFlags = Default
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
            width:      f32
        }

        // Divide in lines
        var lines: Array'OneLine
        var line:  OneLine
        startLine := 0
        lastWasSpace := false

        visit *c, idx: text.chunks
        {
            // User eol
            if c.isEol
            {
                line.start = startLine
                line.end  = cast(s32) idx
                lines.add(line)
                startLine = line.end + 1
                line.width = 0
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

            // Wrap
            if format.flags & .WordWrap
            {
                if line.width + c.boundRect.width > pos.width
                {
                    line.start = startLine
                    line.end  = cast(s32) idx

                    // We do not want the current line to end with a space
                    if lastWasSpace and idx
                    {
                        line.width -= text.chunks[idx - 1].boundRect.width
                        line.end -= 1
                    }

                    lines.add(line)
                    startLine  = cast(s32) idx
                    line.width = 0
                    if c.isSpace
                        startLine += 1
                    else
                        line.width += c.boundRect.x + c.boundRect.width
                    continue
                }
            }

            lastWasSpace = c.isSpace
            line.width += c.boundRect.x + c.boundRect.width
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
            case Right:  x1 = pos.right() - line.width
            case Center: x1 = pos.horzCenter() - line.width * 0.5
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
        if text.chunks.count == 0
            return

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

        // Recompute layout if rectangle has changed
        if text.layoutRect != pos
            computeLayout(pos, text, format)

        // Draw
        visit *c: text.chunks
        {
            if c.isSpace continue
            drawString(c.pos.x, c.pos.y, c.slice, c.font, c.color, c.under)
        }

        // Restore clipping to its previous value
        if format.flags & .Clip
            popClippingRect()
    }
}