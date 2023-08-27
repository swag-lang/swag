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
    mtd impl isSame(ptr: *BaseForm)->bool
    {
        let other = cast(*FormPolygon) ptr
        return  borderColor == other.borderColor and
                borderSize  == other.borderSize and
                backColor   == other.backColor and
                alpha       == other.alpha and
                paintShadow == other.paintShadow
    }

    mtd impl copy(dst, src: *BaseForm)
    {
        let dstf = cast(*FormPolygon) dst
        let srcf = cast(*FormPolygon) src
        dstf.borderColor = srcf.borderColor
        dstf.borderSize  = srcf.borderSize
        dstf.backColor   = srcf.backColor
        dstf.alpha       = srcf.alpha
        dstf.paintShadow = srcf.paintShadow
    }

    mtd impl edit(evt: *MouseEvent, paintRect: Math.Rectangle, movingStart: Math.Point, localPos: Math.Point, zoom: f32)->bool
    {
        if evt.kind == .MouseDoubleClick and evt.button == .Left
        {
            if points.count > 3
                points.removeBack()
            return false
        }

        let x0 = cast(s32) ((localPos.x - paintRect.x) / zoom)
        let y0 = cast(s32) ((localPos.y - paintRect.y) / zoom)

        if points.count < 2
        {
            let xs = cast(s32) ((movingStart.x - paintRect.x) / zoom)
            let ys = cast(s32) ((movingStart.y - paintRect.y) / zoom)
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

    mtd impl moveBaseForm(newPosX, newPosY, newSizeX, newSizeY: s32)
    {
        visit &pt: points
        {
            pt.x += newPosX
            pt.y += newPosY
        }
    }

    mtd impl setBaseForm(newPosX, newPosY, newSizeX, newSizeY: s32)
    {
        var rect = boundRect()
        let fx = newSizeX / rect.width
        let fy = newSizeY / rect.height

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

    mtd impl moveForm(newPosX, newPosY: s32)
    {
        visit &pt: points
        {
            pt.x += newPosX
            pt.y += newPosY
        }
    }

    mtd impl movePoint(pt: u32, newPosX, newPosY: s32)
    {
        points[pt].x += newPosX
        points[pt].y += newPosY
    }

    mtd impl boundRect()->Math.Rectangle
    {
        if !points.count
        {
            points.add({0, 0})
            points.add({48, 0})
            points.add({24, 48})
        }

        var result: retval

        let bSize = borderSize * 0.5
        var x0 = Swag.F32.Max
        var y0 = Swag.F32.Max
        var x1 = 0'f32
        var y1 = 0'f32
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
        var rect = boundRect()

        paintPoints.clear()
        visit pt: points
        {
            let x0 = (pt.x * z) - (rect.x * z) + paintPos.x
            let x1 = (pt.y * z) - (rect.y * z) + paintPos.y
            paintPoints.add({x0, x1})
        }
    }

    mtd impl mouse(view: *EditView, pos: Math.Point, evt: *MouseEvent)->bool
    {
        return gizmo.mouse(view, pos, evt)
    }

    mtd impl paintGizmo(painter: *Painter, z: f32, flags: FormFlags)
    {
        if flags & (.Hot | .Selected)
        {
            computePaintCoords(z)
            gizmo.anchors.clear()
            visit pt: paintPoints
                gizmo.anchors.add({pt.x, pt.y})
            let col = subKind == .PixSel ? Argb.Yellow : Argb.White
            gizmo.paint(null, painter, flags & .Selected ? false : true, col)
        }
    }

    mtd impl isInside(pt: Math.Point)->bool
    {
        return Math.Vector2.polyContains(points, {pt.x, pt.y})
    }

    mtd impl hitTest(pt: Math.Point, z: f32)->bool
    {
        let bSize = borderSize * z * 0.5
        computePaintCoords(z)

        // Just the outline
        if backColor.a == 0
        {
            var v = Math.Vector2{pt.x, pt.y}
            loop i: paintPoints
            {
                let ni = (i + 1) % paintPoints.count
                let p0 = Math.Vector2{paintPoints[i].x, paintPoints[i].y}
                let p1 = Math.Vector2{paintPoints[ni].x, paintPoints[ni].y}
                if v.distanceToSegment(p0, p1) <= bSize
                    return true
            }

            return false
        }

        // Content
        return Math.Vector2.polyContains(paintPoints, {pt.x, pt.y})
    }

    mtd impl paint(app: *Application, painter: *Painter, z: f32, quickMode: bool)
    {
        var bSize = adaptSize(borderSize, MinBorderSize, MaxBorderSize, z, quickMode)
        computePaintCoords(z)

        var backBrush = Brush.createHatch(.CheckBoardLarge, Argb.DarkGray)
        var bk = backColor
        if quickMode and alpha != 100 and subKind == .Highlight
            bk.a = cast(u8) Math.map(cast(f32) alpha, 0, 100, 0, 255)

        if subKind == .PixSel
        {
            bSize = 2
            bk.a = 0
            dashStyle = .Dash
            painter.removeParams(.Antialiased)
        }

        var pen = Pen.createDash(dashStyle, borderColor, bSize)

        linePath.start(paintPoints[0].x - paintPos.x, paintPoints[0].y - paintPos.y)
        loop i: 1 until paintPoints.count
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

    mtd impl setup(capt: *Capture)
    {
        capture = capt
        gizmo.sigChanged = closure|self|(diff, pt, first) {
            GizmoPoints.apply(self, diff, pt, first)
        }
    }
}