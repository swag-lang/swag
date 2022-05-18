using Pixel, Core, Gui

struct FormMultiLine
{
    using base:     BaseForm
    gizmo:          GizmoPoints
    points:         Array'Math.Point
    paintPoints:    Array'Math.Point
}

impl IForm for FormMultiLine
{
    mtd edit(evt: *MouseEvent, paintRect: Math.Rectangle, movingStart: Math.Point, localPos: Math.Point, zoom: f32)->bool
    {
        if evt.kind == .MouseReleased and evt.button == .Left
            return false
        return true
    }

    mtd moveBaseForm(newPosX, newPosY, newSizeX, newSizeY: s32)
    {
        visit &pt: points
        {
            pt.x += newPosX
            pt.y += newPosY
        }
    }

    mtd setBaseForm(newPosX, newPosY, newSizeX, newSizeY: s32)
    {
    }

    mtd moveForm(newPosX, newPosY: s32)
    {
        visit &pt: points
        {
            pt.x += newPosX
            pt.y += newPosY
        }
    }

    mtd movePoint(pt: u32, newPosX, newPosY: s32)
    {
        points[pt].x += newPosX
        points[pt].y += newPosY
    }

    mtd boundRect()->Math.Rectangle
    {
        var result: retval

        bSize := borderSize * 0.5
        x0 := Swag.F32.Max
        y0 := Swag.F32.Max
        x1 := 0'f32
        y1 := 0'f32
        visit &pt: points
        {
            x0 = Math.min(result.x, pt.x - bSize)
            y0 = Math.min(result.y, pt.y - bSize)
            x1 = Math.max(x1, pt.x + bSize)
            y1 = Math.max(y1, pt.y + bSize)
        }

        result.x = x0
        result.y = y0
        result.width = x1 - x0
        result.height = y1 - y0
        return result
    }

    mtd computePaintCoords(z: f32)
    {
        rect := boundRect()

        paintPoints.clear()
        visit pt: points
        {
            x0 := (pt.x * z) - (rect.x * z) + paintPos.x
            x1 := (pt.y * z) - (rect.y * z) + paintPos.y
            paintPoints.add(@{x0, x1})
        }
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
            gizmo.anchors.clear()
            visit pt: paintPoints
                gizmo.anchors.add(dref pt)
            gizmo.paint(painter, flags & .Selected ? false : true)
        }
    }

    mtd hitTest(pt: Math.Point, z: f32)->bool
    {
        bSize := borderSize * z * 0.5
        computePaintCoords(z)
        v := Math.Vector2{pt.x, pt.y}

        /*loop i: paintPoints
        {
            p0 := Math.Vector2{paintPoints[i].x, paintPoints[i].y}
            ni := (i + 1) % paintPoints.count
            p1 := Math.Vector2{paintPoints[ni].x, paintPoints[ni].y}
            if v.distanceToLine(p0, p1) < bSize
                return true
        }*/

        count := 0
        loop i: paintPoints
        {
            p0 := Math.Vector2{paintPoints[i].x, paintPoints[i].y}
            ni := (i + 1) % paintPoints.count
            p1 := Math.Vector2{paintPoints[ni].x, paintPoints[ni].y}

            if Math.Vector2.isLineLineIntersect(@{0, 0}, v, p0, p1)
                count += 1
        }

        return count & 1 ? true : false
    }

    mtd paint(painter: *Painter, z: f32)
    {
        bSize := borderSize * z
        computePaintCoords(z)

        var pl: LinePath
        pl.start(paintPoints[0].x - paintPos.x, paintPoints[0].y - paintPos.y)
        loop i: 1..paintPoints.count-1
            pl.lineTo(paintPoints[i].x - paintPos.x, paintPoints[i].y - paintPos.y)
        pl.close()
        painter.drawPath(&pl, borderColor, bSize)
    }

    mtd setup(capt: *Capture)
    {
        capture = capt

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