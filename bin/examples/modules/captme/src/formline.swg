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
    mtd isSame(ptr: *BaseForm)->bool
    {
        other := cast(*FormLine) ptr
        return  borderColor == other.borderColor and
                borderSize  == other.borderSize and
                alpha       == other.alpha
    }

    mtd copy(dst, src: *BaseForm)
    {
        dstf := cast(*FormLine) dst
        srcf := cast(*FormLine) src
        dstf.borderColor = srcf.borderColor
        dstf.borderSize  = srcf.borderSize
        dstf.alpha       = srcf.alpha
    }

    mtd edit(evt: *MouseEvent, paintRect: Math.Rectangle, movingStart: Math.Point, localPos: Math.Point, zoom: f32)->bool
    {
        xa = cast(s32) ((movingStart.x - paintRect.x) / zoom)
        ya = cast(s32) ((movingStart.y - paintRect.y) / zoom)
        xb = cast(s32) ((localPos.x - paintRect.x) / zoom)
        yb = cast(s32) ((localPos.y - paintRect.y) / zoom)
        if evt.kind == .MouseReleased and evt.button == .Left
            return false
        return true
    }

    mtd moveBaseForm(newPosX, newPosY, newSizeX, newSizeY: s32)
    {
        xa, xb += newPosX
        ya, yb += newPosY
    }

    mtd setBaseForm(newPosX, newPosY, newSizeX, newSizeY: s32)
    {
        xa = newPosX
        ya = newPosY
        xb = xa + newSizeX
        yb = ya + newSizeY
    }

    mtd moveForm(newPosX, newPosY: s32)
    {
        xa, xb += newPosX
        ya, yb += newPosY
    }

    mtd movePoint(pt: u32, newPosX, newPosY: s32)
    {
        if pt == 0
        {
            xa += newPosX
            ya += newPosY
        }
        else
        {
            xb += newPosX
            yb += newPosY
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