struct FormLine
{
    using base:         BaseForm
    beginCap:           LineCapStyle = .Round
    endCap:             LineCapStyle = .Round
    dashCap:            DashCapStyle = .None
    startSize:          f32 = 4
    endSize:            f32 = 4
    xa, ya, xb, yb:     f32

    #[Serialization.NoSerialize]
    {
        gizmo: GizmoPoints
    }
}

impl IForm for FormLine
{
    mtd impl isSame(ptr: *BaseForm)->bool
    {
        let other = cast(*FormLine) ptr
        return borderColor == other.borderColor and
               borderSize == other.borderSize and
               alpha == other.alpha and
               beginCap == other.beginCap and
               endCap == other.endCap and
               dashStyle == other.dashStyle and
               startSize == other.startSize and
               endSize == other.endSize and
               dashCap == other.dashCap and
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

    mtd impl edit(evt: *MouseEvent, movingStart: Math.Point, localPos: Math.Point, zoom: f32)->bool
    {
        xa = cast(s32) (movingStart.x / zoom)
        ya = cast(s32) (movingStart.y / zoom)
        xb = cast(s32) (localPos.x / zoom)
        yb = cast(s32) (localPos.y / zoom)

        var xbu = 0
        var ybu = 0
        capture.snap(&xbu, &ybu, true, true, 1, cast(IForm) self)
        xb += xbu
        yb += ybu

        if evt.kind == .MouseReleased and evt.button == .Left:
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
        let x0 = Math.min(xa, xb)
        let y0 = Math.min(ya, yb)
        let x1 = Math.max(xa, xb)
        let y1 = Math.max(ya, yb)
        return Math.Rectangle{x0, y0, x1 - x0, y1 - y0}
    }

    mtd impl mouse(view: *EditView, pos: Math.Point, evt: *MouseEvent)->bool
    {
        return gizmo.mouse(view, pos, evt)
    }

    mtd impl paintGizmo(painter: *Painter, z: f32, flags: FormFlags)
    {
        if flags & (.Hot | .Selected)
        {
            gizmo.anchors[0] = {xa * z, ya * z}
            gizmo.anchors[1] = {xb * z, yb * z}
            gizmo.paint(null, painter, flags & .Selected ? false : true)
        }
    }

    mtd impl isInside(pt: Math.Point)->bool
    {
        return false
    }

    mtd impl hitTest(pt: Math.Point, z: f32)->bool
    {
        return BaseForm.drawHitTest(self, pt, z)
    }

    mtd impl paint(app: nl *Application, painter: *Painter, z: f32, quickMode: bool)
    {
        let bSize = adaptSize(borderSize, MinBorderSize, MaxBorderSize, z, quickMode, 8, 12)
        let sSize = quickMode ? adaptSize(startSize, MinCapSize, MaxCapSize, z, quickMode, 1, 4) : startSize
        let eSize = quickMode ? adaptSize(endSize, MinCapSize, MaxCapSize, z, quickMode, 1, 4) : endSize

        with var pen = Pen.createDash(dashStyle, borderColor, bSize)
        {
            .dashBlankScale    = 0.25
            .beginCapStyle     = beginCap
            .endCapStyle       = endCap
            .capStartScaleX    = sSize
            .capEndScaleX      = eSize
            .capStartScaleY    = 2
            .capEndScaleY      = 2
            .dashBeginCapStyle = dashCap
            .dashEndCapStyle   = dashCap
        }

        painter.drawLine(xa * z, ya * z, xb * z, yb * z, pen)
    }

    mtd impl setup(capt: nl *Capture)
    {
        capture = capt
        gizmo.anchors.resize(3)

        gizmo.sigChanged = func|self|(diff, pt, first)
        {
            GizmoPoints.apply(self, diff, pt, first)
        }
    }
}
