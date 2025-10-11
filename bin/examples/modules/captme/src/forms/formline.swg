struct FormLine
{
    using base:         BaseForm
    beginCap:           LineCapStyle = .Round
    endCap:             LineCapStyle = .Round
    dashCap:            DashCapStyle = .None
    startSize:          f32 = 4
    endSize:            f32 = 4
    xa, ya, xb, yb:     f32
    xc, yc:             f32 = 10
    cacheCoords:        Math.Rectangle
    cacheCoords2:       Math.Point
    cacheBv:            Math.Rectangle

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
        return .borderColor == other.borderColor and
               .borderSize == other.borderSize and
               .alpha == other.alpha and
               .beginCap == other.beginCap and
               .endCap == other.endCap and
               .dashStyle == other.dashStyle and
               .startSize == other.startSize and
               .endSize == other.endSize and
               .dashCap == other.dashCap and
               .bezier == other.bezier and
               .paintShadow == other.paintShadow
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
        dstf.bezier      = srcf.bezier
        dstf.paintShadow = srcf.paintShadow
    }

    mtd impl edit(evt: *MouseEvent, movingStart: Math.Point, localPos: Math.Point, zoom: f32)->bool
    {
        .xa = cast(s32) (movingStart.x / zoom)
        .ya = cast(s32) (movingStart.y / zoom)
        .xb = cast(s32) (localPos.x / zoom)
        .yb = cast(s32) (localPos.y / zoom)

        var xbu = 0
        var ybu = 0
        .capture.snap(&xbu, &ybu, true, true, 1, cast(IForm) me)
        .xb += xbu
        .yb += ybu

        if evt.kind == .MouseReleased and evt.button == .Left do
            return false
        return true
    }

    mtd impl moveBaseForm(newPosX, newPosY, newSizeX, newSizeY: s32)
    {
        .xa, .xb += newPosX
        .ya, .yb += newPosY
    }

    mtd impl scaleBaseForm(scale: f32)
    {
        .xa, .xb *= scale
        .ya, .yb *= scale
    }

    mtd impl setBaseForm(newPosX, newPosY, newSizeX, newSizeY: s32)
    {
        .xa = newPosX
        .ya = newPosY
        .xb = .xa + newSizeX
        .yb = .ya + newSizeY
    }

    mtd impl moveForm(newPosX, newPosY: s32)
    {
        .xa, .xb += newPosX
        .ya, .yb += newPosY
    }

    mtd impl movePoint(pt: u32, newPosX, newPosY: s32)
    {
        switch pt
        {
        case 0:
            .xa += newPosX
            .ya += newPosY
        case 1:
            .xb += newPosX
            .yb += newPosY
        case 2:
            .xc += newPosX
            .yc += newPosY
        }
    }

    mtd impl boundRect()->Math.Rectangle
    {
        if .bezier
        {
            let newCoords  = Math.Rectangle{.xa, .ya, .xb, .yb}
            let newCoords2 = Math.Point{.xc, .yc}
            if .cacheCoords == newCoords and .cacheCoords2 == newCoords2 do
                return .cacheBv
            .cacheCoords  = newCoords
            .cacheCoords2 = newCoords2
            .cacheBv      = Math.Curve.getBezierBoundingBox({.xa, .ya}, {.xb, .yb}, {.xa + .xc, .ya + .yc})
        }
        else
        {
            let x0 = Math.min(.xa, .xb)
            let y0 = Math.min(.ya, .yb)
            let x1 = Math.max(.xa, .xb)
            let y1 = Math.max(.ya, .yb)
            .cacheBv = Math.Rectangle{x0, y0, x1 - x0, y1 - y0}
        }

        return .cacheBv
    }

    mtd impl mouse(view: *EditView, pos: Math.Point, evt: *MouseEvent)->bool
    {
        return .gizmo.mouse(view, pos, evt)
    }

    mtd impl paintGizmo(painter: *Painter, z: f32, flags: FormFlags)
    {
        if flags.has(.Hot | .Selected)
        {
            .gizmo.anchors[0] = {.xa * z, .ya * z}
            .gizmo.anchors[1] = {.xb * z, .yb * z}
            if .gizmo.anchors.count == 3
            {
                .gizmo.anchors[2] = {(.xa + .xc) * z, (.ya + .yc) * z}
                painter.drawLine(.gizmo.anchors[0].x, .gizmo.anchors[0].y, .gizmo.anchors[2].x, .gizmo.anchors[2].y, Argb.White, 2)
                painter.drawLine(.gizmo.anchors[1].x, .gizmo.anchors[1].y, .gizmo.anchors[2].x, .gizmo.anchors[2].y, Argb.White, 2)
            }

            .gizmo.paint(null, painter, !flags.has(.Selected))
        }
    }

    mtd impl isInside(pt: Math.Point)->bool
    {
        return false
    }

    mtd impl hitTest(pt: Math.Point, z: f32)->bool
    {
        return BaseForm.drawHitTest(me, pt, z)
    }

    mtd impl paint(painter: *Painter, cxt: FormPaintContext)
    {
        var bSize = cxt.hitTest ? Math.max(.borderSize, 5) : .borderSize
        bSize = BaseForm.adaptSize(.borderSize, MinBorderSize, MaxBorderSize, cxt, 8, 12)
        let sSize = cxt.quickMode ? BaseForm.adaptSize(.startSize, MinCapSize, MaxCapSize, cxt, 1, 4) : .startSize
        let eSize = cxt.quickMode ? BaseForm.adaptSize(.endSize, MinCapSize, MaxCapSize, cxt, 1, 4) : .endSize

        if .bezier
        {
            // Add an anchor the first time, when transforming from 'linear' to 'bezier'
            if .gizmo.anchors.count == 2
            {
                var v = Math.Vector2{.xb, .yb} - Math.Vector2{.xa, .ya}
                v *= 0.5
                var vn = v.cross(1).toNormalizedSafe()
                vn *= v.length() * 0.25
                .xc = v.x + vn.x
                .yc = v.y + vn.y
                .gizmo.anchors.resize(3)
            }

            if cxt.quickMode
            {
                .xc = .xa
                .yc = Math.abs(.ya - .yb) * 0.75
            }
        }
        elif .gizmo.anchors.count == 3
        {
            .gizmo.anchors.resize(2)
        }

        with var pen = Pen.createDash(me.dashStyle, me.borderColor, bSize)
        {
            .dashBlankScale  = 0.25
            .begCapStyle     = me.beginCap
            .endCapStyle     = me.endCap
            .capBegScaleX    = sSize
            .capEndScaleX    = eSize
            .capBegScaleY    = 2
            .capEndScaleY    = 2
            .dashBegCapStyle = me.dashCap
            .dashEndCapStyle = me.dashCap
        }

        let z = cxt.z
        if .bezier do
            painter.drawBezier(.xa * z, .ya * z, .xb * z, .yb * z, (.xa + .xc) * z, (.ya + .yc) * z, pen)
        else do
            painter.drawLine(.xa * z, .ya * z, .xb * z, .yb * z, pen)
    }

    mtd impl setup(capt: #null *Capture)
    {
        .capture = capt
        .gizmo.anchors.resize(3)

        .gizmo.sigChanged = mtd||(diff, pt, first)
        {
            GizmoPoints.apply(me, diff, pt, first)
        }
    }
}
