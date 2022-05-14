using Pixel, Core, Gui

struct FormLine
{
    using base: BaseForm
    gizmo:      GizmoPoints
    xa, ya, xb, yb: f32
}

impl IForm for FormLine
{
    mtd computePaintCoords(z: f32)
    {
        bSize := borderSize * z
        xa = paintPos.x + bSize * 0.5
        ya = paintPos.y + bSize * 0.5
        xb = paintPos.x + paintPos.width - bSize * 0.5
        yb = paintPos.y + paintPos.height - bSize * 0.5
    }

    mtd mouse(view: *EditView, pos: Math.Point, evt: *MouseEvent)->bool
    {
        return gizmo.mouse(view, pos, evt)
    }

    mtd offset(offX, offY: s32)
    {
        x += offX
        y += offY
    }

    mtd paintGizmo(painter: *Painter, z: f32, flags: FormFlags)
    {
        if flags & (.Hot | .Selected)
        {
            computePaintCoords(z)
            gizmo.anchors[0] = @{xa, ya}
            gizmo.anchors[1] = @{xb, yb}
            gizmo.paint(painter, flags & .Selected ? false : true)
        }
    }

    mtd hitTest(pt: Math.Point, z: f32)->bool
    {
        bSize := borderSize * z * 0.5
        v := Math.Vector2{pt.x, pt.y}
        computePaintCoords(z)
        return v.distanceToLine(@{xa, ya}, @{xb, yb}) < bSize
    }

    mtd paint(painter: *Painter, z: f32)
    {
        bSize := borderSize * z
        computePaintCoords(z)
        painter.drawLine(xa, ya, xb, yb, borderColor, bSize)
    }
}

impl FormLine
{
    mtd setup(capt: *Capture)
    {
        capture = capt

        gizmo.anchors.add(@{0, 0})
        gizmo.anchors.add(@{0, 0})

        gizmo.sigChanged = @|self|(g, diff, pt, first) {
            if !first
                self.capture.undo()
            undo := self.capture.newUndo(.MoveBaseForm)
            undo.baseForm = self
            zoom := self.capture.editView.zoom
            if pt == 0
            {
                undo.newPosX = cast(s32) (diff.x / zoom)
                undo.newPosY = cast(s32) (diff.y / zoom)
                undo.newSizeX = -cast(s32) (diff.x / zoom)
                undo.newSizeY = -cast(s32) (diff.y / zoom)
            }
            else
            {
                undo.newSizeX = cast(s32) (diff.x / zoom)
                undo.newSizeY = cast(s32) (diff.y / zoom)
            }

            self.capture.pushUndo(undo)
            self.capture.editView.invalidate()
        }
    }
}