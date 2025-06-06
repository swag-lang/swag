struct FormPolygon
{
    using base:     BaseForm
    points:         Array'Math.Vector2

    #[Serialization.NoSerialize]
    {
        gizmo:        GizmoPoints
        linePath:     LinePath
    }
}

impl FormPolygon
{
    mtd setDefaultShape()
    {
        points.clear()
        points.add({0, 10})
        points.add({16, 0})
        points.add({36, 10})
        points.add({48, 32})
        points.add({36, 48})
        points.add({0, 48})
    }
}

impl IForm for FormPolygon
{
    mtd impl isSame(ptr: *BaseForm)->bool
    {
        let other = cast(*FormPolygon) ptr
        return borderColor == other.borderColor and
               borderSize == other.borderSize and
               backColor == other.backColor and
               alpha == other.alpha and
               bezier == other.bezier and
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
        dstf.bezier      = srcf.bezier
    }

    mtd impl edit(evt: *MouseEvent, movingStart: Math.Point, localPos: Math.Point, zoom: f32)->bool
    {
        if evt.kind == .MouseDoubleClick and evt.button == .Left
        {
            if points.count > 3:
                points.removeBack()
            return false
        }

        let x0 = cast(s32) (localPos.x / zoom)
        let y0 = cast(s32) (localPos.y / zoom)

        if points.count < 2
        {
            let xs = cast(s32) (movingStart.x / zoom)
            let ys = cast(s32) (movingStart.y / zoom)
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
        foreach &pt in points
        {
            pt.x += newPosX
            pt.y += newPosY
        }
    }

    mtd impl setBaseForm(newPosX, newPosY, newSizeX, newSizeY: s32)
    {
        var rect = boundRect()
        let fx   = newSizeX / rect.width
        let fy   = newSizeY / rect.height

        foreach &pt in points
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
        foreach &pt in points
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
        if !points.count:
            setDefaultShape()

        var result: retval

        var x0    = Swag.F32.Max
        var y0    = Swag.F32.Max
        var x1    = 0'f32
        var y1    = 0'f32
        foreach &pt in points
        {
            x0 = Math.min(x0, pt.x)
            y0 = Math.min(y0, pt.y)
            x1 = Math.max(x1, pt.x)
            y1 = Math.max(y1, pt.y)
        }

        result.x      = x0
        result.y      = y0
        result.width  = x1 - x0
        result.height = y1 - y0

        return result
    }

    mtd impl mouse(view: *EditView, pos: Math.Point, evt: *MouseEvent)->bool
    {
        return gizmo.mouse(view, pos, evt)
    }

    mtd impl paintGizmo(painter: *Painter, z: f32, flags: FormFlags)
    {
        if flags.has(.Hot | .Selected)
        {
            gizmo.anchors.clear()
            foreach pt in points:
                gizmo.anchors.add({pt.x * z, pt.y * z})
            let col = subKind == .PixSel ? Argb.Yellow : Argb.White
            gizmo.paint(null, painter, !flags.has(.Selected), col)
        }
    }

    mtd impl isInside(pt: Math.Point)->bool
    {
        return Math.Vector2.polyContains(points, {pt.x, pt.y})
    }

    mtd impl hitTest(pt: Math.Point, z: f32)->bool
    {
        return BaseForm.drawHitTest(self, pt, z)
    }

    mtd impl paint(painter: *Painter, cxt: FormPaintContext)
    {
        var dash  = dashStyle
        var bSize = cxt.hitTest ? Math.max(borderSize, 5) : borderSize
        bSize = adaptSize(borderSize, MinBorderSize, MaxBorderSize, cxt)

        var backBrush = Brush.createHatch(.CheckBoardLarge, Argb.DarkGray)
        var bk        = backColor
        if cxt.quickMode and alpha != 100 and subKind == .Highlight:
            bk.a = cast(u8) Math.map(cast(f32) alpha, 0, 100, 0, 255)

        if subKind == .PixSel
        {
            if cxt.hitTest
            {
                bSize = 10
                dash  = .None
            }
            else
            {
                bSize = 2
                dash  = .Dash
            }

            bk.a = 5
            borderColor = Argb.White
            painter.removeParams(.Antialiased)
        }

        var pen = Pen.createDash(dash, borderColor, bSize)

        let z = cxt.z
        linePath.start(points[0].x * z, points[0].y * z)
        for i in 1 until points.count
        {
            if bezier:
                linePath.curveTo(points[i].x * z, points[i].y * z)
            else:
                linePath.lineTo(points[i].x * z, points[i].y * z)
        }

        linePath.close()

        if cxt.quickMode
        {
            if alpha != 100 and subKind == .Highlight:
                painter.fillPath(&linePath, backBrush)
            painter.fillPath(&linePath, bk)
        }
        else:
            painter.fillPolygon(&linePath, bk)

        pen.joinStyle = joinStyle
        painter.drawPath(&linePath, pen)
    }

    mtd impl setup(capt: *Capture)
    {
        capture          = capt
        gizmo.sigChanged = func|self|(diff, pt, first)
        {
            GizmoPoints.apply(self, diff, pt, first)
        }
    }
}
