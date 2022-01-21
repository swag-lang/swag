public struct Button
{
    using wnd: Wnd

    icon:           Icon
    sigPressed:     func(*Self)
    isPressed:      bool
    isPressing:     bool
    isHot:          bool
    isIn:           bool
}

impl Button
{
    mtd processMouseEvent(evt: *MouseEvent)->bool
    {
        pos := surfaceToLocal(evt.surfacePos)
        switch evt.kind
        {
        case MouseEnter:
            isHot, isIn = true
            invalidate()

        case MouseLeave:
            isHot, isIn = false
            invalidate()

        case MouseMove:
            if !isPressing
                break
            rect := getClientRect()
            if !rect.contains(pos)
            {
                isPressed = false
                isIn = false
                invalidate()
            }
            else
            {
                isIn = true
                if !isPressed
                {
                    isPressed = true
                    invalidate()
                }
            }

        case MousePressed:
            if evt.button == .Left
            {
                isPressed, isPressing = true
                captureMouse()
                invalidate()
            }

        case MouseReleased:
            if evt.button == .Left
            {
                releaseMouse()
                isPressed, isPressing = false
                if isIn and sigPressed
                    sigPressed(self)
                invalidate()
                return isIn
            }
        }

        return false
    }
}
