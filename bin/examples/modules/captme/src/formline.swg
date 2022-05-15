using Pixel, Core, Gui

struct FormLine
{
    using base: BaseForm
    gizmo:      GizmoPoints
    xa, ya, xb, yb: f32
    x0, y0, x1, y1: f32
}

impl IForm for FormLine
{
    mtd moveForm(diff: Math.Point)
    {
        xa, xb += cast(s32) diff.x
        ya, yb += cast(s32) diff.y
    }

    mtd movePoint(pt: u32, diff: Math.Point)
    {
        if pt == 0
        {
            xa += diff.x
            ya += diff.y
        }
        else
        {
            xb += diff.x
            yb += diff.y
        }
    }

    mtd boundRect()->Math.Rectangle
    {
        var result: retval
        bSize := borderSize * 0.5
        result.x = Math.min(xa, xb) - bSize
        result.y = Math.min(ya, yb) - bSize
        result.width = Math.abs(xb - xa) + bSize * 2
        result.height = Math.abs(yb - ya) + bSize * 2
        return result
    }

    mtd computePaintCoords(z: f32)
    {
        rect := boundRect()
        x0 = (xa * z) - (rect.x * z) + paintPos.x
        y0 = (ya * z) - (rect.y * z) + paintPos.y
        x1 = (xb * z) - (rect.x * z) + paintPos.x
        y1 = (yb * z) - (rect.y * z) + paintPos.y
    }

    mtd mouse(view: *EditView, pos: Math.Point, evt: *MouseEvent)->bool
    {
        return gizmo.mouse(view, pos, evt)
    }

    mtd paintGizmo(painter: *Painter, z: f32, flags: FormFlags)
    {
        if flags & (.Hot | .Selected)
        {
            computePaintCoords(z)
            gizmo.anchors[0] = @{x0, y0}
            gizmo.anchors[1] = @{x1, y1}
            gizmo.paint(painter, flags & .Selected ? false : true)
        }
    }

    mtd hitTest(pt: Math.Point, z: f32)->bool
    {
        bSize := borderSize * z * 0.5
        v := Math.Vector2{pt.x, pt.y}
        computePaintCoords(z)
        return v.distanceToLine(@{x0, y0}, @{x1, y1}) < bSize
    }

    mtd paint(painter: *Painter, z: f32)
    {
        bSize := borderSize * z
        computePaintCoords(z)
        painter.drawLine(x0 - paintPos.x, y0 - paintPos.y, x1 - paintPos.x, y1 - paintPos.y, borderColor, bSize)
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
            undo := self.capture.newUndo(.MovePoint)
            zoom := self.capture.editView.zoom
            undo.newPosX = cast(s32) (diff.x / zoom)
            undo.newPosY = cast(s32) (diff.y / zoom)
            undo.pointIdx = pt
            undo.baseForm = self
            self.capture.pushUndo(undo)
            self.capture.editView.invalidate()
        }
    }
}