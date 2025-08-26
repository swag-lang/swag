struct GizmoPoints
{
    anchorSize:      f32 = 12

    startRect:       Math.Rectangle
    anchors:         Array'Math.Point
    hotAnchor:       u32 = Swag.U32.Max
    moving:          bool
    firstMoving:     bool
    movingRect:      Math.Rectangle
    movingStart:     Math.Point
    sigChanged:      func||(Math.Point, u32, bool)

    minSizeX:        f32 = 2
    minSizeY:        f32 = 2
}

impl GizmoPoints
{
    mtd mouse(view: *EditView, pos: Math.Point, evt: *MouseEvent)->bool
    {
        switch evt.kind
        {
        case MousePressed:
            if me.moving:
                return true
            if evt.button != .Left or me.hotAnchor == Swag.U32.Max:
                break
            me.moving, me.firstMoving = true
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
                }

                return true
            }

        case MouseMove:
            if me.moving
            {
                var diff = pos - me.movingStart
                if me.firstMoving and me.sigChanged:
                    me.sigChanged(diff, me.hotAnchor, true)
                me.firstMoving = false
                if me.sigChanged:
                    me.sigChanged(diff, me.hotAnchor, false)
            }
            else
            {
                me.hotAnchor = Swag.U32.Max

                var rect: Math.Rectangle = undefined
                foreach c, i in me.anchors
                {
                    rect.x = c.x - anchorSize * 0.5
                    rect.y = c.y - anchorSize * 0.5
                    rect.width, rect.height = anchorSize
                    if rect.contains(pos)
                    {
                        hotAnchor = cast(u32) i
                        break
                    }
                }
            }

            if me.hotAnchor != Swag.U32.Max
            {
                view.getApp().setFrameCursor(Cursor.from(.SizeNWSE))
                return true
            }
        }

        return false
    }

    mtd paint(_app: #null *Application, painter: *Painter, hot: bool, col: Color = Argb.White)
    {
        let anchorSize2 = me.anchorSize * 0.5
        let color       = Color.getBlend(Argb.Black, col, hot ? 0.8 : 1)
        foreach c in me.anchors
        {
            painter.fillCircle(c.x, c.y, anchorSize2, color)
            painter.drawCircle(c.x, c.y, anchorSize2, Argb.Black, anchorSize2 * 0.3)
        }
    }

    func apply(iform: IForm, diff: Math.Point, pt: u32, first: bool)
    {
        let form    = cast(*BaseForm) @dataof(iform)
        let capture = form.capture
        if !first:
            capture.undo()
        let undo = capture.newUndo(.MovePoint)
        let zoom = capture.editView.zoom

        undo.newPosX = cast(s32) (diff.x / zoom)
        undo.newPosY = cast(s32) (diff.y / zoom)
        capture.snap(&undo.newPosX, &undo.newPosY, pt > 0, pt > 0, pt)

        undo.pointIdx = pt
        undo.baseForm = iform
        capture.pushUndo(undo)
        capture.editView.invalidate()
    }
}
