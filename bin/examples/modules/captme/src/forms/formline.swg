#global marked
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
        return me.borderColor == other.borderColor and
               me.borderSize == other.borderSize and
               me.alpha == other.alpha and
               me.beginCap == other.beginCap and
               me.endCap == other.endCap and
               me.dashStyle == other.dashStyle and
               me.startSize == other.startSize and
               me.endSize == other.endSize and
               me.dashCap == other.dashCap and
               me.bezier == other.bezier and
               me.paintShadow == other.paintShadow
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
        me.xa = cast(s32) (movingStart.x / zoom)
        me.ya = cast(s32) (movingStart.y / zoom)
        me.xb = cast(s32) (localPos.x / zoom)
        me.yb = cast(s32) (localPos.y / zoom)

        var xbu = 0
        var ybu = 0
        me.capture.snap(&xbu, &ybu, true, true, 1, cast(IForm) me)
        me.xb += xbu
        me.yb += ybu

        if evt.kind == .MouseReleased and evt.button == .Left:
            return false
        return true
    }

    mtd impl moveBaseForm(newPosX, newPosY, newSizeX, newSizeY: s32)
    {
        me.xa, me.xb += newPosX
        me.ya, me.yb += newPosY
    }

    mtd impl scaleBaseForm(scale: f32)
    {
        me.xa, me.xb *= #me.scale
        me.ya, me.yb *= #me.scale
    }

    mtd impl setBaseForm(newPosX, newPosY, newSizeX, newSizeY: s32)
    {
        me.xa = newPosX
        me.ya = newPosY
        me.xb = me.xa + newSizeX
        me.yb = me.ya + newSizeY
    }

    mtd impl moveForm(newPosX, newPosY: s32)
    {
        me.xa, me.xb += newPosX
        me.ya, me.yb += newPosY
    }

    mtd impl movePoint(pt: u32, newPosX, newPosY: s32)
    {
        switch pt
        {
        case 0:
            me.xa += newPosX
            me.ya += newPosY
        case 1:
            me.xb += newPosX
            me.yb += newPosY
        case 2:
            me.xc += newPosX
            me.yc += newPosY
        }
    }

    mtd impl boundRect()->Math.Rectangle
    {
        if me.bezier
        {
            let newCoords  = Math.Rectangle{me.xa, me.ya, me.xb, me.yb}
            let newCoords2 = Math.Point{me.xc, me.yc}
            if me.cacheCoords == newCoords and me.cacheCoords2 == newCoords2:
                return me.cacheBv
            me.cacheCoords  = newCoords
            me.cacheCoords2 = newCoords2
            me.cacheBv      = Math.Curve.getBezierBoundingBox({me.xa, me.ya}, {me.xb, me.yb}, {me.xa + me.xc, me.ya + me.yc})
        }
        else
        {
            let x0 = Math.min(me.xa, me.xb)
            let y0 = Math.min(me.ya, me.yb)
            let x1 = Math.max(me.xa, me.xb)
            let y1 = Math.max(me.ya, me.yb)
            me.cacheBv = Math.Rectangle{x0, y0, x1 - x0, y1 - y0}
        }

        return me.cacheBv
    }

    mtd impl mouse(view: *EditView, pos: Math.Point, evt: *MouseEvent)->bool
    {
        return me.gizmo.mouse(view, pos, evt)
    }

    mtd impl paintGizmo(painter: *Painter, z: f32, flags: FormFlags)
    {
        if flags.has(.Hot | .Selected)
        {
            me.gizmo.anchors[0] = {me.xa * z, me.ya * z}
            me.gizmo.anchors[1] = {me.xb * z, me.yb * z}
            if me.gizmo.anchors.count == 3
            {
                me.gizmo.anchors[2] = {(me.xa + me.xc) * z, (me.ya + me.yc) * z}
                painter.drawLine(me.gizmo.anchors[0].x, me.gizmo.anchors[0].y, me.gizmo.anchors[2].x, me.gizmo.anchors[2].y, Argb.White, 2)
                painter.drawLine(me.gizmo.anchors[1].x, me.gizmo.anchors[1].y, me.gizmo.anchors[2].x, me.gizmo.anchors[2].y, Argb.White, 2)
            }

            me.gizmo.paint(null, painter, !flags.has(.Selected))
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
        var bSize = cxt.hitTest ? Math.max(me.borderSize, 5) : me.borderSize
        bSize = BaseForm.adaptSize(me.borderSize, MinBorderSize, MaxBorderSize, cxt, 8, 12)
        let sSize = cxt.quickMode ? BaseForm.adaptSize(me.startSize, MinCapSize, MaxCapSize, cxt, 1, 4) : me.startSize
        let eSize = cxt.quickMode ? BaseForm.adaptSize(me.endSize, MinCapSize, MaxCapSize, cxt, 1, 4) : me.endSize

        if me.bezier
        {
            // Add an anchor the first time, when transforming from 'linear' to 'bezier'
            if me.gizmo.anchors.count == 2
            {
                var v = Math.Vector2{me.xb, me.yb} - Math.Vector2{me.xa, me.ya}
                v *= 0.5
                var vn = v.cross(1).toNormalizedSafe()
                vn *= v.length() * 0.25
                me.xc = v.x + vn.x
                me.yc = v.y + vn.y
                me.gizmo.anchors.resize(3)
            }

            if cxt.quickMode
            {
                me.xc = me.xa
                me.yc = Math.abs(me.ya - me.yb) * 0.75
            }
        }
        elif me.gizmo.anchors.count == 3
        {
            me.gizmo.anchors.resize(2)
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
        if me.bezier:
            painter.drawBezier(me.xa * z, me.ya * z, me.xb * z, me.yb * z, (me.xa + me.xc) * z, (me.ya + me.yc) * z, pen)
        else:
            painter.drawLine(me.xa * z, me.ya * z, me.xb * z, me.yb * z, pen)
    }

    mtd impl setup(capt: #null *Capture)
    {
        me.capture = capt
        me.gizmo.anchors.resize(3)

        me.gizmo.sigChanged = mtd||(diff, pt, first)
        {
            GizmoPoints.apply(me, diff, pt, first)
        }
    }
}
