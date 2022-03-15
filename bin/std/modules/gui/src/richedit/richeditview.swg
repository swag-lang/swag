#global public
using Core, Pixel

struct RichEditView
{
    using wnd:  Wnd
    ed:         *RichEdit
}

impl IWnd for RichEditView
{
    mtd onPaintEvent(evt: *PaintEvent)
    {
        painter := evt.bc

        y := padding.y

        visit l, i: ed.lines
        {
            paintLine(painter, i, y, true)
            y += 20
        }
    }
}

impl RichEditView
{
    mtd paintString(bc: *PaintContext, x, y: f32, str: String, strStyle: OneStyle, visible: bool)->f32
    {
        painter := bc.painter
        if visible and strStyle.colBk.a
        {
        }

        font := getTheme().res.fontDefault.getFont(strStyle.fontMode)
        painter.drawString(x, y, str, font, strStyle.colFg)
        return x
    }

    mtd paintLine(bc: *PaintContext, lineIndex: uint, y: f32, visible: bool)
    {
        line := ed.lines[lineIndex]

        // Dummy character for empty lines, so that a selected line display something
        dummyChar  := " "'u8
        dummyStyle := 0'u8
        count  := line.length()
        pChar  := line.chars.buffer
        pStyle := line.styles.buffer
        if !count
        {
            pChar  = &dummyChar
            pStyle = &dummyStyle
            count   = 1
        }

        var str: String{allocator: @getcontext().tempAllocator}
        str.reserve(line.length())

        var oldStyle, newStyle: OneStyle
        x := padding.x
        oldStyle.colFg = Argb.White

        i := 0
        while i < count
        {
            defer { i += 1; pChar += 1; pStyle += 1; }
            str += line.chars[i]
        }

        if !str.isEmpty()
        {
            x1 := paintString(bc, x, y, str, oldStyle, visible)
        }
    }
}
