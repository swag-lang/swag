struct FormDraw
{
    using base:     BaseForm
    points:         Array'Math.Vector2
    anchorSize:     f32 = 12
}

impl FormDraw
{
    mtd setDefaultShape()
    {
        me.points.clear()
        me.points.add({48, 0})
        me.points.add({0, 12})
        me.points.add({34, 28})
        me.points.add({0, 48})
    }
}

impl IForm for FormDraw
{
    mtd impl isSame(ptr: *BaseForm)->bool
    {
        let other = cast(*FormDraw) ptr
        return me.borderColor == other.borderColor and
               me.borderSize == other.borderSize and
               me.alpha == other.alpha and
               me.paintShadow == me.paintShadow
    }

    mtd impl copy(dst, src: *BaseForm)
    {
        let dstf = cast(*FormDraw) dst
        let srcf = cast(*FormDraw) src
        dstf.borderColor = srcf.borderColor
        dstf.borderSize  = srcf.borderSize
        dstf.alpha       = srcf.alpha
        dstf.paintShadow = srcf.paintShadow
    }

    mtd impl edit(evt: *MouseEvent, movingStart: Math.Point, localPos: Math.Point, zoom: f32)->bool
    {
        if evt.kind == .MouseReleased and evt.button == .Left:
            return false

        let x0 = localPos.x / zoom
        let y0 = localPos.y / zoom

        if me.points.count < 2
        {
            let xs = movingStart.x / zoom
            let ys = movingStart.y / zoom
            me.points.add({xs, ys})
            me.points.add({x0, y0})
        }
        elif evt.kind == .MouseMove
        {
            let xs = Math.abs(x0 - me.points.back().x)
            let ys = Math.abs(y0 - me.points.back().y)
            if xs > 5 or ys > 5:
                me.points.add({x0, y0})
        }
        else
        {
            me.points[me.points.count - 1] = {x0, y0}
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
            pt.x *= val
            pt.y *= val
        }
    }

    mtd impl setBaseForm(newPosX, newPosY, newSizeX, newSizeY: s32)
    {
        var rect  = me.boundRect()
        let bSize = me.borderSize * 0.5
        rect.inflate(-bSize)
        let fx = newSizeX / rect.width
        let fy = newSizeY / rect.height

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

        let bSize = me.borderSize * 0.5
        var x0    = Swag.F32.Max
        var y0    = Swag.F32.Max
        var x1    = 0'f32
        var y1    = 0'f32
        foreach &pt in me.points
        {
            x0 = Math.min(x0, pt.x - bSize)
            y0 = Math.min(y0, pt.y - bSize)
            x1 = Math.max(x1, pt.x + bSize)
            y1 = Math.max(y1, pt.y + bSize)
        }

        result.x      = x0
        result.y      = y0
        result.width  = x1 - x0
        result.height = y1 - y0
        return result
    }

    mtd impl mouse(view: *EditView, pos: Math.Point, evt: *MouseEvent)->bool
    {
        return false
    }

    mtd impl paintGizmo(painter: *Painter, z: f32, flags: FormFlags)
    {
        if flags.has(.Hot | .Selected)
        {
            let anchorSize2 = me.anchorSize * 0.5
            let col         = me.subKind == .PixSel ? Argb.Yellow : Argb.White
            let color       = Color.getBlend(Argb.Black, col, (flags & .Hot) ? 0.8 : 1)

            let start = me.points[0] * z
            painter.fillCircle(start.x, start.y, anchorSize2, color)
            painter.drawCircle(start.x, start.y, anchorSize2, Argb.Black, anchorSize2 * 0.3)
            let end = me.points.back() * z
            painter.fillCircle(end.x, end.y, anchorSize2, color)
            painter.drawCircle(end.x, end.y, anchorSize2, Argb.Black, anchorSize2 * 0.3)
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
        var bSize = BaseForm.adaptSize(me.borderSize, MinBorderSize, MaxBorderSize, cxt)
        bSize = Math.max(bSize, 2)

        let z   = cxt.z
        var pl: LinePath
        pl.start(me.points[0].x * z, me.points[0].y * z)
        for i in 1 until me.points.count:
            pl.lineTo(me.points[i].x * z, me.points[i].y * z)

        if me.subKind == .PixSel
        {
            pl.close()

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

            me.borderColor = Argb.White
            painter.removeParams(.Antialiased)
            me.backColor.a = 5
            painter.fillPath(&pl, me.backColor)
        }

        var pen = Pen.createDash(dash, me.borderColor, bSize)
        pen.joinStyle = .Round

        if cxt.quickMode and me.alpha != 100 and me.subKind == .Highlight
        {
            var pen1 = pen
            pen1.brush = Brush.createHatch(.CheckBoardLarge, Argb.DarkGray)
            painter.drawPath(&pl, pen1)
            pen.brush.color.a = cast(u8) Math.map(cast(f32) me.alpha, 0, 100, 0, 255)
        }

        painter.drawPath(&pl, pen)
    }

    mtd impl setup(capt: *Capture)
    {
        me.capture = capt
    }
}
