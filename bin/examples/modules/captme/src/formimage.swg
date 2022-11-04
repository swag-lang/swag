enum FormImageKind
{
    Image
    Blur
    Pixelate
}

struct FormImage
{
    using base: BaseForm
    image:      Image
    kind:       FormImageKind = .Image
    strength:   s32 = 50

    #[Serialization.NoSerialize]
    {
        stc:            s32
        xc, yc:         s32
        wc, hc:         s32
        texture:        Texture
        gizmo:          GizmoRect
        textureKind:    FormImageKind = .Image
    }
}

impl IForm for FormImage
{
    mtd isSame(ptr: *BaseForm)->bool
    {
        other := cast(*FormImage) ptr
        return  borderColor == other.borderColor and
                borderSize  == other.borderSize and
                alpha       == other.alpha and
                kind        == other.kind and
                paintShadow == other.paintShadow and
                strength    == other.strength
    }

    mtd copy(dst, src: *BaseForm)
    {
        dstf := cast(*FormImage) dst
        srcf := cast(*FormImage) src
        dstf.borderColor = srcf.borderColor
        dstf.borderSize  = srcf.borderSize
        dstf.alpha       = srcf.alpha
        dstf.kind        = srcf.kind
        dstf.image       = srcf.image
        dstf.paintShadow = srcf.paintShadow
        dstf.strength    = srcf.strength
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
        return true
    }

    mtd paint(app: *Application, painter: *Painter, z: f32, quickMode: bool)
    {
        bSize := adaptSize(borderSize, MinBorderSize, MaxBorderSize, z, quickMode)
        pp    := Math.Rectangle{0, 0, paintPos.width, paintPos.height}
        pen   := Pen.createSolid(borderColor, bSize)

        renderer := app.getRenderer()
        switch kind
        {
        case Image:
            if !texture.isValid() or textureKind != kind
            {
                textureKind = kind
                renderer.removeTexture(&texture)
                texture = renderer.addImage(image)
            }

        case Blur, Pixelate:
            if !texture.isValid() or
               xc != x or yc != y or wc != w or hc != h or
               textureKind != kind or
               stc != strength
            {
                textureKind = kind
                stc = strength
                xc = x; yc = y; wc = w; hc = h
                renderer.removeTexture(&texture)
                img := capture.backImg.toCrop(x, y, w, h)
                st := Math.map(cast(f32) strength, 0, 100, 1, 16)
                nw := Math.max(cast(s32) (img.width / st), 4)
                nh := Math.max(cast(s32) (img.height / st), 4)
                img.resize(nw, nh, kind == .Pixelate ? .Raw : .Quadratic)
                img.resize(w, h, kind == .Pixelate ? .Raw : .Quadratic)
                texture = app.getRenderer().addImage(img)
            }
        }

        if borderColor.a and (pp.width <= 2*bSize or pp.height < 2*bSize)
            painter.fillRect(pp, borderColor)
        else
        {
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