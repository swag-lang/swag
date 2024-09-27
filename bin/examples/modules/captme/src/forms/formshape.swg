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
        return borderColor == other.borderColor and
               borderSize == other.borderSize and
               backColor == other.backColor and
               alpha == other.alpha and
               kind == other.kind and
               dashStyle == other.dashStyle and
               paintShadow == other.paintShadow
    }

    mtd impl copy(dst, src: *BaseForm)
    {
        let dstf = cast(*FormShape) dst
        let srcf = cast(*FormShape) src
        dstf.borderColor = srcf.borderColor
        dstf.borderSize  = srcf.borderSize
        dstf.backColor   = srcf.backColor
        dstf.alpha       = srcf.alpha
        dstf.kind        = srcf.kind
        dstf.dashStyle   = srcf.dashStyle
        dstf.paintShadow = srcf.paintShadow
    }

    mtd impl edit(evt: *MouseEvent, paintRect: Math.Rectangle, movingStart: Math.Point, localPos: Math.Point, zoom: f32)->bool
    {
        let xa = cast(s32) ((movingStart.x - paintRect.x) / zoom)
        let ya = cast(s32) ((movingStart.y - paintRect.y) / zoom)
        var xb = cast(s32) ((localPos.x - paintRect.x) / zoom)
        var yb = cast(s32) ((localPos.y - paintRect.y) / zoom)
        setBaseForm(xa, ya, xb - xa, yb - ya)

        var xbu = 0
        var ybu = 0
        capture.snap(&xbu, &ybu, true, true, 0, cast(IForm) self)
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

        setBaseForm(xa, ya, xb - xa, yb - ya)

        if evt.kind == .MouseReleased and evt.button == .Left:
            return false
        return true
    }

    mtd impl moveBaseForm(newPosX, newPosY, newSizeX, newSizeY: s32)
    {
        x += newPosX
        y += newPosY
        w += newSizeX
        h += newSizeY
    }

    mtd impl setBaseForm(newPosX, newPosY, newSizeX, newSizeY: s32)
    {
        if newSizeX < 0
        {
            x = newPosX + newSizeX
            w = -newSizeX
        }
        else
        {
            x = newPosX
            w = newSizeX
        }

        if newSizeY < 0
        {
            y = newPosY + newSizeY
            h = -newSizeY
        }
        else
        {
            y = newPosY
            h = newSizeY
        }
    }

    mtd impl moveForm(newPosX, newPosY: s32)
    {
        x += newPosX
        y += newPosY
    }

    mtd impl movePoint(pt: u32, newPosX, newPosY: s32) {}

    mtd impl boundRect()->Math.Rectangle
    {
        return Math.Rectangle{x, y, w, h}
    }

    mtd impl mouse(view: *EditView, pos: Math.Point, evt: *MouseEvent)->bool
    {
        return gizmo.mouse(view, pos, evt)
    }

    mtd impl paintGizmo(painter: *Painter, z: f32, flags: FormFlags)
    {
        if flags & (.Hot | .Selected)
        {
            gizmo.rect = paintPos
            let col = subKind == .PixSel ? Argb.Yellow : Argb.White
            gizmo.paint(painter, flags & .Selected ? false : true, col)
        }
    }

    mtd impl isInside(pt: Math.Point)->bool
    {
        switch kind
        {
        case Ellipse:
            var br  = boundRect()
            var pos = Math.Vector2{pt.x, pt.y}
            var cen = Math.Vector2{br.horzCenter(), br.vertCenter()}
            return pos.isInEllipse(cen, br.width * 0.5, br.height * 0.5)
        default:
            return true
        }
    }

    mtd impl hitTest(pt: Math.Point, z: f32)->bool
    {
        switch kind
        {
        case Ellipse:
            var pos = Math.Vector2{pt.x, pt.y}
            var cen = Math.Vector2{paintPos.horzCenter(), paintPos.vertCenter()}
            if !pos.isInEllipse(cen, paintPos.width * 0.5, paintPos.height * 0.5):
                return false
            if backColor.a == 0 and pos.isInEllipse(cen, (paintPos.width * 0.5) - (2 * borderSize), (paintPos.height * 0.5) - (2 * borderSize)):
                return false

        case Rectangle, RoundRectangle:
            var rect = paintPos
            rect.inflate(-2 * borderSize)
            if backColor.a == 0 and rect.contains(pt):
                return false
        }

        return true
    }

    mtd impl paint(app: *Application, painter: *Painter, z: f32, quickMode: bool)
    {
        var bSize = adaptSize(borderSize, MinBorderSize, MaxBorderSize, z, quickMode)
        let rSize = adaptSize(roundSize, MinRoundSize, MaxRoundSize, z, quickMode)

        if subKind == .PixSel
        {
            bSize       = 2
            dashStyle   = .Dash
            borderColor = Argb.White
            painter.setAntialiased(false)
        }

        var pp  = Math.Rectangle{0, 0, paintPos.width, paintPos.height}
        var pen = Pen.createDash(dashStyle, borderColor, bSize)
        pen.borderPos = .Inside
        var backBrush = Brush.createHatch(.CheckBoardLarge, Argb.DarkGray)

        var bk = backColor
        if quickMode and alpha != 100 and subKind == .Highlight:
            bk.a = cast(u8) Math.map(cast(f32) alpha, 0, 100, 0, 255)

        if subKind == .PixSel
        {
            #[Swag.Complete]
            switch kind
            {
            case Ellipse:
                let sizeX = pp.width * 0.5
                let sizeY = pp.height * 0.5
                painter.drawEllipse(pp.x + sizeX, pp.y + sizeY, sizeX, sizeY, pen)

            case Rectangle:
                painter.drawRect(pp, pen)

            case RoundRectangle:
                painter.drawRoundRect(pp, rSize, rSize, pen)
            }
        }
        else
        {
            #[Swag.Complete]
            switch kind
            {
            case Ellipse:
                let sizeX = pp.width * 0.5
                let sizeY = pp.height * 0.5
                if quickMode and alpha != 100 and subKind == .Highlight:
                    painter.fillEllipse(pp.x + sizeX, pp.y + sizeY, sizeX, sizeY, backBrush)
                painter.fillEllipse(pp.x + sizeX, pp.y + sizeY, sizeX, sizeY, bk)
                painter.drawEllipse(pp.x + sizeX, pp.y + sizeY, sizeX, sizeY, pen)

            case Rectangle:
                if borderColor.a and (pp.width <= 2 * bSize or pp.height < 2 * bSize):
                    painter.fillRect(pp, borderColor)
                else
                {
                    if quickMode and alpha != 100 and subKind == .Highlight:
                        painter.fillRect(pp, backBrush)
                    painter.fillRect(pp, bk)
                    painter.drawRect(pp, pen)
                }

            case RoundRectangle:
                var pp1 = pp
                pp1.inflate(-bSize * 0.5)
                if quickMode and alpha != 100 and subKind == .Highlight:
                    painter.fillRoundRect(pp1, rSize, rSize, backBrush)
                painter.fillRoundRect(pp1, rSize, rSize, bk)
                painter.drawRoundRect(pp, rSize, rSize, pen)
            }
        }
    }

    mtd impl setup(capt: *Capture)
    {
        capture = capt

        with gizmo
        {
            .roundShape        = true
            .anchorSize        = 10
            .trackChange       = true
            .paintSimpleBorder = false
            .paintBigBorder    = true
        }

        gizmo.sigChanged = func|self|(g, first, hitMove)
        {
            g.apply(self, first, hitMove)
        }
    }
}
