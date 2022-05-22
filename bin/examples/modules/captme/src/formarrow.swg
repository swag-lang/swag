using Pixel, Core, Gui

enum FormArrowShape
{
    Flat
    Arrow
}

struct FormArrow
{
    using base:     BaseForm
    gizmo:          GizmoPoints
    startShape      = FormArrowShape.Flat
    endShape        = FormArrowShape.Arrow
    startSize:      f32 = 5
    endSize:        f32 = 5

    xa, ya, xb, yb: f32
    x0, y0, x1, y1: f32
    points:         Array'Math.Vector2
    paintPoints:    Array'Math.Vector2
}

impl IForm for FormArrow
{
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

        computePoints()
        bSize := borderSize
        px0 := Swag.F32.Max
        py0 := Swag.F32.Max
        px1 := 0'f32
        py1 := 0'f32
        visit &pt: points
        {
            px0 = Math.min(result.x, pt.x - bSize)
            py0 = Math.min(result.y, pt.y - bSize)
            px1 = Math.max(px1, pt.x + bSize)
            py1 = Math.max(py1, pt.y + bSize)
        }

        result.x = px0
        result.y = py0
        result.width = px1 - px0
        result.height = py1 - py0
        return result
    }

    mtd computePoints()
    {
        v0 := Math.Vector2{xa, ya}
        v1 := Math.Vector2{xb, yb}

        n      := v1 - v0
        length := n.length()
        n.normalizeSafe()

        start := n * (borderSize + startSize + startSize * 0.5)
        end   := n * (borderSize + endSize + endSize * 0.5)
        side := n.cross(1)
        leftSide  := side * (borderSize + startSize) * 0.5
        rightSide := side * (borderSize + endSize) * 0.5
        side *= borderSize * 0.5

        points.clear()

        switch startShape
        {
        case Flat:
            points.add(v0 - side)

        case Arrow:
            points.add(v0)
            points.add(v0 + start - side - leftSide)
            points.add(v0 + start - side)
        }

        switch endShape
        {
        case Flat:
            points.add(v1 - side)
            points.add(v1 + side)

        case Arrow:
            points.add(v1 - end - side)
            points.add(v1 - end - side - rightSide)
            points.add(v1)
            points.add(v1 - end + side + rightSide)
            points.add(v1 - end + side)
        }

        switch startShape
        {
        case Flat:
            points.add(v0 + side)

        case Arrow:
            points.add(v0 + start + side)
            points.add(v0 + start + side + leftSide)
        }
    }

    mtd computePaintCoords(z: f32)
    {
        rect := boundRect()
        x0 = (xa * z) - (rect.x * z) + paintPos.x
        y0 = (ya * z) - (rect.y * z) + paintPos.y
        x1 = (xb * z) - (rect.x * z) + paintPos.x
        y1 = (yb * z) - (rect.y * z) + paintPos.y

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
            gizmo.anchors[0] = @{x0, y0}
            gizmo.anchors[1] = @{x1, y1}
            gizmo.paint(painter, flags & .Selected ? false : true)
        }
    }

    mtd hitTest(pt: Math.Point, z: f32)->bool
    {
        bSize := borderSize * z * 0.5
        computePaintCoords(z)
        v := Math.Vector2{pt.x, pt.y}

        // Just the outline
        if backColor.a == 0
        {
            loop i: paintPoints
            {
                p0 := Math.Vector2{paintPoints[i].x, paintPoints[i].y}
                ni := (i + 1) % paintPoints.count
                p1 := Math.Vector2{paintPoints[ni].x, paintPoints[ni].y}
                if v.distanceToLine(p0, p1) < bSize
                    return true
            }

            return false
        }

        // Content
        count := 0
        loop i: paintPoints
        {
            p0 := Math.Vector2{paintPoints[i].x, paintPoints[i].y}
            ni := (i + 1) % paintPoints.count
            p1 := Math.Vector2{paintPoints[ni].x, paintPoints[ni].y}

            res := Math.Vector2.segSegIntersect(@{0, 0}, v, p0, p1)
            if res.yes
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

        painter.fillPath(&pl, backColor)
    }

    mtd setup(capt: *Capture)
    {
        capture = capt
        computePoints()
        gizmo.anchors.clear()

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