#global marked
enum FormShapeKind
{
    Ellipse
    Rectangle
    RoundRectangle
}

const MinRoundSize = 10.0
const MaxRoundSize = 50.0

struct FormShape
{
    using base:     BaseForm
    kind:           FormShapeKind = Ellipse
    roundSize:      f32 = 10

    #[Serialization.NoSerialize]
    {
        gizmo: GizmoRect
    }
}

impl IForm for FormShape
{
    mtd impl isSame(ptr: *BaseForm)->bool
    {
        let other = cast(*FormShape) ptr
        return me.borderColor == other.borderColor and
               me.borderSize == other.borderSize and
               me.backColor == other.backColor and
               me.alpha == other.alpha and
               me.kind == other.kind and
               me.subKind == other.subKind and
               me.dashStyle == other.dashStyle and
               me.roundSize == other.roundSize and
               me.paintShadow == other.paintShadow
    }

    mtd impl copy(dst, src: *BaseForm)
    {
        let dstf = cast(*FormShape) dst
        let srcf = cast(*FormShape) src
        dstf.borderColor = srcf.borderColor
        dstf.borderSize  = srcf.borderSize
        dstf.roundSize   = srcf.roundSize
        dstf.backColor   = srcf.backColor
        dstf.alpha       = srcf.alpha
        dstf.kind        = srcf.kind
        dstf.subKind     = srcf.subKind
        dstf.dashStyle   = srcf.dashStyle
        dstf.paintShadow = srcf.paintShadow
    }

    mtd impl edit(evt: *MouseEvent, movingStart: Math.Point, localPos: Math.Point, zoom: f32)->bool
    {
        let xa = cast(s32) (movingStart.x / zoom)
        let ya = cast(s32) (movingStart.y / zoom)
        var xb = cast(s32) (localPos.x / zoom)
        var yb = cast(s32) (localPos.y / zoom)
        me.setBaseForm(xa, ya, xb - xa, yb - ya)

        var xbu = 0
        var ybu = 0
        me.capture.snap(&xbu, &ybu, true, true, 0, cast(IForm) me)
        xb += xbu
        yb += ybu

        // Same size
        let shiftPressed = Input.Keyboard.isKeyPressed(.Shift) or Input.Keyboard.isKeyPressed(.RShift)
        if shiftPressed
        {
            let ww = xb - xa
            let hh = yb - ya
            if ww > hh:
                yb = ya + ww
            else:
                xb = xa + hh
        }

        me.setBaseForm(xa, ya, xb - xa, yb - ya)

        if evt.kind == .MouseReleased and evt.button == .Left:
            return false
        return true
    }

    mtd impl moveBaseForm(newPosX, newPosY, newSizeX, newSizeY: s32)
    {
        me.x += newPosX
        me.y += newPosY
        me.w += newSizeX
        me.h += newSizeY
    }

    mtd impl scaleBaseForm(val: f32)
    {
        me.x = cast(s32) (me.x * val)
        me.y = cast(s32) (me.y * val)
        me.w = cast(s32) (me.w * val)
        me.h = cast(s32) (me.h * val)
    }

    mtd impl setBaseForm(newPosX, newPosY, newSizeX, newSizeY: s32)
    {
        if newSizeX < 0
        {
            me.x = newPosX + newSizeX
            me.w = -newSizeX
        }
        else
        {
            me.x = newPosX
            me.w = newSizeX
        }

        if newSizeY < 0
        {
            me.y = newPosY + newSizeY
            me.h = -newSizeY
        }
        else
        {
            me.y = newPosY
            me.h = newSizeY
        }
    }

    mtd impl moveForm(newPosX, newPosY: s32)
    {
        me.x += newPosX
        me.y += newPosY
    }

    mtd impl movePoint(pt: u32, newPosX, newPosY: s32) {}

    mtd impl boundRect()->Math.Rectangle
    {
        return Math.Rectangle{me.x, me.y, me.w, me.h}
    }

    mtd impl mouse(view: *EditView, pos: Math.Point, evt: *MouseEvent)->bool
    {
        return me.gizmo.mouse(view, pos, evt)
    }

    mtd impl paintGizmo(painter: *Painter, z: f32, flags: FormFlags)
    {
        if flags.has(.Hot | .Selected)
        {
            me.gizmo.rect = me.boundRect() * z
            let col = me.subKind == .PixSel ? Argb.Yellow : Argb.White
            me.gizmo.paint(painter, !flags.has(.Selected), col)
        }
    }

    mtd impl isInside(pt: Math.Point)->bool
    {
        switch me.kind
        {
        case Ellipse:
            var br  = me.boundRect()
            var pos = Math.Vector2{pt.x, pt.y}
            var cen = Math.Vector2{br.horzCenter(), br.vertCenter()}
            return Math.Geometry.isInEllipse(pos, cen, br.width * 0.5, br.height * 0.5)
        default:
            return true
        }
    }

    mtd impl hitTest(pt: Math.Point, z: f32)->bool
    {
        return BaseForm.drawHitTest(me, pt, z)
    }

    mtd impl paint(painter: *Painter, cxt: FormPaintContext)
    {
        var dash  = me.dashStyle
        var bSize = cxt.hitTest ? Math.max(me.borderSize, 5) : me.borderSize
        bSize = BaseForm.adaptSize(bSize, MinBorderSize, MaxBorderSize, cxt)
        let rSize = BaseForm.adaptSize(me.roundSize, MinRoundSize, MaxRoundSize, cxt)
        var bk    = me.backColor

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
            me.borderColor = Argb.White
            painter.setAntialiased(false)
        }

        let z   = cxt.z
        var pp  = me.boundRect() * z
        var pen = Pen.createDash(dash, me.borderColor, bSize)
        pen.borderPos = .Inside
        var backBrush = Brush.createHatch(.CheckBoardLarge, Argb.DarkGray)

        if cxt.quickMode and me.alpha != 100 and me.subKind == .Highlight:
            bk.a = cast(u8) Math.map(cast(f32) me.alpha, 0, 100, 0, 255)

        #[Swag.Complete]
        switch me.kind
        {
        case Ellipse:
            let sizeX = pp.width * 0.5
            let sizeY = pp.height * 0.5
            if cxt.quickMode and me.alpha != 100 and me.subKind == .Highlight:
                painter.fillEllipse(pp.x + sizeX, pp.y + sizeY, sizeX, sizeY, backBrush)
            painter.fillEllipse(pp.x + sizeX, pp.y + sizeY, sizeX, sizeY, bk)
            painter.drawEllipse(pp.x + sizeX, pp.y + sizeY, sizeX, sizeY, pen)

        case Rectangle:
            if me.borderColor.a and (pp.width <= 2 * bSize or pp.height < 2 * bSize):
                painter.fillRect(pp, me.borderColor)
            else
            {
                if cxt.quickMode and me.alpha != 100 and me.subKind == .Highlight:
                    painter.fillRect(pp, backBrush)
                painter.fillRect(pp, bk)
                painter.drawRect(pp, pen)
            }

        case RoundRectangle:
            var pp1 = pp
            pp1.inflate(-bSize * 0.5)
            if cxt.quickMode and me.alpha != 100 and me.subKind == .Highlight:
                painter.fillRoundRect(pp1, rSize, rSize, backBrush)
            painter.fillRoundRect(pp1, rSize, rSize, bk)
            painter.drawRoundRect(pp, rSize, rSize, pen)
        }
    }

    mtd impl setup(capt: *Capture)
    {
        me.capture = capt

        with me.gizmo
        {
            .roundShape        = true
            .trackChange       = true
            .paintSimpleBorder = false
            .paintBigBorder    = true
        }

        me.gizmo.sigChanged = mtd||(g, first, hitMove)
        {
            g.apply(me, first, hitMove)
        }
    }
}
