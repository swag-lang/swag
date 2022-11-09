struct FormLine
{
    using base: BaseForm
    beginCap:   LineCapStyle = .Round
    endCap:     LineCapStyle = .Round
    dashCap:    DashCapStyle = .None
    startSize:  f32 = 4
    endSize:    f32 = 4
    xa, ya, xb, yb: f32

    #[Serialization.NoSerialize]
    {
        gizmo: GizmoPoints
        x0, y0, x1, y1: f32
    }
}

impl IForm for FormLine
{
    mtd isSame(ptr: *BaseForm)->bool
    {
        other := cast(*FormLine) ptr
        return  borderColor == other.borderColor and
                borderSize  == other.borderSize and
                alpha       == other.alpha and
                beginCap    == other.beginCap and
                endCap      == other.endCap and
                dashStyle   == other.dashStyle and
                startSize   == other.startSize and
                endSize     == other.endSize and
                dashCap     == other.dashCap and
                paintShadow == other.paintShadow
    }

    mtd copy(dst, src: *BaseForm)
    {
        dstf := cast(*FormLine) dst
        srcf := cast(*FormLine) src
        dstf.borderColor = srcf.borderColor
        dstf.borderSize  = srcf.borderSize
        dstf.alpha       = srcf.alpha
        dstf.beginCap    = srcf.beginCap
        dstf.endCap      = srcf.endCap
        dstf.dashStyle   = srcf.dashStyle
        dstf.startSize   = srcf.startSize
        dstf.endSize     = srcf.endSize
        dstf.dashCap     = srcf.dashCap
        dstf.paintShadow = srcf.paintShadow
    }

    mtd edit(evt: *MouseEvent, paintRect: Math.Rectangle, movingStart: Math.Point, localPos: Math.Point, zoom: f32)->bool
    {
        xa = cast(s32) ((movingStart.x - paintRect.x) / zoom)
        ya = cast(s32) ((movingStart.y - paintRect.y) / zoom)
        xb = cast(s32) ((localPos.x - paintRect.x) / zoom)
        yb = cast(s32) ((localPos.y - paintRect.y) / zoom)

        xbu := 0
        ybu := 0
        capture.snap(&xbu, &ybu, true, true, 1, cast(IForm) self)
        xb += xbu
        yb += ybu

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

    mtd boundRect(raw: bool)->Math.Rectangle
    {
        var result: retval
        bSize := raw ? 0 : borderSize * 6
        result.x = Math.min(xa, xb) - bSize
        result.y = Math.min(ya, yb) - bSize
        result.width = Math.abs(xb - xa) + bSize * 2
        result.height = Math.abs(yb - ya) + bSize * 2
        return result
    }

    mtd computePaintCoords(z: f32)
    {
        rect := boundRect(false)
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
            gizmo.paint(null, painter, flags & .Selected ? false : true)
        }
    }

    mtd isInside(pt: Math.Point)->bool
    {
        return false
    }

    mtd hitTest(pt: Math.Point, z: f32)->bool
    {
        var painter: Painter{fake: true}
        painter.begin()
        paint(null, &painter, z, false)
        painter.end()

        pos := Math.Vector2{pt.x - paintPos.x, pt.y - paintPos.y}
        visit cmd: painter.commandBuffer
        {
            if cmd.id != .DrawTriangles
                continue
            for i := 0; i < cmd.drawTriangles.count; i += 3
            {
                tri := cmd.drawTriangles.start + i
                x0  := painter.vertexBuffer[tri].pos
                x1  := painter.vertexBuffer[tri+1].pos
                x2  := painter.vertexBuffer[tri+2].pos
                if pos.isInTriangle(x0, x1, x2)
                    return true
            }
        }

        return false
    }

    mtd paint(app: *Application, painter: *Painter, z: f32, quickMode: bool)
    {
        bSize := adaptSize(borderSize, MinBorderSize, MaxBorderSize, z, quickMode)
        computePaintCoords(z)
        pen := Pen.createDash(dashStyle, borderColor, bSize)
        pen.beginCapStyle  = beginCap
        pen.endCapStyle    = endCap
        pen.capStartScaleX = startSize
        pen.capEndScaleX   = endSize
        pen.capStartScaleY = 2
        pen.capEndScaleY   = 2

        if quickMode
        {
            while pen.capStartScaleX > 4 or pen.capEndScaleX > 4
            {
                pen.capStartScaleX *= 0.9
                pen.capEndScaleX *= 0.9
            }
        }

        pen.dashBeginCapStyle = dashCap
        pen.dashEndCapStyle   = dashCap

        painter.drawLine(x0 - paintPos.x, y0 - paintPos.y, x1 - paintPos.x, y1 - paintPos.y, pen)
    }

    mtd setup(capt: *Capture)
    {
        capture = capt
        gizmo.anchors.resize(2)

        gizmo.sigChanged = @|self|(g, diff, pt, first) {
            g.apply(self, diff, pt, first)
        }
    }
}

impl FormLine
{
    func drawCapOffset(cap: LineCapStyle, size, scale: f32)->f32
    {
        switch cap
        {
        case ArrowAnchor:   return -size * 0.5
        case SquareAnchor:  return size * 0.5 * scale
        }

        return 0
    }
}