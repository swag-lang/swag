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
        @init(&me.texture, 1)
    }
}

impl IForm for FormImage
{
    mtd impl isSame(ptr: *BaseForm)->bool
    {
        let other = cast(*FormImage) ptr
        if me.kind == .Image and me.backColor != other.backColor:
            return false
        return me.borderColor == other.borderColor and
               me.borderSize == other.borderSize and
               me.alpha == other.alpha and
               me.kind == other.kind and
               me.paintShadow == other.paintShadow and
               me.strength == other.strength
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
        if srcf.image.isValid():
            dstf.image = srcf.image
    }

    mtd impl edit(evt: *MouseEvent, movingStart: Math.Point, localPos: Math.Point, zoom: f32)->bool
    {
        let xa = cast(s32) (movingStart.x / zoom)
        let ya = cast(s32) (movingStart.y / zoom)
        let xb = cast(s32) (localPos.x / zoom)
        let yb = cast(s32) (localPos.y / zoom)
        me.setBaseForm(xa, ya, xb - xa, yb - ya)
        if evt.kind == .MouseReleased and evt.button == .Left
        {
            me.isReadyToCapture = true
            return false
        }

        return true
    }

    mtd mustEditInside() => me.kind == .Image and me.place == .Free and me.editInside

    mtd impl moveBaseForm(newPosX, newPosY, newSizeX, newSizeY: s32)
    {
        if me.mustEditInside()
        {
            me.freePos.x += newPosX
            me.freePos.y += newPosY
            me.freePos.width += newSizeX
            me.freePos.height += newSizeY
        }
        else
        {
            me.x += newPosX
            me.y += newPosY
            me.w += newSizeX
            me.h += newSizeY
        }
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
        if me.mustEditInside()
        {
            if newSizeX < 0
            {
                me.freePos.x     = newPosX + newSizeX
                me.freePos.width = -newSizeX
            }
            else
            {
                me.freePos.x     = newPosX
                me.freePos.width = newSizeX
            }

            if newSizeY < 0
            {
                me.freePos.y      = newPosY + newSizeY
                me.freePos.height = -newSizeY
            }
            else
            {
                me.freePos.y      = newPosY
                me.freePos.height = newSizeY
            }
        }
        else
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
    }

    mtd impl moveForm(newPosX, newPosY: s32)
    {
        if me.mustEditInside()
        {
            me.freePos.x += newPosX
            me.freePos.y += newPosY
        }
        else
        {
            me.x += newPosX
            me.y += newPosY
        }
    }

    mtd impl movePoint(pt: u32, newPosX, newPosY: s32) {}

    mtd impl boundRect()->Math.Rectangle
    {
        return Math.Rectangle{me.x, me.y, me.w, me.h}
    }

    mtd impl mouse(view: *EditView, pos: Math.Point, evt: *MouseEvent)->bool
    {
        if me.kind == .Image and
           me.place == .Free and
           evt.kind == .MouseDoubleClick and
           evt.button == .Left and
           me.image.isValid()
        {
            me.editInside = !me.editInside
            return true
        }

        if me.mustEditInside()
        {
            evt.modifiers ^= .Shift
        }

        return me.gizmo.mouse(view, pos, evt)
    }

    mtd impl paintGizmo(painter: *Painter, z: f32, flags: FormFlags)
    {
        if flags.has(.Hot | .Selected)
        {
            me.gizmo.rect = me.boundRect() * z
            if me.mustEditInside()
            {
                me.gizmo.rect.offset(me.freePos.x * z, me.freePos.y * z)
                me.gizmo.rect.width  = me.freePos.width * z
                me.gizmo.rect.height = me.freePos.height * z
                me.gizmo.paint(painter, !flags.has(.Selected), Argb.Yellow)
            }
            else
            {
                me.gizmo.paint(painter, !flags.has(.Selected))
            }
        }
        else
        {
            me.editInside = false
        }
    }

    mtd impl isInside(pt: Math.Point)->bool
    {
        return me.boundRect().contains(pt)
    }

    mtd impl hitTest(pt: Math.Point, z: f32)->bool
    {
        var pp = me.boundRect() * z
        if me.mustEditInside()
        {
            pp.offset(me.freePos.x * z, me.freePos.y * z)
            pp.width  = me.freePos.width * z
            pp.height = me.freePos.height * z
            return pp.contains(pt)
        }

        return pp.contains(pt)
    }

    mtd backCaptureMask(mask: *Image)
    {
        let renderer = g_App.getRenderer()

        me.image.visitPixels(mask)
        {
            var maskImg = cast(*Image) userData
            var pix1    = maskImg.pixels.buffer + x * 4
            pix1 += maskImg.width * y * 4
            if !pix1[0]:
                dref cast(*u32) pix = 0
        }

        renderer.removeTexture(&me.texture)
        me.texture = g_App.getRenderer().addImage(me.image)
    }

    mtd backCapture()
    {
        let renderer = g_App.getRenderer()
        let x1       = cast(s32) (me.x - me.capture.backImagePos.x)
        let y1       = cast(s32) (me.y - me.capture.backImagePos.y)

        if me.w > 1 and me.h > 1 and me.isReadyToCapture
        {
            me.isReadyToCapture = false
            me.dirty            = false
            me.textureKind      = me.kind
            renderer.removeTexture(&me.texture)
            me.image   = me.capture.backImg.toCrop(x1, y1, me.w, me.h)
            me.texture = g_App.getRenderer().addImage(me.image)
        }
        elif me.image.isValid()
        {
            me.texture = g_App.getRenderer().addImage(me.image)
        }
    }

    mtd impl paint(painter: *Painter, cxt: FormPaintContext)
    {
        let z     = cxt.z
        let bSize = BaseForm.adaptSize(me.borderSize, MinBorderSize, MaxBorderSize, cxt)
        var pp    = me.boundRect() * z
        let pen   = Pen.createSolid(me.borderColor, bSize)

        if !cxt.quickMode
        {
            let renderer = g_App.getRenderer()
            let x1       = cast(s32) (me.x - me.capture.backImagePos.x)
            let y1       = cast(s32) (me.y - me.capture.backImagePos.y)

            switch me.kind
            {
            case Image:
                if (!me.texture.isValid() or me.textureKind != me.kind or me.dirty) and me.image.isValid()
                {
                    me.dirty       = false
                    me.textureKind = me.kind
                    renderer.removeTexture(&me.texture)
                    me.texture = renderer.addImage(me.image)
                    if !me.freePos.width:
                        me.freePos.width = me.image.width
                    if !me.freePos.height:
                        me.freePos.height = me.image.height
                }

            case Capture:
                if !me.texture.isValid() or
                   me.textureKind != me.kind
                {
                    me.backCapture()
                }

            case Blur, Pixelate:
                if !me.texture.isValid() or
                   me.xc != me.x or me.yc != me.y or me.wc != me.w or me.hc != me.h or
                   me.textureKind != me.kind or
                   me.dirty or
                   me.stc != me.strength or
                   me.serial != me.capture.serialBackTexture
                {
                    if me.w > 1 and me.h > 1
                    {
                        me.dirty       = false
                        me.serial      = me.capture.serialBackTexture
                        me.textureKind = me.kind
                        me.stc         = me.strength
                        me.xc          = me.x
                        me.yc          = me.y
                        me.wc          = me.w
                        me.hc          = me.h
                        renderer.removeTexture(&me.texture)
                        var img = me.capture.backImg.toCrop(x1, y1, me.w, me.h)
                        let st  = Math.map(cast(f32) me.strength, 0, 100, 1, 16)
                        let nw  = Math.max(cast(s32) (img.width / st), 4)
                        let nh  = Math.max(cast(s32) (img.height / st), 4)
                        img.resize(nw, nh, .Quadratic)
                        img.resize(me.w, me.h, me.kind == .Pixelate ? .Raw : .Quadratic)
                        me.texture = g_App.getRenderer().addImage(img)
                    }
                }

            case Zoom:
                if !me.texture.isValid() or
                   me.xc != me.x or me.yc != me.y or me.wc != me.w or me.hc != me.h or
                   me.textureKind != me.kind or
                   me.dirty or
                   me.stc != me.strength or
                   me.serial != me.capture.serialBackTexture
                {
                    if me.w > 1 and me.h > 1
                    {
                        me.dirty       = false
                        me.serial      = me.capture.serialBackTexture
                        me.textureKind = me.kind
                        me.stc         = me.strength
                        me.xc          = me.x
                        me.yc          = me.y
                        me.wc          = me.w
                        me.hc          = me.h
                        renderer.removeTexture(&me.texture)
                        const ZoomFactor = 2.5
                        let stw          = cast(s32) Math.map(cast(f32) me.strength, 0, 100, 1, me.w / ZoomFactor)
                        let sth          = cast(s32) Math.map(cast(f32) me.strength, 0, 100, 1, me.h / ZoomFactor)
                        var img          = me.capture.backImg.toCrop(x1 + stw, y1 + sth, me.w - stw * 2, me.h - sth * 2)
                        img.resize(me.w, me.h, .Raw)
                        me.texture = g_App.getRenderer().addImage(img)
                    }
                }
            }
        }

        let iconColor = g_App.theme.colors.btnIcon_Icon

        if me.borderColor.a and (pp.width <= 2 * bSize or pp.height < 2 * bSize):
            painter.fillRect(pp, me.borderColor)
        else
        {
            if cxt.quickMode
            {
                pp.height -= 10

                var icon: Icon
                switch me.kind
                {
                case Image:
                    painter.fillRect(pp, me.backColor)
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
            elif me.kind == .Capture and !me.texture.isValid()
            {
                painter.drawRect(pp, Argb.White, 4)
            }
            elif me.kind == .Image and !me.image.isValid()
            {
                painter.fillRect(pp, me.backColor)
                var icon = Icon.from(&g_Main.icons48, 13)
                icon.paint(painter, pp.horzCenter() - icon.sizeX * 0.5, pp.vertCenter() - icon.sizeY * 0.5, iconColor)
                pp.inflate(-bSize * 0.5)
                painter.drawRect(pp, pen)
            }
            elif me.kind == .Image
            {
                painter.fillRect(pp, me.backColor)
                switch me.place
                {
                case Frame:
                    painter.drawTexture(pp, me.texture)
                case Center:
                    var pp1 = pp
                    pp1.x      = pp.horzCenter() - (me.texture.width / 2) * z
                    pp1.y      = pp.vertCenter() - (me.texture.height / 2) * z
                    pp1.width  = me.texture.width * z
                    pp1.height = me.texture.height * z
                    painter.drawTexture(pp1, me.texture)
                case Free:
                    var pp1 = pp
                    pp1.offset(me.freePos.x * z, me.freePos.y * z)
                    pp1.width  = me.freePos.width * z
                    pp1.height = me.freePos.height * z
                    painter.drawTexture(pp1, me.texture)
                }

                pp.inflate(-bSize * 0.5)
                painter.drawRect(pp, pen)
            }
            else
            {
                painter.drawTexture(pp, me.texture)
                pp.inflate(-bSize * 0.5)
                painter.drawRect(pp, pen)
            }
        }
    }

    mtd impl setup(capt: *Capture)
    {
        me.capture = capt

        with me.gizmo
        {
            .roundShape        = true
            .anchorSize        = 10
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
