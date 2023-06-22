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
    using base: BaseForm
    kind:       FormShapeKind = Ellipse
    roundSize:  f32 = 10

    #[Serialization.NoSerialize]
    {
        gizmo: GizmoRect
    }
}

impl IForm for FormShape
{
    mtd isSame(ptr: *BaseForm)->bool
    {
        other := cast(*FormShape) ptr
        return  borderColor == other.borderColor and
                borderSize  == other.borderSize and
                backColor   == other.backColor and
                alpha       == other.alpha and
                kind        == other.kind and
                dashStyle   == other.dashStyle and
                paintShadow == other.paintShadow
    }

    mtd copy(dst, src: *BaseForm)
    {
        dstf := cast(*FormShape) dst
        srcf := cast(*FormShape) src
        dstf.borderColor = srcf.borderColor
        dstf.borderSize  = srcf.borderSize
        dstf.backColor   = srcf.backColor
        dstf.alpha       = srcf.alpha
        dstf.kind        = srcf.kind
        dstf.dashStyle   = srcf.dashStyle
        dstf.paintShadow = srcf.paintShadow
    }

    mtd edit(evt: *MouseEvent, paintRect: Math.Rectangle, movingStart: Math.Point, localPos: Math.Point, zoom: f32)->bool
    {
        xa := cast(s32) ((movingStart.x - paintRect.x) / zoom)
        ya := cast(s32) ((movingStart.y - paintRect.y) / zoom)
        xb := cast(s32) ((localPos.x - paintRect.x) / zoom)
        yb := cast(s32) ((localPos.y - paintRect.y) / zoom)
        setBaseForm(xa, ya, xb - xa, yb - ya)

        xbu := 0
        ybu := 0
        capture.snap(&xbu, &ybu, true, true, 0, cast(IForm) self)
        xb += xbu
        yb += ybu

        // Same size
        shiftPressed := Input.Keyboard.isKeyPressed(.Shift) or Input.Keyboard.isKeyPressed(.RShift)
        if shiftPressed
        {
            ww := xb - xa
            hh := yb - ya
            if ww > hh
                yb = ya + ww
            else
                xb = xa + hh
        }

        setBaseForm(xa, ya, xb - xa, yb - ya)

        if evt.kind == .MouseReleased and evt.button == .Left
            return false
        return true
    }

    mtd moveBaseForm(newPosX, newPosY, newSizeX, newSizeY: s32)
    {
        x += newPosX
        y += newPosY
        w += newSizeX
        h += newSizeY
    }

    mtd setBaseForm(newPosX, newPosY, newSizeX, newSizeY: s32)
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

    mtd moveForm(newPosX, newPosY: s32)
    {
        x += newPosX
        y += newPosY
    }

    mtd movePoint(pt: u32, newPosX, newPosY: s32)
    {
    }

    mtd boundRect()->Math.Rectangle
    {
        return Math.Rectangle{x, y, w, h}
    }

    mtd mouse(view: *EditView, pos: Math.Point, evt: *MouseEvent)->bool
    {
        return gizmo.mouse(view, pos, evt)
    }

    mtd paintGizmo(painter: *Painter, z: f32, flags: FormFlags)
    {
        if flags & (.Hot | .Selected)
        {
            gizmo.rect = paintPos
            col := subKind == .PixSel ? Argb.Yellow : Argb.White
            gizmo.paint(painter, flags & .Selected ? false : true, col)
        }
    }

    mtd isInside(pt: Math.Point)->bool
    {
        switch kind
        {
        case Ellipse:
            br := boundRect()
            pos := Math.Vector2{pt.x, pt.y}
            cen := Math.Vector2{br.horzCenter(), br.vertCenter()}
            return pos.isInEllipse(cen, br.width * 0.5, br.height * 0.5)
        default:
            return true
        }
    }

    mtd hitTest(pt: Math.Point, z: f32)->bool
    {
        switch kind
        {
        case Ellipse:
            pos := Math.Vector2{pt.x, pt.y}
            cen := Math.Vector2{paintPos.horzCenter(), paintPos.vertCenter()}
            if !pos.isInEllipse(cen, paintPos.width * 0.5, paintPos.height * 0.5)
                return false
            if backColor.a == 0 and pos.isInEllipse(cen, (paintPos.width * 0.5) - (2*borderSize), (paintPos.height * 0.5) - (2*borderSize))
                return false

        case Rectangle, RoundRectangle:
            rect := paintPos
            rect.inflate(-2*borderSize)
            if backColor.a == 0 and rect.contains(pt)
                return false
        }

        return true
    }

    mtd paint(app: *Application, painter: *Painter, z: f32, quickMode: bool)
    {
        bSize := adaptSize(borderSize, MinBorderSize, MaxBorderSize, z, quickMode)
        rSize := adaptSize(roundSize, MinRoundSize, MaxRoundSize, z, quickMode)

        if subKind == .PixSel
        {
            bSize = 2
            dashStyle = .Dash
            borderColor = Argb.White
            painter.setAntialiased(false)
        }

        pp  := Math.Rectangle{0, 0, paintPos.width, paintPos.height}
        pen := Pen.createDash(dashStyle, borderColor, bSize)
        pen.borderPos = .Inside
        backBrush := Brush.createHatch(.CheckBoardLarge, Argb.DarkGray)

        bk := backColor
        if quickMode and alpha != 100 and subKind == .Highlight
            bk.a = cast(u8) Math.map(cast(f32) alpha, 0, 100, 0, 255)

        if subKind == .PixSel
        {
            #[Swag.Complete]
            switch kind
            {
            case Ellipse:
                sizeX := pp.width * 0.5
                sizeY := pp.height * 0.5
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
                sizeX := pp.width * 0.5
                sizeY := pp.height * 0.5
                if quickMode and alpha != 100 and subKind == .Highlight
                    painter.fillEllipse(pp.x + sizeX, pp.y + sizeY, sizeX, sizeY, backBrush)
                painter.fillEllipse(pp.x + sizeX, pp.y + sizeY, sizeX, sizeY, bk)
                painter.drawEllipse(pp.x + sizeX, pp.y + sizeY, sizeX, sizeY, pen)

            case Rectangle:
                if borderColor.a and (pp.width <= 2*bSize or pp.height < 2*bSize)
                    painter.fillRect(pp, borderColor)
                else
                {
                    if quickMode and alpha != 100 and subKind == .Highlight
                        painter.fillRect(pp, backBrush)
                    painter.fillRect(pp, bk)
                    painter.drawRect(pp, pen)
                }

            case RoundRectangle:
                pp1 := pp
                pp1.inflate(-bSize*0.5)
                if quickMode and alpha != 100 and subKind == .Highlight
                    painter.fillRoundRect(pp1, rSize, rSize, backBrush)
                painter.fillRoundRect(pp1, rSize, rSize, bk)
                painter.drawRoundRect(pp, rSize, rSize, pen)
            }
        }
    }

    mtd setup(capt: *Capture)
    {
        capture = capt

        with gizmo
        {
            .roundShape = true
            .anchorSize = 10
            .trackChange = true
            .paintSimpleBorder = false
            .paintBigBorder = true
        }

        gizmo.sigChanged = closure|self|(g, first, hitMove) {
            g.apply(self, first, hitMove)
        }
    }
}