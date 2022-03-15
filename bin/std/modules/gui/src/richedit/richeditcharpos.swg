#global public
using Core, Pixel

struct RichEditCharPos
{
    lineIndex:      uint
    charPosInLine:  uint
}

impl RichEdit
{
    mtd getSizeChar(line: *RichEditLine, i: uint, x: f32)->Math.Point
    {
        var size: retval

        /*if line.chars[i] == "\t"'u8
        {
            //size.x = GetTabOffset(_x);
        }
        else
        {
            styleIdx := line.styles[i]
            font := getTheme().res.fontDefault.getFont(styles[styleIdx].fontMode)
            me := Painter.measureString(line.chars[i], font)
            size.x = me.width
            size.y = me.height
            size.y += m_LineSpacing
        }*/

        return size
    }

    mtd getCharacterPos(cp: RichEditCharPos)->Math.Point
    {
        var result: retval

        lineIndex := Math.min(cp.lineIndex, lines.count - 1)
        pline     := lines[lineIndex]
        result.y  = pline.drawingRect.bottom()

        result.x = getMarginLeft()
        loop i: cp.charPosInLine
        {
            size := getSizeChar(pline, i, result.x)
            result.x += size.x
        }

        return result
    }
}