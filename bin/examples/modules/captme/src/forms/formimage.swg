enum FormImageKind
{
    Image
    Blur
    Pixelate
    Zoom
    Capture
}

enum FormImagePlacement
{
    Frame
    Center
    Free
}

struct FormImage
{
    using base:     BaseForm
    image:          Image
    kind:           FormImageKind = .Image
    strength:       s32 = 50
    place:          FormImagePlacement = .Center
    freePos:        Math.Rectangle

    #[Serialization.NoSerialize]
    {
        editInside:           bool
        dirty:                bool
        isReadyToCapture:     bool

        serial:               u32
        stc:                  s32
        xc, yc:               s32
        wc, hc:               s32
        texture:              Texture
        gizmo:                GizmoRect
        textureKind:          FormImageKind = .Image
    }
}

impl FormImage
{
    mtd opPostCopy()
    {
        @init(&.texture, 1)
    }
}

impl IForm for FormImage
{
    mtd impl isSame(ptr: *BaseForm)->bool
    {
        let other = cast(*FormImage) ptr
        if .kind == .Image and .backColor != other.backColor do
            return false
        return .borderColor == other.borderColor and
               .borderSize == other.borderSize and
               .alpha == other.alpha and
               .kind == other.kind and
               .paintShadow == other.paintShadow and
               .strength == other.strength
    }

    mtd impl copy(dst, src: *BaseForm)
    {
        let dstf = cast(*FormImage) dst
        let srcf = cast(*FormImage) src
        dstf.borderColor = srcf.borderColor
        dstf.borderSize  = srcf.borderSize
        dstf.backColor   = srcf.backColor
        dstf.alpha       = srcf.alpha
        dstf.kind        = srcf.kind
        dstf.paintShadow = srcf.paintShadow
        dstf.strength    = srcf.strength
        if srcf.image.isValid() do
            dstf.image = srcf.image
    }

    mtd impl edit(evt: *MouseEvent, movingStart: Math.Point, localPos: Math.Point, zoom: f32)->bool
    {
        let xa = cast(s32) (movingStart.x / zoom)
        let ya = cast(s32) (movingStart.y / zoom)
        let xb = cast(s32) (localPos.x / zoom)
        let yb = cast(s32) (localPos.y / zoom)
        .setBaseForm(xa, ya, xb - xa, yb - ya)
        if evt.kind == .MouseReleased and evt.button == .Left
        {
            .isReadyToCapture = true
            return false
        }

        return true
    }

    mtd mustEditInside() => .kind == .Image and .place == .Free and .editInside

    mtd impl moveBaseForm(newPosX, newPosY, newSizeX, newSizeY: s32)
    {
        if .mustEditInside()
        {
            .freePos.x += newPosX
            .freePos.y += newPosY
            .freePos.width += newSizeX
            .freePos.height += newSizeY
        }
        else
        {
            .x += newPosX
            .y += newPosY
            .w += newSizeX
            .h += newSizeY
        }
    }

    mtd impl scaleBaseForm(val: f32)
    {
        .x = cast(s32) (.x * val)
        .y = cast(s32) (.y * val)
        .w = cast(s32) (.w * val)
        .h = cast(s32) (.h * val)
    }

    mtd impl setBaseForm(newPosX, newPosY, newSizeX, newSizeY: s32)
    {
        if .mustEditInside()
        {
            if newSizeX < 0
            {
                .freePos.x     = newPosX + newSizeX
                .freePos.width = -newSizeX
            }
            else
            {
                .freePos.x     = newPosX
                .freePos.width = newSizeX
            }

            if newSizeY < 0
            {
                .freePos.y      = newPosY + newSizeY
                .freePos.height = -newSizeY
            }
            else
            {
                .freePos.y      = newPosY
                .freePos.height = newSizeY
            }
        }
        else
        {
            if newSizeX < 0
            {
                .x = newPosX + newSizeX
                .w = -newSizeX
            }
            else
            {
                .x = newPosX
                .w = newSizeX
            }

            if newSizeY < 0
            {
                .y = newPosY + newSizeY
                .h = -newSizeY
            }
            else
            {
                .y = newPosY
                .h = newSizeY
            }
        }
    }

    mtd impl moveForm(newPosX, newPosY: s32)
    {
        if .mustEditInside()
        {
            .freePos.x += newPosX
            .freePos.y += newPosY
        }
        else
        {
            .x += newPosX
            .y += newPosY
        }
    }

    mtd impl movePoint(pt: u32, newPosX, newPosY: s32) {}

    mtd impl boundRect()->Math.Rectangle
    {
        return Math.Rectangle{.x, .y, .w, .h}
    }

    mtd impl mouse(view: *EditView, pos: Math.Point, evt: *MouseEvent)->bool
    {
        if .kind == .Image and
           .place == .Free and
           evt.kind == .MouseDoubleClick and
           evt.button == .Left and
           .image.isValid()
        {
            .editInside = !.editInside
            return true
        }

        if .mustEditInside()
        {
            evt.modifiers ^= .Shift
        }

        return .gizmo.mouse(view, pos, evt)
    }

    mtd impl paintGizmo(painter: *Painter, z: f32, flags: FormFlags)
    {
        if flags.has(.Hot | .Selected)
        {
            .gizmo.rect = .boundRect() * z
            if .mustEditInside()
            {
                .gizmo.rect.offset(.freePos.x * z, .freePos.y * z)
                .gizmo.rect.width  = .freePos.width * z
                .gizmo.rect.height = .freePos.height * z
                .gizmo.paint(painter, !flags.has(.Selected), Argb.Yellow)
            }
            else
            {
                .gizmo.paint(painter, !flags.has(.Selected))
            }
        }
        else
        {
            .editInside = false
        }
    }

    mtd impl isInside(pt: Math.Point)->bool
    {
        return .boundRect().contains(pt)
    }

    mtd impl hitTest(pt: Math.Point, z: f32)->bool
    {
        var pp = .boundRect() * z
        if .mustEditInside()
        {
            pp.offset(.freePos.x * z, .freePos.y * z)
            pp.width  = .freePos.width * z
            pp.height = .freePos.height * z
            return pp.contains(pt)
        }

        return pp.contains(pt)
    }

    mtd backCaptureMask(mask: *Image)
    {
        let renderer = g_App.getRenderer()

        .image.visitPixels(mask)
        {
            var maskImg = cast(*Image) userData
            var pix1    = maskImg.pixels.buffer + x * 4
            pix1 += maskImg.width * y * 4
            if !pix1[0] do
                dref cast(*u32) pix = 0
        }

        renderer.removeTexture(&.texture)
        .texture = g_App.getRenderer().addImage(.image)
    }

    mtd backCapture()
    {
        let renderer = g_App.getRenderer()
        let x1       = cast(s32) (.x - .capture.backImagePos.x)
        let y1       = cast(s32) (.y - .capture.backImagePos.y)

        if .w > 1 and .h > 1 and .isReadyToCapture
        {
            .isReadyToCapture = false
            .dirty            = false
            .textureKind      = .kind
            renderer.removeTexture(&.texture)
            .image   = .capture.backImg.toCrop(x1, y1, .w, .h)
            .texture = g_App.getRenderer().addImage(.image)
        }
        elif .image.isValid()
        {
            .texture = g_App.getRenderer().addImage(.image)
        }
    }

    mtd impl paint(painter: *Painter, cxt: FormPaintContext)
    {
        let z     = cxt.z
        let bSize = BaseForm.adaptSize(.borderSize, MinBorderSize, MaxBorderSize, cxt)
        var pp    = .boundRect() * z
        let pen   = Pen.createSolid(.borderColor, bSize)

        if !cxt.quickMode
        {
            let renderer = g_App.getRenderer()
            let x1       = cast(s32) (.x - .capture.backImagePos.x)
            let y1       = cast(s32) (.y - .capture.backImagePos.y)

            switch .kind
            {
            case Image:
                if (!.texture.isValid() or .textureKind != .kind or .dirty) and .image.isValid()
                {
                    .dirty       = false
                    .textureKind = .kind
                    renderer.removeTexture(&.texture)
                    .texture = renderer.addImage(.image)
                    if !.freePos.width do
                        .freePos.width = .image.width
                    if !.freePos.height do
                        .freePos.height = .image.height
                }

            case Capture:
                if !.texture.isValid() or
                   .textureKind != .kind
                {
                    .backCapture()
                }

            case Blur, Pixelate:
                if !.texture.isValid() or
                   .xc != .x or .yc != .y or .wc != .w or .hc != .h or
                   .textureKind != .kind or
                   .dirty or
                   .stc != .strength or
                   .serial != .capture.serialBackTexture
                {
                    if .w > 1 and .h > 1
                    {
                        .dirty       = false
                        .serial      = .capture.serialBackTexture
                        .textureKind = .kind
                        .stc         = .strength
                        .xc          = .x
                        .yc          = .y
                        .wc          = .w
                        .hc          = .h
                        renderer.removeTexture(&.texture)
                        var img = .capture.backImg.toCrop(x1, y1, .w, .h)
                        let st  = Math.map(cast(f32) .strength, 0, 100, 1, 16)
                        let nw  = Math.max(cast(s32) (img.width / st), 4)
                        let nh  = Math.max(cast(s32) (img.height / st), 4)
                        img.resize(nw, nh, .Quadratic)
                        img.resize(.w, .h, .kind == .Pixelate ? .Raw : .Quadratic)
                        .texture = g_App.getRenderer().addImage(img)
                    }
                }

            case Zoom:
                if !.texture.isValid() or
                   .xc != .x or .yc != .y or .wc != .w or .hc != .h or
                   .textureKind != .kind or
                   .dirty or
                   .stc != .strength or
                   .serial != .capture.serialBackTexture
                {
                    if .w > 1 and .h > 1
                    {
                        .dirty       = false
                        .serial      = .capture.serialBackTexture
                        .textureKind = .kind
                        .stc         = .strength
                        .xc          = .x
                        .yc          = .y
                        .wc          = .w
                        .hc          = .h
                        renderer.removeTexture(&.texture)
                        const ZoomFactor = 2.5
                        let stw          = cast(s32) Math.map(cast(f32) .strength, 0, 100, 1, .w / ZoomFactor)
                        let sth          = cast(s32) Math.map(cast(f32) .strength, 0, 100, 1, .h / ZoomFactor)
                        var img          = .capture.backImg.toCrop(x1 + stw, y1 + sth, .w - stw * 2, .h - sth * 2)
                        img.resize(.w, .h, .Raw)
                        .texture = g_App.getRenderer().addImage(img)
                    }
                }
            }
        }

        let iconColor = g_App.theme.colors.btnIcon_Icon

        if .borderColor.a and (pp.width <= 2 * bSize or pp.height < 2 * bSize) do
            painter.fillRect(pp, .borderColor)
        else
        {
            if cxt.quickMode
            {
                pp.height -= 10

                var icon: Icon
                switch .kind
                {
                case Image:
                    painter.fillRect(pp, .backColor)
                    icon = Icon.from(&g_Main.icons48, 13)
                case Blur:
                    icon = Icon.from(&g_Main.icons48, 14)
                case Pixelate:
                    icon = Icon.from(&g_Main.icons48, 15)
                case Zoom:
                    icon = Icon.from(&g_Main.icons48, 16)
                case Capture:
                    icon = Icon.from(&g_Main.icons48, 18)
                }

                icon.sizeX, icon.sizeY = 32
                icon.paint(painter, pp.horzCenter() - icon.sizeX * 0.5, pp.vertCenter() - icon.sizeY * 0.5, iconColor)
                pp.inflate(-bSize * 0.5)
                painter.drawRect(pp, pen)
            }
            elif .kind == .Capture and !.texture.isValid()
            {
                painter.drawRect(pp, Argb.White, 4)
            }
            elif .kind == .Image and !.image.isValid()
            {
                painter.fillRect(pp, .backColor)
                let icon = Icon.from(&g_Main.icons48, 13)
                icon.paint(painter, pp.horzCenter() - icon.sizeX * 0.5, pp.vertCenter() - icon.sizeY * 0.5, iconColor)
                pp.inflate(-bSize * 0.5)
                painter.drawRect(pp, pen)
            }
            elif .kind == .Image
            {
                painter.fillRect(pp, .backColor)
                switch .place
                {
                case Frame:
                    painter.drawTexture(pp, .texture)
                case Center:
                    var pp1 = pp
                    pp1.x      = pp.horzCenter() - (.texture.width / 2) * z
                    pp1.y      = pp.vertCenter() - (.texture.height / 2) * z
                    pp1.width  = .texture.width * z
                    pp1.height = .texture.height * z
                    painter.drawTexture(pp1, .texture)
                case Free:
                    var pp1 = pp
                    pp1.offset(.freePos.x * z, .freePos.y * z)
                    pp1.width  = .freePos.width * z
                    pp1.height = .freePos.height * z
                    painter.drawTexture(pp1, .texture)
                }

                pp.inflate(-bSize * 0.5)
                painter.drawRect(pp, pen)
            }
            else
            {
                painter.drawTexture(pp, .texture)
                pp.inflate(-bSize * 0.5)
                painter.drawRect(pp, pen)
            }
        }
    }

    mtd impl setup(capt: *Capture)
    {
        .capture = capt

        with me.gizmo
        {
            .roundShape        = true
            .anchorSize        = 10
            .trackChange       = true
            .paintSimpleBorder = false
            .paintBigBorder    = true
        }

        .gizmo.sigChanged = mtd||(g, first, hitMove)
        {
            g.apply(me, first, hitMove)
        }
    }
}
