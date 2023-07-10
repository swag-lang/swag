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
    mtd impl isSame(ptr: *BaseForm)->bool
    {
        let other = cast(*FormLine) ptr
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

    mtd impl copy(dst, src: *BaseForm)
    {
        let dstf = cast(*FormLine) dst
        let srcf = cast(*FormLine) src
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

    mtd impl edit(evt: *MouseEvent, paintRect: Math.Rectangle, movingStart: Math.Point, localPos: Math.Point, zoom: f32)->bool
    {
        xa = cast(s32) ((movingStart.x - paintRect.x) / zoom)
        ya = cast(s32) ((movingStart.y - paintRect.y) / zoom)
        xb = cast(s32) ((localPos.x - paintRect.x) / zoom)
        yb = cast(s32) ((localPos.y - paintRect.y) / zoom)

        var xbu = 0
        var ybu = 0
        capture.snap(&xbu, &ybu, true, true, 1, cast(IForm) self)
        xb += xbu
        yb += ybu

        if evt.kind == .MouseReleased and evt.button == .Left
            return false
        return true
    }

    mtd impl moveBaseForm(newPosX, newPosY, newSizeX, newSizeY: s32)
    {
        xa, xb += newPosX
        ya, yb += newPosY
    }

    mtd impl setBaseForm(newPosX, newPosY, newSizeX, newSizeY: s32)
    {
        xa = newPosX
        ya = newPosY
        xb = xa + newSizeX
        yb = ya + newSizeY
    }

    mtd impl moveForm(newPosX, newPosY: s32)
    {
        xa, xb += newPosX
        ya, yb += newPosY
    }

    mtd impl movePoint(pt: u32, newPosX, newPosY: s32)
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

    mtd impl boundRect()->Math.Rectangle
    {
        var result: retval

        var va = Math.Vector2{xa, ya}
        var vb = Math.Vector2{xb, yb}

        func bound(va, vb, da, db: Math.Vector2)->Math.Vector4
        {
            var va1 = va + da
            var va2 = va - da
            var vb1 = vb + db
            var vb2 = vb - db

            var result: retval
            result.x = Math.min(va1.x, va2.x, vb1.x, vb2.x)
            result.y = Math.min(va1.y, va2.y, vb1.y, vb2.y)
            result.z = Math.max(va1.x, va2.x, vb1.x, vb2.x)
            result.w = Math.max(va1.y, va2.y, vb1.y, vb2.y)
            return result
        }

        var (w0, l0) = Painter.capStyleExtent(beginCap, borderSize, startSize)
        var (w1, l1) = Painter.capStyleExtent(endCap, borderSize, endSize)

        var d = vb - va
        d.normalizeSafe()
        var d1 = d.cross(1)
        d1.normalizeSafe()

        if beginCap == .ArrowAnchor
            l0 = 2*2*borderSize
        if endCap == .ArrowAnchor
            l1 = 2*2*borderSize

        var v1 = bound(va, vb, d*l0 + d1*w0, d*l1 + d1*w1)
        var v2 = bound(va, vb, d*l0 - d1*w0, d*l1 - d1*w1)

        v1.x = Math.min(v1.x, v2.x)
        v1.y = Math.min(v1.y, v2.y)
        v1.z = Math.max(v1.z, v2.z)
        v1.w = Math.max(v1.w, v2.w)

        /////
        result.x = v1.x
        result.y = v1.y
        result.width = v1.z - v1.x
        result.height = v1.w - v1.y

        return result
    }

    mtd computePaintCoords(z: f32)
    {
        var rect = boundRect()
        x0 = (xa * z) - (rect.x * z) + paintPos.x
        y0 = (ya * z) - (rect.y * z) + paintPos.y
        x1 = (xb * z) - (rect.x * z) + paintPos.x
        y1 = (yb * z) - (rect.y * z) + paintPos.y
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
            gizmo.anchors[0] = {x0, y0}
            gizmo.anchors[1] = {x1, y1}
            gizmo.paint(null, painter, flags & .Selected ? false : true)
        }
    }

    mtd impl isInside(pt: Math.Point)->bool
    {
        return false
    }

    mtd impl hitTest(pt: Math.Point, z: f32)->bool
    {
        var painter: Painter{fake = true}
        painter.begin()
        paint(null, &painter, z, false)
        painter.end()

        var pos = Math.Vector2{pt.x - paintPos.x, pt.y - paintPos.y}
        visit cmd: painter.commandBuffer
        {
            if cmd.id != .DrawTriangles
                continue
            for var i = 0; i < cmd.drawTriangles.count; i += 3
            {
                let tri = cmd.drawTriangles.start + i
                let xx0 = painter.vertexBuffer[tri].pos
                let xx1 = painter.vertexBuffer[tri+1].pos
                let xx2 = painter.vertexBuffer[tri+2].pos
                if pos.isInTriangle(xx0, xx1, xx2)
                    return true
            }
        }

        return false
    }

    mtd impl paint(app: *Application, painter: *Painter, z: f32, quickMode: bool)
    {
        computePaintCoords(z)

        let bSize = adaptSize(borderSize, MinBorderSize, MaxBorderSize, z, quickMode)
        let sSize = quickMode ? Math.min(startSize, 4) : startSize
        let eSize = quickMode ? Math.min(endSize, 4) : endSize

        let pen = Pen.createDash(dashStyle, borderColor, bSize)
        pen.beginCapStyle  = beginCap
        pen.endCapStyle    = endCap
        pen.capStartScaleX = sSize
        pen.capEndScaleX   = eSize
        pen.capStartScaleY = 2
        pen.capEndScaleY   = 2

        pen.dashBeginCapStyle = dashCap
        pen.dashEndCapStyle   = dashCap

        painter.drawLine(x0 - paintPos.x, y0 - paintPos.y, x1 - paintPos.x, y1 - paintPos.y, pen)
    }

    mtd impl setup(capt: *Capture)
    {
        capture = capt
        gizmo.anchors.resize(2)

        gizmo.sigChanged = closure|self|(diff, pt, first) {
            GizmoPoints.apply(self, diff, pt, first)
        }
    }
}
