#global marked
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
        if evt.id != me.id:
            return

        if evt.setFlags & .Icon and (me.icon.imageList != evt.icon.imageList or me.icon.index != evt.icon.index)
        {
            evt.accepted = true
            me.icon      = evt.icon
            me.invalidate()
        }

        if evt.setFlags & .Name and me.name != evt.name
        {
            evt.accepted = true
            me.name      = evt.name
            me.invalidate()
        }

        if evt.setFlags & .Disabled
        {
            evt.accepted = true
            me.enable(!evt.disabled)
        }
    }
}

impl Button
{
    mtd processMouseEvent(evt: *MouseEvent)->bool
    {
        var pos = me.surfaceToLocal(evt.surfacePos)
        switch evt.kind
        {
        case MouseWheel:
            evt.accepted = false

        case MouseEnter:
            me.isHot, me.isIn = true
            me.invalidate()

        case MouseLeave:
            me.isHot, me.isIn = false
            me.invalidate()

        case MouseMove:
            if !me.isPressing:
                break
            var rect = me.getClientRect()
            if !rect.contains(pos)
            {
                me.isPressed, me.isIn = false
                me.invalidate()
            }
            else
            {
                me.isIn = true
                if !me.isPressed
                {
                    me.isPressed = true
                    me.invalidate()
                }
            }

        case MousePressed:
            if evt.button == .Left
            {
                me.isPressed, me.isPressing = true
                me.captureMouse()
                me.invalidate()
            }
            elif evt.button == .Right
            {
                if me.isIn:
                    me.sigRightPressed.call(me)
            }

        case MouseReleased:
            if evt.button == .Left
            {
                if me.isPressing or me.isPressed
                {
                    me.isPressing = false
                    me.releaseMouse()
                }

                if me.isPressed
                {
                    me.isPressed, me.isPressing = false
                    if me.isIn:
                        me.sigPressed.call(me)
                    me.invalidate()
                }

                return me.isIn
            }
        }

        return false
    }
}
