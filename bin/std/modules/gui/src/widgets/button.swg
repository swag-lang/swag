public struct Button
{
    using wnd:           Wnd

    icon:                Icon
    iconSizeX:           f32
    iconSizeY:           f32
    sigPressed:          SigArray'func||(*Button)
    sigRightPressed:     SigArray'func||(*Button)
    isPressed:           bool
    isPressing:          bool
    isHot:               bool
    isIn:                bool
}

impl IWnd for Button
{
    mtd impl onApplyStateEvent(evt: *CommandStateEvent)
    {
        if evt.id != .id do
            return

        if evt.setFlags & .Icon and (.icon.imageList != evt.icon.imageList or .icon.index != evt.icon.index)
        {
            evt.accepted = true
            .icon        = evt.icon
            .invalidate()
        }

        if evt.setFlags & .Name and .name != evt.name
        {
            evt.accepted = true
            .name        = evt.name
            .invalidate()
        }

        if evt.setFlags & .Disabled
        {
            evt.accepted = true
            .enable(!evt.disabled)
        }
    }
}

impl Button
{
    mtd processMouseEvent(evt: *MouseEvent)->bool
    {
        let pos = .surfaceToLocal(evt.surfacePos)
        switch evt.kind
        {
        case MouseWheel:
            evt.accepted = false

        case MouseEnter:
            .isHot, .isIn = true
            .invalidate()

        case MouseLeave:
            .isHot, .isIn = false
            .invalidate()

        case MouseMove:
            if !.isPressing do
                break
            let rect = .getClientRect()
            if !rect.contains(pos)
            {
                .isPressed, .isIn = false
                .invalidate()
            }
            else
            {
                .isIn = true
                if !.isPressed
                {
                    .isPressed = true
                    .invalidate()
                }
            }

        case MousePressed:
            if evt.button == .Left
            {
                .isPressed, .isPressing = true
                .captureMouse()
                .invalidate()
            }
            elif evt.button == .Right
            {
                if .isIn do
                    .sigRightPressed.call(me)
            }

        case MouseReleased:
            if evt.button == .Left
            {
                if .isPressing or .isPressed
                {
                    .isPressing = false
                    .releaseMouse()
                }

                if .isPressed
                {
                    .isPressed, .isPressing = false
                    if .isIn do
                        .sigPressed.call(me)
                    .invalidate()
                }

                return .isIn
            }
        }

        return false
    }
}
