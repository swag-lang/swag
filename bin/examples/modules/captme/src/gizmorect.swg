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
            if .moving do
                return true
            if evt.button != .Left or .hotAnchor == Swag.U32.Max do
                break
            .moving, .firstMoving = true
            .startRect   = .rect
            .movingRect  = .rect
            .movingStart = pos
            view.captureMouse()
            return true

        case MouseReleased:
            if .moving
            {
                if evt.button == .Left
                {
                    .moving, .firstMoving = false
                    .hotAnchor = Swag.U32.Max
                    view.releaseMouse()
                    if !.trackChange and .sigChanged do
                        .sigChanged(me, false, 0)
                    if .sigHasChanged do
                        .sigHasChanged()
                }

                return true
            }

        case MouseLeave:
            if .hotAnchor != Swag.U32.Max
            {
                .hotAnchor = Swag.U32.Max
                return true
            }

        case MouseMove:
            if .moving
            {
                var hitMove = 0'u32
                if .firstMoving and .sigChanged and .trackChange do
                    .sigChanged(me, true, 0)

                .firstMoving = false

                var diff = pos - .movingStart
                .movingStart = pos

                switch .hotAnchor
                {
                case 0:
                    if prop
                    {
                        if Math.abs(diff.x) > Math.abs(diff.y) do
                            diff.y = diff.x
                        else do
                            diff.x = diff.y
                    }

                    .movingRect.x      = Math.min(.startRect.right() - .minSizeX, .movingRect.x + diff.x)
                    .movingRect.y      = Math.min(.startRect.bottom() - .minSizeY, .movingRect.y + diff.y)
                    .movingRect.width  = .startRect.right() - .movingRect.x
                    .movingRect.height = .startRect.bottom() - .movingRect.y
                    hitMove            = 1 | 2

                case 1:
                    .movingRect.y      = Math.min(.movingRect.y + diff.y, .startRect.bottom() - .minSizeY)
                    .movingRect.height = .startRect.bottom() - .movingRect.y
                    hitMove            = 2

                case 2:
                    if prop
                    {
                        if Math.abs(diff.x) > Math.abs(diff.y) do
                            diff.y = -diff.x
                        else do
                            diff.x = -diff.y
                    }

                    .movingRect.y      = Math.min(.startRect.bottom() - .minSizeY, .movingRect.y + diff.y)
                    .movingRect.width  = Math.max(.movingRect.width + diff.x, .minSizeX)
                    .movingRect.height = .startRect.bottom() - .movingRect.y
                    hitMove            = 2 | 4

                case 3:
                    .movingRect.x     = Math.min(.startRect.right() - .minSizeX, .movingRect.x + diff.x)
                    .movingRect.width = .startRect.right() - .movingRect.x
                    hitMove           = 1

                case 4:
                    .movingRect.width = Math.max(.movingRect.width + diff.x, .minSizeX)
                    hitMove           = 4

                case 5:
                    if prop
                    {
                        if Math.abs(diff.x) > Math.abs(diff.y) do
                            diff.y = -diff.x
                        else do
                            diff.x = -diff.y
                    }

                    .movingRect.x      = Math.min(.startRect.right() - .minSizeX, .movingRect.x + diff.x)
                    .movingRect.width  = .startRect.right() - .movingRect.x
                    .movingRect.height = Math.max(.movingRect.height + diff.y, .minSizeY)
                    hitMove            = 1 | 8

                case 6:
                    .movingRect.height = Math.max(.movingRect.height + diff.y, .minSizeY)
                    hitMove            = 8

                case 7:
                    if prop
                    {
                        if Math.abs(diff.x) > Math.abs(diff.y) do
                            diff.y = diff.x
                        else do
                            diff.x = diff.y
                    }

                    .movingRect.width  = Math.max(.movingRect.width + diff.x, .minSizeX)
                    .movingRect.height = Math.max(.movingRect.height + diff.y, .minSizeY)
                    hitMove            = 4 | 8
                }

                if .trackChange and .sigChanged do
                    .sigChanged(me, false, hitMove)
            }
            else
            {
                .hotAnchor = Swag.U32.Max
                foreach c, i in .anchors where c.contains(pos)
                {
                    .hotAnchor = i
                    break
                }
            }

            switch .hotAnchor
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

            if .hotAnchor != Swag.U32.Max do
                return true
        }

        return false
    }

    mtd paint(painter: *Painter, hot: bool, col: Color = Argb.White)
    {
        let anchorSize2 = .anchorSize * 0.5

        let color = Color.getBlend(Argb.Black, col, hot ? 0.8 : 1)

        if .moving and !.trackChange do
            painter.drawRect(.movingRect, Pen.createDash(.Dot, color, 2))

        if !hot
        {
            if .paintSimpleBorder
            {
                painter.drawRect(.rect, color, 2)
            }
            elif .paintBigBorder
            {
                painter.drawRect(.rect, Argb.DarkGray, 4)
                var rect1 = .rect
                rect1.inflate(2)
                painter.drawRect(rect1, col, 1)
                var rect2 = .rect
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

        foreach c, i in .anchors
        {
            if .roundShape
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
        if !first do
            capture.undo()
        let undo = capture.newUndo(.MoveBaseForm)
        undo.baseForm = iform
        let zoom = capture.editView.zoom

        let dx = cast(s32) ((.movingRect.x - .startRect.x) / zoom)
        let dy = cast(s32) ((.movingRect.y - .startRect.y) / zoom)
        undo.newPosX = dx
        undo.newPosY = dy
        if hitMove & 1 do
            capture.snap(&undo.newPosX, null, false, false)
        if hitMove & 2 do
            capture.snap(null, &undo.newPosY, false, false)

        let dsx = cast(s32) ((.movingRect.width - .startRect.width) / zoom)
        let dsy = cast(s32) ((.movingRect.height - .startRect.height) / zoom)
        undo.newSizeX = dsx
        undo.newSizeY = dsy
        if hitMove & 4 do
            capture.snap(&undo.newSizeX, null, true, true)
        if hitMove & 8 do
            capture.snap(null, &undo.newSizeY, true, true)

        if hitMove & 1 do
            undo.newSizeX -= (undo.newPosX - dx)
        if hitMove & 2 do
            undo.newSizeY -= (undo.newPosY - dy)

        capture.pushUndo(undo)
        capture.editView.invalidate()
    }
}
