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
        me.points.clear()
        me.points.add({0, 10})
        me.points.add({16, 0})
        me.points.add({36, 10})
        me.points.add({48, 32})
        me.points.add({36, 48})
        me.points.add({0, 48})
    }
}

impl IForm for FormPolygon
{
    mtd impl isSame(ptr: *BaseForm)->bool
    {
        let other = cast(*FormPolygon) ptr
        return me.borderColor == other.borderColor and
               me.borderSize == other.borderSize and
               me.backColor == other.backColor and
               me.alpha == other.alpha and
               me.bezier == other.bezier and
               me.paintShadow == other.paintShadow
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
            if me.points.count > 3:
                me.points.removeBack()
            return false
        }

        let x0 = cast(s32) (localPos.x / zoom)
        let y0 = cast(s32) (localPos.y / zoom)

        if me.points.count < 2
        {
            let xs = cast(s32) (movingStart.x / zoom)
            let ys = cast(s32) (movingStart.y / zoom)
            me.points.add({xs, ys})
            me.points.add({x0, y0})
        }
        elif evt.kind == .MouseReleased and evt.button == .Left
        {
            me.points.add({x0, y0})
        }
        else
        {
            me.points[me.points.count - 1].x = x0
            me.points[me.points.count - 1].y = y0
        }

        return true
    }

    mtd impl moveBaseForm(newPosX, newPosY, newSizeX, newSizeY: s32)
    {
        foreach &pt in me.points
        {
            pt.x += newPosX
            pt.y += newPosY
        }
    }

    mtd impl scaleBaseForm(val: f32)
    {
        foreach &pt in me.points
        {
            pt.x, pt.y *= val
        }
    }

    mtd impl setBaseForm(newPosX, newPosY, newSizeX, newSizeY: s32)
    {
        var rect = me.boundRect()
        let fx   = newSizeX / rect.width
        let fy   = newSizeY / rect.height

        foreach &pt in me.points
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
        foreach &pt in me.points
        {
            pt.x += newPosX
            pt.y += newPosY
        }
    }

    mtd impl movePoint(pt: u32, newPosX, newPosY: s32)
    {
        me.points[pt].x += newPosX
        me.points[pt].y += newPosY
    }

    mtd impl boundRect()->Math.Rectangle
    {
        if !me.points.count:
            me.setDefaultShape()

        var result: retval

        var x0 = Swag.F32.Max
        var y0 = Swag.F32.Max
        var x1 = 0'f32
        var y1 = 0'f32
        foreach &pt in me.points
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
        return me.gizmo.mouse(view, pos, evt)
    }

    mtd impl paintGizmo(painter: *Painter, z: f32, flags: FormFlags)
    {
        if flags.has(.Hot | .Selected)
        {
            me.gizmo.anchors.clear()
            foreach pt in me.points:
                me.gizmo.anchors.add({pt.x * z, pt.y * z})
            let col = me.subKind == .PixSel ? Argb.Yellow : Argb.White
            me.gizmo.paint(null, painter, !flags.has(.Selected), col)
        }
    }

    mtd impl isInside(pt: Math.Point)->bool
    {
        return Math.Geometry.polyContains(me.points, {pt.x, pt.y})
    }

    mtd impl hitTest(pt: Math.Point, z: f32)->bool
    {
        return BaseForm.drawHitTest(me, pt, z)
    }

    mtd impl paint(painter: *Painter, cxt: FormPaintContext)
    {
        var dash  = me.dashStyle
        var bSize = cxt.hitTest ? Math.max(me.borderSize, 5) : me.borderSize
        bSize = BaseForm.adaptSize(me.borderSize, MinBorderSize, MaxBorderSize, cxt)

        var backBrush = Brush.createHatch(.CheckBoardLarge, Argb.DarkGray)
        var bk        = me.backColor
        if cxt.quickMode and me.alpha != 100 and me.subKind == .Highlight:
            bk.a = cast(u8) Math.map(cast(f32) me.alpha, 0, 100, 0, 255)

        if me.subKind == .PixSel
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

            bk.a           = 5
            me.borderColor = g_App.theme.colors.btnIcon_Icon
            painter.removeParams(.Antialiased)
        }

        var pen = Pen.createDash(dash, me.borderColor, bSize)

        let z = cxt.z
        me.linePath.start(me.points[0].x * z, me.points[0].y * z)
        for i in 1 until me.points.count
        {
            if me.bezier:
                me.linePath.curveTo(me.points[i].x * z, me.points[i].y * z)
            else:
                me.linePath.lineTo(me.points[i].x * z, me.points[i].y * z)
        }

        me.linePath.close()

        if cxt.quickMode
        {
            if me.alpha != 100 and me.subKind == .Highlight:
                painter.fillPath(&me.linePath, backBrush)
            painter.fillPath(&me.linePath, bk)
        }
        else:
            painter.fillPolygon(&me.linePath, bk)

        pen.joinStyle = me.joinStyle
        painter.drawPath(&me.linePath, pen)
    }

    mtd impl setup(capt: *Capture)
    {
        me.capture          = capt
        me.gizmo.sigChanged = mtd||(diff, pt, first)
        {
            GizmoPoints.apply(me, diff, pt, first)
        }
    }
}
