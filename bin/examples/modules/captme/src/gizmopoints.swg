using Core, Pixel, Gui

struct GizmoPoints
{
    anchorSize:         f32 = 8

    startRect:      Math.Rectangle
    anchors:        Array'Math.Point
    hotAnchor:      u32 = Swag.U32.Max
    moving:         bool
    firstMoving:    bool
    movingRect:     Math.Rectangle
    movingStart:    Math.Point
    sigChanged:     closure(self, bool)

    minSizeX:       f32 = 2
    minSizeY:       f32 = 2
}

impl GizmoPoints
{
    mtd mouse(view: *EditView, pos: Math.Point, evt: *MouseEvent)->bool
    {
        switch evt.kind
        {
        case MousePressed:
            /*if evt.button != .Left or hotAnchor == Swag.U32.Max
                break
            moving, firstMoving = true
            startRect   = rect
            movingRect  = rect
            movingStart = pos
            view.captureMouse()*/
            return true

        case MouseReleased:
            if moving
            {
                /*moving, firstMoving = false
                hotAnchor = Swag.U32.Max
                view.getApp().popCursor()
                view.releaseMouse()
                if !trackChange and sigChanged
                    sigChanged(self, false)*/
                return true
            }

        case MouseMove:
            if moving
            {
                /*if firstMoving and sigChanged and trackChange
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
                return true*/
            }
            else
            {
                if hotAnchor != Swag.U32.Max
                    view.getApp().popCursor()
                hotAnchor = Swag.U32.Max
                visit c, i: anchors
                {
                    var rect: Math.Rectangle
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

    mtd paint(painter: *Painter, hot: bool)
    {
        anchorSize2 := anchorSize * 0.5
        color := Color.getBlend(Argb.Black, Argb.White, hot ? 0.8 : 1)
        visit c: anchors
        {
            painter.fillCircle(c.x, c.y, anchorSize2, color)
        }
    }
}