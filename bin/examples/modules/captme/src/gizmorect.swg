using Core, Pixel, Gui

struct GizmoRect
{
    paintBorder:    bool = true
    roundShape:     bool = false
    anchorSize:     f32 = 8

    rect:           Math.Rectangle
    anchors:        [8] Math.Rectangle
    hotAnchor:      u32 = Swag.U32.Max
    moving:         bool
    movingRect:     Math.Rectangle
    movingStart:    Math.Point
    sigChanged:     closure(self)
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
            moving = true
            movingRect = rect
            movingStart = pos
            view.captureMouse()
            return true

        case MouseReleased:
            if moving
            {
                moving = false
                hotAnchor = Swag.U32.Max
                view.getApp().popCursor()
                view.releaseMouse()
                if sigChanged
                    sigChanged(self)
                return true
            }

        case MouseMove:
            if moving
            {
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
                    movingRect.x += diff.x
                    movingRect.y += diff.y
                    movingRect.width -= diff.x
                    movingRect.height -= diff.y
                case 1:
                    movingRect.y += diff.y
                    movingRect.height -= diff.y
                case 2:
                    if evt.modifiers & .Shift
                    {
                        if Math.abs(diff.x) > Math.abs(diff.y)
                            diff.y = -diff.x
                        else
                            diff.x = -diff.y
                    }
                    movingRect.y += diff.y
                    movingRect.width += diff.x
                    movingRect.height -= diff.y

                case 3:
                    movingRect.x = pos.x
                    movingRect.width = rect.right() - pos.x
                case 4:
                    movingRect.width = pos.x - rect.x

                case 5:
                    if evt.modifiers & .Shift
                    {
                        if Math.abs(diff.x) > Math.abs(diff.y)
                            diff.y = -diff.x
                        else
                            diff.x = -diff.y
                    }
                    movingRect.x += diff.x
                    movingRect.width -= diff.x
                    movingRect.height += diff.y
                case 6:
                    movingRect.height = pos.y - rect.y
                case 7:
                    if evt.modifiers & .Shift
                    {
                        if Math.abs(diff.x) > Math.abs(diff.y)
                            diff.y = diff.x
                        else
                            diff.x = diff.y
                    }
                    movingRect.width += diff.x
                    movingRect.height += diff.y
                }

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

    mtd paint(painter: *Painter, alpha: f32 = 1)
    {
        anchorSize2 := anchorSize * 0.5
        color := Color.getBlend(Argb.Black, Argb.White, alpha)

        if moving
            painter.drawRect(movingRect, Pen.createDash(.Dot, color, 2))

        if paintBorder
            painter.drawRect(rect, color, 2)

        anchors[0] = @{rect.x - anchorSize, rect.y - anchorSize, anchorSize, anchorSize}
        anchors[1] = @{rect.horzCenter() - anchorSize2, rect.y - anchorSize, anchorSize, anchorSize}
        anchors[2] = @{rect.right(), rect.y - anchorSize, anchorSize, anchorSize}
        anchors[3] = @{rect.x - anchorSize, rect.vertCenter() - anchorSize2, anchorSize, anchorSize}
        anchors[4] = @{rect.right(), rect.vertCenter() - anchorSize2, anchorSize, anchorSize}
        anchors[5] = @{rect.x - anchorSize, rect.bottom(), anchorSize, anchorSize}
        anchors[6] = @{rect.horzCenter() - anchorSize2, rect.bottom(), anchorSize, anchorSize}
        anchors[7] = @{rect.right(), rect.bottom(), anchorSize, anchorSize}

        visit c, i: anchors
        {
            if roundShape
                painter.fillCircle(c.horzCenter(), c.vertCenter(), anchorSize2, color)
            else
                painter.fillRect(c, color)
        }
    }
}