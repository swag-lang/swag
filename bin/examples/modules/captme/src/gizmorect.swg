struct GizmoRect
{
    paintSimpleBorder:     bool = true
    paintBigBorder:        bool = true
    roundShape:            bool = false
    anchorSize:            f32 = 12
    trackChange:           bool = false

    rect:                  Math.Rectangle
    startRect:             Math.Rectangle
    anchors:               [8] Math.Rectangle
    hotAnchor:             u32 = Swag.U32.Max
    moving:                bool
    firstMoving:           bool
    movingRect:            Math.Rectangle
    movingStart:           Math.Point
    sigChanged:            func||(*GizmoRect, bool, u32)
    sigHasChanged:         func||()

    minSizeX:              f32 = 2
    minSizeY:              f32 = 2
}

impl GizmoRect
{
    mtd mouse(view: *EditView, pos: Math.Point, evt: *MouseEvent)->bool
    {
        let prop = (evt.modifiers & .Shift) and !(evt.modifiers & .Control)

        switch evt.kind
        {
        case MousePressed:
            if me.moving:
                return true
            if evt.button != .Left or me.hotAnchor == Swag.U32.Max:
                break
            me.moving, me.firstMoving = true
            me.startRect   = me.rect
            me.movingRect  = me.rect
            me.movingStart = pos
            view.captureMouse()
            return true

        case MouseReleased:
            if me.moving
            {
                if evt.button == .Left
                {
                    me.moving, me.firstMoving = false
                    me.hotAnchor = Swag.U32.Max
                    view.releaseMouse()
                    if !me.trackChange and me.sigChanged:
                        me.sigChanged(me, false, 0)
                    if me.sigHasChanged:
                        me.sigHasChanged()
                }

                return true
            }

        case MouseLeave:
            if me.hotAnchor != Swag.U32.Max
            {
                me.hotAnchor = Swag.U32.Max
                return true
            }

        case MouseMove:
            if me.moving
            {
                var hitMove = 0'u32
                if me.firstMoving and me.sigChanged and me.trackChange:
                    me.sigChanged(me, true, 0)

                me.firstMoving = false

                var diff = pos - me.movingStart
                me.movingStart = pos

                switch me.hotAnchor
                {
                case 0:
                    if prop
                    {
                        if Math.abs(diff.x) > Math.abs(diff.y):
                            diff.y = diff.x
                        else:
                            diff.x = diff.y
                    }

                    me.movingRect.x      = Math.min(me.startRect.right() - me.minSizeX, me.movingRect.x + diff.x)
                    me.movingRect.y      = Math.min(me.startRect.bottom() - me.minSizeY, me.movingRect.y + diff.y)
                    me.movingRect.width  = me.startRect.right() - me.movingRect.x
                    me.movingRect.height = me.startRect.bottom() - me.movingRect.y
                    hitMove              = 1 | 2

                case 1:
                    me.movingRect.y      = Math.min(me.movingRect.y + diff.y, me.startRect.bottom() - me.minSizeY)
                    me.movingRect.height = me.startRect.bottom() - me.movingRect.y
                    hitMove              = 2

                case 2:
                    if prop
                    {
                        if Math.abs(diff.x) > Math.abs(diff.y):
                            diff.y = -diff.x
                        else:
                            diff.x = -diff.y
                    }

                    me.movingRect.y      = Math.min(me.startRect.bottom() - me.minSizeY, me.movingRect.y + diff.y)
                    me.movingRect.width  = Math.max(me.movingRect.width + diff.x, me.minSizeX)
                    me.movingRect.height = me.startRect.bottom() - me.movingRect.y
                    hitMove              = 2 | 4

                case 3:
                    me.movingRect.x     = Math.min(me.startRect.right() - me.minSizeX, me.movingRect.x + diff.x)
                    me.movingRect.width = me.startRect.right() - me.movingRect.x
                    hitMove             = 1

                case 4:
                    me.movingRect.width = Math.max(me.movingRect.width + diff.x, me.minSizeX)
                    hitMove             = 4

                case 5:
                    if prop
                    {
                        if Math.abs(diff.x) > Math.abs(diff.y):
                            diff.y = -diff.x
                        else:
                            diff.x = -diff.y
                    }

                    me.movingRect.x      = Math.min(me.startRect.right() - me.minSizeX, me.movingRect.x + diff.x)
                    me.movingRect.width  = me.startRect.right() - me.movingRect.x
                    me.movingRect.height = Math.max(me.movingRect.height + diff.y, me.minSizeY)
                    hitMove              = 1 | 8

                case 6:
                    me.movingRect.height = Math.max(me.movingRect.height + diff.y, me.minSizeY)
                    hitMove              = 8

                case 7:
                    if prop
                    {
                        if Math.abs(diff.x) > Math.abs(diff.y):
                            diff.y = diff.x
                        else:
                            diff.x = diff.y
                    }

                    me.movingRect.width  = Math.max(me.movingRect.width + diff.x, me.minSizeX)
                    me.movingRect.height = Math.max(me.movingRect.height + diff.y, me.minSizeY)
                    hitMove              = 4 | 8
                }

                if me.trackChange and me.sigChanged:
                    me.sigChanged(me, false, hitMove)
            }
            else
            {
                me.hotAnchor = Swag.U32.Max
                foreach c, i in me.anchors where c.contains(pos)
                {
                    me.hotAnchor = i
                    break
                }
            }

            switch me.hotAnchor
            {
            case 0, 7:
                view.getApp().setFrameCursor(Cursor.from(.SizeNWSE))
            case 1, 6:
                view.getApp().setFrameCursor(Cursor.from(.SizeNS))
            case 2, 5:
                view.getApp().setFrameCursor(Cursor.from(.SizeNESW))
            case 3, 4:
                view.getApp().setFrameCursor(Cursor.from(.SizeWE))
            }

            if me.hotAnchor != Swag.U32.Max:
                return true
        }

        return false
    }

    mtd paint(painter: *Painter, hot: bool, col: Color = Argb.White)
    {
        let anchorSize2 = me.anchorSize * 0.5

        let color = Color.getBlend(Argb.Black, col, hot ? 0.8 : 1)

        if me.moving and !me.trackChange:
            painter.drawRect(me.movingRect, Pen.createDash(.Dot, color, 2))

        if !hot
        {
            if me.paintSimpleBorder
            {
                painter.drawRect(me.rect, color, 2)
            }
            elif me.paintBigBorder
            {
                painter.drawRect(me.rect, Argb.DarkGray, 4)
                var rect1 = me.rect
                rect1.inflate(2)
                painter.drawRect(rect1, col, 1)
                var rect2 = me.rect
                rect2.inflate(-2)
                painter.drawRect(rect2, col, 1)
            }
        }

        .anchors[0] = {.rect.x - anchorSize2, .rect.y - anchorSize2, .anchorSize, .anchorSize}
        .anchors[1] = {.rect.horzCenter() - anchorSize2, .rect.y - anchorSize2, .anchorSize, .anchorSize}
        .anchors[2] = {.rect.right() - anchorSize2, .rect.y - anchorSize2, .anchorSize, .anchorSize}
        .anchors[3] = {.rect.x - anchorSize2, .rect.vertCenter() - anchorSize2, .anchorSize, .anchorSize}
        .anchors[4] = {.rect.right() - anchorSize2, .rect.vertCenter() - anchorSize2, .anchorSize, .anchorSize}
        .anchors[5] = {.rect.x - anchorSize2, .rect.bottom() - anchorSize2, .anchorSize, .anchorSize}
        .anchors[6] = {.rect.horzCenter() - anchorSize2, .rect.bottom() - anchorSize2, .anchorSize, .anchorSize}
        .anchors[7] = {.rect.right() - anchorSize2, .rect.bottom() - anchorSize2, .anchorSize, .anchorSize}

        foreach c, i in me.anchors
        {
            if me.roundShape
            {
                painter.fillCircle(c.horzCenter(), c.vertCenter(), anchorSize2, color)
                painter.drawCircle(c.horzCenter(), c.vertCenter(), anchorSize2, Argb.Black, anchorSize2 * 0.3)
            }
            else
            {
                painter.fillRect(c, Argb.White)
                painter.drawRect(c, color, 3)
            }
        }
    }

    mtd apply(iform: IForm, first: bool, hitMove: u32)
    {
        let form    = cast(*BaseForm) @dataof(iform)
        let capture = form.capture
        if !first:
            capture.undo()
        let undo = capture.newUndo(.MoveBaseForm)
        undo.baseForm = iform
        let zoom = capture.editView.zoom

        let dx = cast(s32) ((me.movingRect.x - me.startRect.x) / zoom)
        let dy = cast(s32) ((me.movingRect.y - me.startRect.y) / zoom)
        undo.newPosX = dx
        undo.newPosY = dy
        if hitMove & 1:
            capture.snap(&undo.newPosX, null, false, false)
        if hitMove & 2:
            capture.snap(null, &undo.newPosY, false, false)

        let dsx = cast(s32) ((me.movingRect.width - me.startRect.width) / zoom)
        let dsy = cast(s32) ((me.movingRect.height - me.startRect.height) / zoom)
        undo.newSizeX = dsx
        undo.newSizeY = dsy
        if hitMove & 4:
            capture.snap(&undo.newSizeX, null, true, true)
        if hitMove & 8:
            capture.snap(null, &undo.newSizeY, true, true)

        if hitMove & 1:
            undo.newSizeX -= (undo.newPosX - dx)
        if hitMove & 2:
            undo.newSizeY -= (undo.newPosY - dy)

        capture.pushUndo(undo)
        capture.editView.invalidate()
    }
}
