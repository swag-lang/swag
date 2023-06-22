struct FormDraw
{
    using base:     BaseForm
    points:         Array'Math.Vector2

    #[Serialization.NoSerialize]
    {
        paintPoints: Array'Math.Vector2
    }
}

impl FormDraw
{
    mtd setDefaultShape()
    {
        points.clear()
        points.add({48, 0})
        points.add({0, 12})
        points.add({34, 28})
        points.add({0, 48})
    }
}

impl IForm for FormDraw
{
    mtd isSame(ptr: *BaseForm)->bool
    {
        other := cast(*FormDraw) ptr
        return  borderColor == other.borderColor and
                borderSize  == other.borderSize and
                alpha       == other.alpha and
                paintShadow == paintShadow
    }

    mtd copy(dst, src: *BaseForm)
    {
        dstf := cast(*FormDraw) dst
        srcf := cast(*FormDraw) src
        dstf.borderColor = srcf.borderColor
        dstf.borderSize  = srcf.borderSize
        dstf.alpha       = srcf.alpha
        dstf.paintShadow = srcf.paintShadow
    }

    mtd edit(evt: *MouseEvent, paintRect: Math.Rectangle, movingStart: Math.Point, localPos: Math.Point, zoom: f32)->bool
    {
        if evt.kind == .MouseReleased and evt.button == .Left
            return false

        x0 := (localPos.x - paintRect.x) / zoom
        y0 := (localPos.y - paintRect.y) / zoom

        if points.count < 2
        {
            xs := (movingStart.x - paintRect.x) / zoom
            ys := (movingStart.y - paintRect.y) / zoom
            points.add({xs, ys})
            points.add({x0, y0})
        }
        elif evt.kind == .MouseMove
        {
            xs := Math.abs(x0 - points.back().x)
            ys := Math.abs(y0 - points.back().y)
            if xs > 5 or ys > 5
                points.add({x0, y0})
        }
        else
        {
            points[points.count - 1] = {x0, y0}
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
        bSize := borderSize * 0.5
        rect.inflate(-bSize)
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
            setDefaultShape()

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
        return false
    }

    mtd paintGizmo(painter: *Painter, z: f32, flags: FormFlags)
    {
        if flags & (.Hot | .Selected)
        {
            const anchorSize = 8
            anchorSize2 := anchorSize * 0.5
            col := subKind == .PixSel ? Argb.Yellow : Argb.White
            color := Color.getBlend(Argb.Black, col, (flags & .Hot) ? 0.8 : 1)

            start := paintPoints[0]
            painter.fillCircle(start.x, start.y, anchorSize2, color)
            painter.drawCircle(start.x, start.y, anchorSize2, Argb.Black, 1)
            end := paintPoints.back()
            painter.fillCircle(end.x, end.y, anchorSize2, color)
            painter.drawCircle(end.x, end.y, anchorSize2, Argb.Black, 1)
        }
    }

    mtd isInside(pt: Math.Point)->bool
    {
        return Math.Vector2.polyContains(points, {pt.x, pt.y})
    }

    mtd hitTest(pt: Math.Point, z: f32)->bool
    {
        bSize := borderSize * z * 0.5
        bSize = Math.max(bSize, 2)

        computePaintCoords(z)

        if subKind == .PixSel
            return Math.Vector2.polyContains(paintPoints, {pt.x, pt.y})

        v := Math.Vector2{pt.x, pt.y}
        loop i: 0..paintPoints.count-2
        {
            if v.distanceToSegment(paintPoints[i], paintPoints[i+1]) <= bSize
                return true
        }

        return false
    }

    mtd paint(app: *Application, painter: *Painter, z: f32, quickMode: bool)
    {
        bSize := adaptSize(borderSize, MinBorderSize, MaxBorderSize, z, quickMode)
        bSize = Math.max(bSize, 2)
        computePaintCoords(z)

        var pl: LinePath
        pl.start(paintPoints[0].x - paintPos.x, paintPoints[0].y - paintPos.y)
        loop i: 1..paintPoints.count-1
            pl.lineTo(paintPoints[i].x - paintPos.x, paintPoints[i].y - paintPos.y)

        if subKind == .PixSel
        {
            pl.close()
            bSize = 2
            dashStyle = .Dash
            painter.removeParams(.Antialiased)
        }

        pen := Pen.createDash(dashStyle, borderColor, bSize)
        pen.joinStyle = .Round

        if quickMode and alpha != 100 and subKind == .Highlight
        {
            pen1 := pen
            pen1.brush = Brush.createHatch(.CheckBoardLarge, Argb.DarkGray)
            painter.drawPath(&pl, pen1)
            pen.brush.color.a = cast(u8) Math.map(cast(f32) alpha, 0, 100, 0, 255)
        }

        painter.drawPath(&pl, pen)
    }

    mtd setup(capt: *Capture)
    {
        capture = capt
    }
}