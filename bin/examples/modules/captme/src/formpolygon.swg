struct FormPolygon
{
    using base:     BaseForm
    points:         Array'Math.Vector2

    #[Serialization.NoSerialize]
    {
        gizmo:          GizmoPoints
        paintPoints:    Array'Math.Vector2
        linePath:       LinePath
    }
}

impl FormPolygon
{
    mtd setDefaultShape()
    {
        points.clear()
        points.add({0, 0})
        points.add({48, 0})
        points.add({24, 48})
    }
}

impl IForm for FormPolygon
{
    mtd isSame(ptr: *BaseForm)->bool
    {
        other := cast(*FormPolygon) ptr
        return  borderColor == other.borderColor and
                borderSize  == other.borderSize and
                backColor   == other.backColor and
                alpha       == other.alpha and
                paintShadow == other.paintShadow
    }

    mtd copy(dst, src: *BaseForm)
    {
        dstf := cast(*FormPolygon) dst
        srcf := cast(*FormPolygon) src
        dstf.borderColor = srcf.borderColor
        dstf.borderSize  = srcf.borderSize
        dstf.backColor   = srcf.backColor
        dstf.alpha       = srcf.alpha
        dstf.paintShadow = srcf.paintShadow
    }

    mtd edit(evt: *MouseEvent, paintRect: Math.Rectangle, movingStart: Math.Point, localPos: Math.Point, zoom: f32)->bool
    {
        if evt.kind == .MouseDoubleClick and evt.button == .Left
        {
            if points.count > 3
                points.removeBack()
            return false
        }

        x0 := cast(s32) ((localPos.x - paintRect.x) / zoom)
        y0 := cast(s32) ((localPos.y - paintRect.y) / zoom)

        if points.count < 2
        {
            xs := cast(s32) ((movingStart.x - paintRect.x) / zoom)
            ys := cast(s32) ((movingStart.y - paintRect.y) / zoom)
            points.add({xs, ys})
            points.add({x0, y0})
        }
        elif evt.kind == .MouseReleased and evt.button == .Left
        {
            points.add({x0, y0})
        }
        else
        {
            points[points.count - 1].x = x0
            points[points.count - 1].y = y0
        }

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
        rect := boundRect()
        fx := newSizeX / rect.width
        fy := newSizeY / rect.height

        visit &pt: points
        {
            pt.x -= rect.x
            pt.y -= rect.y
            pt.x *= fx
            pt.y *= fy
            pt.x += newPosX
            pt.y += newPosY
        }
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
        if !points.count
        {
            points.add({0, 0})
            points.add({48, 0})
            points.add({24, 48})
        }

        var result: retval

        bSize := borderSize * 0.5
        x0 := Swag.F32.Max
        y0 := Swag.F32.Max
        x1 := 0'f32
        y1 := 0'f32
        visit &pt: points
        {
            x0 = Math.min(x0, pt.x - bSize)
            y0 = Math.min(y0, pt.y - bSize)
            x1 = Math.max(x1, pt.x + bSize)
            y1 = Math.max(y1, pt.y + bSize)
        }

        result.x = x0
        result.y = y0
        result.width = x1 - x0
        result.height = y1 - y0

        if joinStyle == .Miter or joinStyle == .MiterBevel
            result.inflate(borderSize * 3) // Magic number !

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
            paintPoints.add({x0, x1})
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
                gizmo.anchors.add({pt.x, pt.y})
            col := subKind == .PixSel ? Argb.Yellow : Argb.White
            gizmo.paint(null, painter, flags & .Selected ? false : true, col)
        }
    }

    mtd isInside(pt: Math.Point)->bool
    {
        return Math.Vector2.polyContains(points, {pt.x, pt.y})
    }

    mtd hitTest(pt: Math.Point, z: f32)->bool
    {
        bSize := borderSize * z * 0.5
        computePaintCoords(z)

        // Just the outline
        if backColor.a == 0
        {
            v := Math.Vector2{pt.x, pt.y}
            loop i: paintPoints
            {
                ni := (i + 1) % paintPoints.count
                p0 := Math.Vector2{paintPoints[i].x, paintPoints[i].y}
                p1 := Math.Vector2{paintPoints[ni].x, paintPoints[ni].y}
                if v.distanceToSegment(p0, p1) <= bSize
                    return true
            }

            return false
        }

        // Content
        return Math.Vector2.polyContains(paintPoints, {pt.x, pt.y})
    }

    mtd paint(app: *Application, painter: *Painter, z: f32, quickMode: bool)
    {
        bSize := adaptSize(borderSize, MinBorderSize, MaxBorderSize, z, quickMode)
        computePaintCoords(z)

        backBrush := Brush.createHatch(.CheckBoardLarge, Argb.DarkGray)
        bk := backColor
        if quickMode and alpha != 100 and subKind == .Highlight
            bk.a = cast(u8) Math.map(cast(f32) alpha, 0, 100, 0, 255)

        if subKind == .PixSel
        {
            bSize = 2
            bk.a = 0
            dashStyle = .Dash
            painter.removeParams(.Antialiased)
        }

        pen := Pen.createDash(dashStyle, borderColor, bSize)

        linePath.start(paintPoints[0].x - paintPos.x, paintPoints[0].y - paintPos.y)
        loop i: 1..paintPoints.count-1
            linePath.lineTo(paintPoints[i].x - paintPos.x, paintPoints[i].y - paintPos.y)
        linePath.close()

        if quickMode
        {
            if alpha != 100 and subKind == .Highlight
                painter.fillPath(&linePath, backBrush)
            painter.fillPath(&linePath, bk)
        }
        else
            painter.fillPolygon(&linePath, bk)

        pen.joinStyle = joinStyle
        painter.drawPath(&linePath, pen)
    }

    mtd setup(capt: *Capture)
    {
        capture = capt
        gizmo.sigChanged = closure|self|(diff, pt, first) {
            GizmoPoints.apply(self, diff, pt, first)
        }
    }
}