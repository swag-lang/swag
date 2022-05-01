using Core, Pixel, Gui

struct GizmoRect
{
    rect:           Math.Rectangle
    anchors:        [8] Math.Rectangle
    hotAnchor:      u32 = Swag.U32.Max
    moving:         bool
    movingRect:     Math.Rectangle
    sigChanged:     closure(self)
}

impl GizmoRect
{
    mtd mouse(view: *EditView, evt: *MouseEvent)->bool
    {
        pos := view.surfaceToLocal(evt.surfacePos)
        pos += view.scrollPos

        switch evt.kind
        {
        case MousePressed:
            if evt.button != .Left or hotAnchor == Swag.U32.Max
                break
            moving = true
            movingRect = rect
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
                switch hotAnchor
                {
                case 0:
                    movingRect.x = pos.x
                    movingRect.y = pos.y
                    movingRect.width = rect.right() - pos.x
                    movingRect.height = rect.bottom() - pos.y
                case 1:
                    movingRect.y = pos.y
                    movingRect.height = rect.bottom() - pos.y
                case 2:
                    movingRect.y = pos.y
                    movingRect.width = pos.x - rect.x
                    movingRect.height = rect.bottom() - pos.y
                case 3:
                    movingRect.x = pos.x
                    movingRect.width = rect.right() - pos.x
                case 4:
                    movingRect.width = pos.x - rect.x
                case 5:
                    movingRect.x = pos.x
                    movingRect.width = rect.right() - pos.x
                    movingRect.height = pos.y - rect.y
                case 6:
                    movingRect.height = pos.y - rect.y
                case 7:
                    movingRect.width = pos.x - rect.x
                    movingRect.height = pos.y - rect.y
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

    mtd paint(painter: *Painter)
    {
        const AchorSize = 8'f32
        const AchorSize2 = AchorSize * 0.5

        if moving
            painter.drawRect(movingRect, Pen.createDash(.Dot, Argb.White, 2))
        painter.drawRect(rect, Argb.White, 2)

        anchors[0] = @{rect.x - AchorSize, rect.y - AchorSize, AchorSize, AchorSize}
        anchors[1] = @{rect.horzCenter() - AchorSize, rect.y - AchorSize, AchorSize, AchorSize}
        anchors[2] = @{rect.right(), rect.y - AchorSize, AchorSize, AchorSize}
        anchors[3] = @{rect.x - AchorSize, rect.vertCenter() - AchorSize2, AchorSize, AchorSize}
        anchors[4] = @{rect.right(), rect.vertCenter() - AchorSize2, AchorSize, AchorSize}
        anchors[5] = @{rect.x - AchorSize, rect.bottom(), AchorSize, AchorSize}
        anchors[6] = @{rect.horzCenter() - AchorSize, rect.bottom(), AchorSize, AchorSize}
        anchors[7] = @{rect.right(), rect.bottom(), AchorSize, AchorSize}

        visit c, i: anchors
        {
            painter.fillRect(c, Argb.White)
            painter.drawRect(c, Argb.Black)
        }
    }
}