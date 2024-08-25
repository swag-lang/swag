struct GizmoPoints
{
    anchorSize:      f32 = 8

    startRect:       Math.Rectangle
    anchors:         Array'Math.Point
    hotAnchor:       u32 = Swag.U32.Max
    moving:          bool
    firstMoving:     bool
    movingRect:      Math.Rectangle
    movingStart:     Math.Point
    sigChanged:      closure(Math.Point, u32, bool)

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
            if evt.button != .Left or hotAnchor == Swag.U32.Max:
                break
            moving, firstMoving = true
            movingStart = pos
            view.captureMouse()
            return true

        case MouseReleased:
            if moving
            {
                moving, firstMoving = false
                hotAnchor = Swag.U32.Max
                view.getApp().popCursor()
                view.releaseMouse()
                return true
            }

        case MouseMove:
            if moving
            {
                var diff = pos - movingStart
                if firstMoving and sigChanged:
                    sigChanged(diff, hotAnchor, true)
                firstMoving = false
                if sigChanged:
                    sigChanged(diff, hotAnchor, false)
                return true
            }
            else
            {
                if hotAnchor != Swag.U32.Max:
                    view.getApp().popCursor()
                hotAnchor = Swag.U32.Max

                var rect: Math.Rectangle = undefined
                foreach c, i in anchors
                {
                    rect.x = c.x - anchorSize * 0.5
                    rect.y = c.y - anchorSize * 0.5
                    rect.width, rect.height = anchorSize
                    if rect.contains(pos)
                    {
                        view.getApp().pushCursor(Cursor.from(.SizeNWSE))
                        hotAnchor = cast(u32) i
                        return true
                    }
                }
            }
        }

        return false
    }

    mtd paint(_app: *Application, painter: *Painter, hot: bool, col: Color = Argb.White)
    {
        let anchorSize2 = anchorSize * 0.5
        let color       = Color.getBlend(Argb.Black, col, hot ? 0.8 : 1)
        foreach c in anchors
        {
            painter.fillCircle(c.x, c.y, anchorSize2, color)
            painter.drawCircle(c.x, c.y, anchorSize2, Argb.Black, 1)
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
