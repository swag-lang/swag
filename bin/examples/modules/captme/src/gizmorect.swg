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
    sigChanged:     closure(self, bool)

    minSizeX:       f32 = 2
    minSizeY:       f32 = 2
}

impl GizmoRect
{
    mtd mouse(view: *EditView, pos: Math.Point, evt: *MouseEvent)->bool
    {
        switch evt.kind
        {
        case MousePressed:
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
                moving, firstMoving = false
                hotAnchor = Swag.U32.Max
                view.getApp().popCursor()
                view.releaseMouse()
                if !trackChange and sigChanged
                    sigChanged(self, false)
                return true
            }

        case MouseLeave:
            if hotAnchor != Swag.U32.Max
                view.getApp().popCursor()
            hotAnchor = Swag.U32.Max
            return true

        case MouseMove:
            if moving
            {
                if firstMoving and sigChanged and trackChange
                    sigChanged(self, true)
                firstMoving = false

                diff := pos - movingStart
                movingStart = pos

                switch hotAnchor
                {
                case 0:
                    if evt.modifiers & .Shift
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

                case 1:
                    movingRect.y      = Math.min(movingRect.y + diff.y, startRect.bottom() - minSizeY)
                    movingRect.height = startRect.bottom() - movingRect.y

                case 2:
                    if evt.modifiers & .Shift
                    {
                        if Math.abs(diff.x) > Math.abs(diff.y)
                            diff.y = -diff.x
                        else
                            diff.x = -diff.y
                    }

                    movingRect.y      = Math.min(startRect.bottom() - minSizeY, movingRect.y + diff.y)
                    movingRect.width  = Math.max(movingRect.width + diff.x, minSizeX)
                    movingRect.height = startRect.bottom() - movingRect.y

                case 3:
                    movingRect.x     = Math.min(startRect.right() - minSizeX, movingRect.x + diff.x)
                    movingRect.width = startRect.right() - movingRect.x

                case 4:
                    movingRect.width = Math.max(movingRect.width + diff.x, minSizeX)

                case 5:
                    if evt.modifiers & .Shift
                    {
                        if Math.abs(diff.x) > Math.abs(diff.y)
                            diff.y = -diff.x
                        else
                            diff.x = -diff.y
                    }

                    movingRect.x      = Math.min(startRect.right() - minSizeX, movingRect.x + diff.x)
                    movingRect.width  = startRect.right() - movingRect.x
                    movingRect.height = Math.max(movingRect.height + diff.y, minSizeY)

                case 6:
                    movingRect.height = Math.max(movingRect.height + diff.y, minSizeY)

                case 7:
                    if evt.modifiers & .Shift
                    {
                        if Math.abs(diff.x) > Math.abs(diff.y)
                            diff.y = diff.x
                        else
                            diff.x = diff.y
                    }

                    movingRect.width  = Math.max(movingRect.width + diff.x, minSizeX)
                    movingRect.height = Math.max(movingRect.height + diff.y, minSizeY)
                }

                if trackChange and sigChanged
                    sigChanged(self, false)
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

    mtd paint(painter: *Painter, hot: bool)
    {
        anchorSize2 := anchorSize * 0.5

        color := Color.getBlend(Argb.Black, Argb.White, hot ? 0.8 : 1)

        if moving and !trackChange
            painter.drawRect(movingRect, Pen.createDash(.Dot, color, 2))

        if !hot
        {
            if paintSimpleBorder
                painter.drawRect(rect, color, 2)
            elif paintBigBorder
            {
                painter.drawRect(rect, Argb.DarkGray, 4)
                rect1 := rect
                rect1.inflate(2)
                painter.drawRect(rect1, Argb.White, 1)
                rect2 := rect
                rect2.inflate(-2)
                painter.drawRect(rect2, Argb.White, 1)
            }
        }

        anchors[0] = @{rect.x - anchorSize2, rect.y - anchorSize2, anchorSize, anchorSize}
        anchors[1] = @{rect.horzCenter() - anchorSize2, rect.y - anchorSize2, anchorSize, anchorSize}
        anchors[2] = @{rect.right() - anchorSize2, rect.y - anchorSize2, anchorSize, anchorSize}
        anchors[3] = @{rect.x - anchorSize2, rect.vertCenter() - anchorSize2, anchorSize, anchorSize}
        anchors[4] = @{rect.right() - anchorSize2, rect.vertCenter() - anchorSize2, anchorSize, anchorSize}
        anchors[5] = @{rect.x - anchorSize2, rect.bottom() - anchorSize2, anchorSize, anchorSize}
        anchors[6] = @{rect.horzCenter() - anchorSize2, rect.bottom() - anchorSize2, anchorSize, anchorSize}
        anchors[7] = @{rect.right() - anchorSize2, rect.bottom() - anchorSize2, anchorSize, anchorSize}

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
}