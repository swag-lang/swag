struct GizmoRect
{
    paintSimpleBorder:  bool = true
    paintBigBorder:     bool = true
    roundShape:         bool = false
    anchorSize:         f32 = 8
    trackChange:        bool = false

    rect:           Math.Rectangle
    startRect:      Math.Rectangle
    anchors:        [8] Math.Rectangle
    hotAnchor:      u32 = Swag.U32.Max
    moving:         bool
    firstMoving:    bool
    movingRect:     Math.Rectangle
    movingStart:    Math.Point
    sigChanged:     closure(self, bool, u32)

    minSizeX:       f32 = 2
    minSizeY:       f32 = 2
}

impl GizmoRect
{
    mtd mouse(view: *EditView, pos: Math.Point, evt: *MouseEvent)->bool
    {
        var prop = (evt.modifiers & .Shift) and !(evt.modifiers & .Control)

        switch evt.kind
        {
        case MousePressed:
            if moving
                return true
            if evt.button != .Left or hotAnchor == Swag.U32.Max
                break
            moving, firstMoving = true
            startRect   = rect
            movingRect  = rect
            movingStart = pos
            view.captureMouse()
            return true

        case MouseReleased:
            if moving
            {
                if evt.button == .Left
                {
                    moving, firstMoving = false
                    hotAnchor = Swag.U32.Max
                    view.getApp().popCursor()
                    view.releaseMouse()
                    if !trackChange and sigChanged
                        sigChanged(self, false, 0)
                }

                return true
            }

        case MouseLeave:
            if hotAnchor != Swag.U32.Max
            {
                view.getApp().popCursor()
                hotAnchor = Swag.U32.Max
                return true
            }

        case MouseMove:
            if moving
            {
                var hitMove = 0'u32
                if firstMoving and sigChanged and trackChange
                    sigChanged(self, true, 0)

                firstMoving = false

                var diff = pos - movingStart
                movingStart = pos

                switch hotAnchor
                {
                case 0:
                    if prop
                    {
                        if Math.abs(diff.x) > Math.abs(diff.y)
                            diff.y = diff.x
                        else
                            diff.x = diff.y
                    }

                    movingRect.x      = Math.min(startRect.right() - minSizeX, movingRect.x + diff.x)
                    movingRect.y      = Math.min(startRect.bottom() - minSizeY, movingRect.y + diff.y)
                    movingRect.width  = startRect.right() - movingRect.x
                    movingRect.height = startRect.bottom() - movingRect.y
                    hitMove = 1 | 2

                case 1:
                    movingRect.y      = Math.min(movingRect.y + diff.y, startRect.bottom() - minSizeY)
                    movingRect.height = startRect.bottom() - movingRect.y
                    hitMove = 2

                case 2:
                    if prop
                    {
                        if Math.abs(diff.x) > Math.abs(diff.y)
                            diff.y = -diff.x
                        else
                            diff.x = -diff.y
                    }

                    movingRect.y      = Math.min(startRect.bottom() - minSizeY, movingRect.y + diff.y)
                    movingRect.width  = Math.max(movingRect.width + diff.x, minSizeX)
                    movingRect.height = startRect.bottom() - movingRect.y
                    hitMove = 2 | 4

                case 3:
                    movingRect.x     = Math.min(startRect.right() - minSizeX, movingRect.x + diff.x)
                    movingRect.width = startRect.right() - movingRect.x
                    hitMove = 1

                case 4:
                    movingRect.width = Math.max(movingRect.width + diff.x, minSizeX)
                    hitMove = 4

                case 5:
                    if prop
                    {
                        if Math.abs(diff.x) > Math.abs(diff.y)
                            diff.y = -diff.x
                        else
                            diff.x = -diff.y
                    }

                    movingRect.x      = Math.min(startRect.right() - minSizeX, movingRect.x + diff.x)
                    movingRect.width  = startRect.right() - movingRect.x
                    movingRect.height = Math.max(movingRect.height + diff.y, minSizeY)
                    hitMove = 1 | 8

                case 6:
                    movingRect.height = Math.max(movingRect.height + diff.y, minSizeY)
                    hitMove = 8

                case 7:
                    if prop
                    {
                        if Math.abs(diff.x) > Math.abs(diff.y)
                            diff.y = diff.x
                        else
                            diff.x = diff.y
                    }

                    movingRect.width  = Math.max(movingRect.width + diff.x, minSizeX)
                    movingRect.height = Math.max(movingRect.height + diff.y, minSizeY)
                    hitMove = 4 | 8
                }

                if trackChange and sigChanged
                    sigChanged(self, false, hitMove)
                return true
            }
            else
            {
                if hotAnchor != Swag.U32.Max
                    view.getApp().popCursor()
                hotAnchor = Swag.U32.Max
                visit c, i: anchors
                {
                    if c.contains(pos)
                    {
                        switch i
                        {
                        case 0, 7: view.getApp().pushCursor(Cursor.from(.SizeNWSE))
                        case 1, 6: view.getApp().pushCursor(Cursor.from(.SizeNS))
                        case 2, 5: view.getApp().pushCursor(Cursor.from(.SizeNESW))
                        case 3, 4: view.getApp().pushCursor(Cursor.from(.SizeWE))
                        }

                        hotAnchor = i
                        return true
                    }
                }
            }
        }

        return false
    }

    mtd paint(painter: *Painter, hot: bool, col: Color = Argb.White)
    {
        var anchorSize2 = anchorSize * 0.5

        var color = Color.getBlend(Argb.Black, col, hot ? 0.8 : 1)

        if moving and !trackChange
            painter.drawRect(movingRect, Pen.createDash(.Dot, color, 2))

        if !hot
        {
            if paintSimpleBorder
                painter.drawRect(rect, color, 2)
            elif paintBigBorder
            {
                painter.drawRect(rect, Argb.DarkGray, 4)
                var rect1 = rect
                rect1.inflate(2)
                painter.drawRect(rect1, col, 1)
                var rect2 = rect
                rect2.inflate(-2)
                painter.drawRect(rect2, col, 1)
            }
        }

        anchors[0] = {rect.x - anchorSize2, rect.y - anchorSize2, anchorSize, anchorSize}
        anchors[1] = {rect.horzCenter() - anchorSize2, rect.y - anchorSize2, anchorSize, anchorSize}
        anchors[2] = {rect.right() - anchorSize2, rect.y - anchorSize2, anchorSize, anchorSize}
        anchors[3] = {rect.x - anchorSize2, rect.vertCenter() - anchorSize2, anchorSize, anchorSize}
        anchors[4] = {rect.right() - anchorSize2, rect.vertCenter() - anchorSize2, anchorSize, anchorSize}
        anchors[5] = {rect.x - anchorSize2, rect.bottom() - anchorSize2, anchorSize, anchorSize}
        anchors[6] = {rect.horzCenter() - anchorSize2, rect.bottom() - anchorSize2, anchorSize, anchorSize}
        anchors[7] = {rect.right() - anchorSize2, rect.bottom() - anchorSize2, anchorSize, anchorSize}

        visit c, i: anchors
        {
            if roundShape
            {
                painter.fillCircle(c.horzCenter(), c.vertCenter(), anchorSize2, color)
                painter.drawCircle(c.horzCenter(), c.vertCenter(), anchorSize2, Argb.Black, 1)
            }
            else
            {
                painter.fillRect(c, color)
                painter.drawRect(c, Argb.Black, 1)
            }
        }
    }

    mtd apply(iform: IForm, first: bool, hitMove: u32)
    {
        var form = cast(*BaseForm) @dataof(iform)
        var capture = form.capture
        if !first
            capture.undo()
        var undo = capture.newUndo(.MoveBaseForm)
        undo.baseForm = iform
        var zoom = capture.editView.zoom

        var dx = cast(s32) ((movingRect.x - startRect.x) / zoom)
        var dy = cast(s32) ((movingRect.y - startRect.y) / zoom)
        undo.newPosX = dx
        undo.newPosY = dy
        if hitMove & 1
            capture.snap(&undo.newPosX, null, false, false)
        if hitMove & 2
            capture.snap(null, &undo.newPosY, false, false)

        var dsx = cast(s32) ((movingRect.width - startRect.width) / zoom)
        var dsy = cast(s32) ((movingRect.height - startRect.height) / zoom)
        undo.newSizeX = dsx
        undo.newSizeY = dsy
        if hitMove & 4
            capture.snap(&undo.newSizeX, null, true, true)
        if hitMove & 8
            capture.snap(null, &undo.newSizeY, true, true)

        if hitMove & 1
            undo.newSizeX -= (undo.newPosX - dx)
        if hitMove & 2
            undo.newSizeY -= (undo.newPosY - dy)

        capture.pushUndo(undo)
        capture.editView.invalidate()
    }
}