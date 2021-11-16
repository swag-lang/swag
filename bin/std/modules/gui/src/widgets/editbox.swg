using Core, Pixel

public struct EditBox
{
    using scrollwnd:    ScrollWnd
    timerCaret:         *Timer
    visibleCaret:       bool
}

impl IWnd for EditBox
{
    mtd onPaintEvent(evt: *PaintEvent)
    {
        painter := evt.bc.painter

        if visibleCaret and getFocus() == self
        {
            pos := getCaretPos()
            painter.fillRect(pos, Argb.White)
        }
    }

    mtd onMouseEvent(evt: *MouseEvent)
    {
        switch evt.kind
        {
        case MousePressed:
            if evt.button == .Left
                setFocus()
            evt.accepted = true
        }
    }

    mtd onKeyEvent(evt: *KeyEvent)
    {
    }

    mtd onTimerEvent(evt: *TimerEvent)
    {
        visibleCaret = !visibleCaret
        pos := getCaretPos()
        pos.inflate(1)
        pos.offset(-getScrollPos())
        invalidateRect(pos)
    }
}

public impl EditBox
{
    func create(parent: *Wnd, content: string, position: Math.Rectangle, id: string = null)->*EditBox
    {
        pos := position
        if pos.height == 0 pos.height = parent.getThemeMetrics().btnPush_Height
        if pos.width == 0  pos.width  = parent.getThemeMetrics().btnPush_Width
        res := Wnd.create'EditBox(parent, content, pos, id)
        evt := CreateEvent{}
        res.sendEvent(&evt)

        discard res.surface.app.addTimer(500, res)
        return res
    }

    mtd getCaretPos()->Math.Rectangle
    {
        pos := position
        pos.width = 4
        return pos
    }
}