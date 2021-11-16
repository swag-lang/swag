using Core

public struct EditBox
{
    using scrollwnd:    ScrollWnd
    timerCaret:         *Timer
}

impl IWnd for EditBox
{
    mtd onPaintEvent(evt: *PaintEvent)
    {
    }

    mtd onMouseEvent(evt: *MouseEvent)
    {
        switch evt.kind
        {
        case MousePressed:
            if evt.button == .Left
                setFocus()
        }
    }

    mtd onKeyEvent(evt: *KeyEvent)
    {
    }

    mtd onTimerEvent(evt: *TimerEvent)
    {
        @print("x")
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

        discard res.surface.app.addTimer(1000, res)
        return res
    }
}