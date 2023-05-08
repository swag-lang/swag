public struct Button
{
    using wnd: Wnd

    icon:               Icon
    sigPressed:         SigArray'closure(*Self)
    sigRightPressed:    SigArray'closure(*Self)
    isPressed:          bool
    isPressing:         bool
    isHot:              bool
    isIn:               bool
}

impl IWnd for Button
{
    mtd onApplyStateEvent(evt: *CommandStateEvent)
    {
        if evt.id != id
            return

        if evt.setFlags & .Icon and icon != evt.icon
        {
            evt.accepted = true
            icon = evt.icon
            invalidate()
        }

        if evt.setFlags & .Name and name != evt.name
        {
            evt.accepted = true
            name = evt.name
            invalidate()
        }

        if evt.setFlags & .Disabled
        {
            evt.accepted = true
            enable(!evt.disabled)
        }
    }
}

impl Button
{
    mtd processMouseEvent(evt: *MouseEvent)->bool
    {
        pos := surfaceToLocal(evt.surfacePos)
        switch evt.kind
        {
        case MouseWheel:
            evt.accepted = false

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
            elif evt.button == .Right
            {
                if isIn
                    sigRightPressed.call(self)
            }

        case MouseReleased:
            if evt.button == .Left and isPressed
            {
                releaseMouse()
                isPressed, isPressing = false
                if isIn
                    sigPressed.call(self)
                invalidate()
                return isIn
            }
        }

        return false
    }
}
