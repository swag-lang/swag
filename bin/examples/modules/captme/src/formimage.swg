enum FormImageKind
{
    Image
}

struct FormImage
{
    using base: BaseForm
    image: Image
    kind:  FormImageKind = Image

    #[Serialization.NoSerialize]
    {
        texture:    Texture
        gizmo:      GizmoRect
    }
}

impl IForm for FormImage
{
    mtd isSame(ptr: *BaseForm)->bool
    {
        other := cast(*FormImage) ptr
        return  borderColor == other.borderColor and
                borderSize  == other.borderSize and
                backColor   == other.backColor and
                alpha       == other.alpha and
                kind        == other.kind and
                paintShadow == other.paintShadow
    }

    mtd copy(dst, src: *BaseForm)
    {
        dstf := cast(*FormImage) dst
        srcf := cast(*FormImage) src
        dstf.borderColor = srcf.borderColor
        dstf.borderSize  = srcf.borderSize
        dstf.backColor   = srcf.backColor
        dstf.alpha       = srcf.alpha
        dstf.kind        = srcf.kind
        dstf.image       = srcf.image
        dstf.paintShadow = srcf.paintShadow
    }

    mtd edit(evt: *MouseEvent, paintRect: Math.Rectangle, movingStart: Math.Point, localPos: Math.Point, zoom: f32)->bool
    {
        xa := cast(s32) ((movingStart.x - paintRect.x) / zoom)
        ya := cast(s32) ((movingStart.y - paintRect.y) / zoom)
        xb := cast(s32) ((localPos.x - paintRect.x) / zoom)
        yb := cast(s32) ((localPos.y - paintRect.y) / zoom)
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

    mtd boundRect(raw: bool)->Math.Rectangle
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
            gizmo.paint(painter, flags & .Selected ? false : true)
        }
    }

    mtd hitTest(pt: Math.Point, z: f32)->bool
    {
        rect := paintPos
        rect.inflate(-2*borderSize)
        if backColor.a == 0 and rect.contains(pt)
            return false
        return true
    }

    mtd paint(app: *Application, painter: *Painter, z: f32, quickMode: bool)
    {
        bSize := adaptSize(borderSize, MinBorderSize, MaxBorderSize, z, quickMode)
        pp    := Math.Rectangle{0, 0, paintPos.width, paintPos.height}
        pen   := Pen.createSolid(borderColor, bSize)

        if !texture.isValid()
            texture = app.getRenderer().addImage(image)

        if borderColor.a and (pp.width <= 2*bSize or pp.height < 2*bSize)
            painter.fillRect(pp, borderColor)
        else
        {
            painter.fillRect(pp, backColor)

            painter.drawTexture(pp, texture)

            pp.inflate(-bSize * 0.5)
            painter.drawRect(pp, pen)
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

        gizmo.sigChanged = @|self|(g, first, hitMove) {
            g.apply(self, first, hitMove)
        }
    }
}