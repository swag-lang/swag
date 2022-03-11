#global public
using Core, Pixel

enum ColorPickerMode
{
    SaturationLightness
    HueVert
    HueHorz
    LightnessVert
    LightnessHorz
    SaturationVert
    SaturationHorz
    AlphaHorz
    AlphaVert
}

struct ColorPicker
{
    using wnd:      Wnd
    hue, sat, lum:  f32
    alpha:          f32
    mode            = ColorPickerMode.SaturationLightness
    sigChanged:     SigArray'closure(*Self)

    dirty:      bool = true
    isMoving:   bool
    img:        Image
    texture:    Texture
    xCur, yCur: s32
}

impl IWnd for ColorPicker
{
    mtd computeXY()
    {
        clientRect := getClientRect()

        #[Swag.Complete]
        switch mode
        {
        case SaturationLightness:
            xCur = cast(s32) (sat * clientRect.width)
            l1 := Math.lerp(1, 0.5, sat)
            l2 := 1 - (lum / l1)
            if l2 < 0
            {
                l2 = -l2
                l2 = 1 - l2
                yCur = 0
                xCur = cast(s32) (l2 * clientRect.width)
            }
            else
            {
                yCur = cast(s32) (l2 * clientRect.height)
            }

        case HueVert:
            yCur = cast(s32) (hue * clientRect.height)
        case HueHorz:
            xCur = cast(s32) (hue * clientRect.width)

        case LightnessVert:
            yCur = cast(s32) ((1 - lum) * clientRect.height)
        case LightnessHorz:
            xCur = cast(s32) ((1 - lum) * clientRect.width)

        case SaturationVert:
            yCur = cast(s32) ((1 - sat) * clientRect.height)
        case SaturationHorz:
            xCur = cast(s32) ((1 - sat) * clientRect.width)

        case AlphaHorz:
            xCur = cast(s32) (alpha * clientRect.width)
        case AlphaVert:
            yCur = cast(s32) (alpha * clientRect.height)
        }
    }

    mtd onPaintEvent(evt: *PaintEvent)
    {
        bc := evt.bc
        painter    := bc.painter
        clientRect := getClientRect()
        theme      := getTheme()
        rects      := getThemeRects()
        colors     := getThemeColors()

        // Reconstruct image
        if dirty
        {
            dirty = false
            if !img.pixels
                img = Image.create(cast(s32) clientRect.width, cast(s32) clientRect.height, .RGBA8)

            var modeFill: Image.FillHslType = ?

            if mode == .AlphaHorz
            {
                c := Color.fromHsl(hue, sat, lum)
                c1 := c; c1.a = 0
                c2 := c; c2.a = 255
                img.fillGradient2(c1, c2)
            }
            elif mode == .AlphaVert
            {
                c := Color.fromHsl(hue, sat, lum)
                c1 := c; c1.a = 255
                c2 := c; c2.a = 0
                img.fillGradient4(c1, c1, c2, c2)
            }
            else
            {
                switch mode
                {
                case SaturationLightness:   modeFill = .SaturationLightness
                case HueVert:               modeFill = .HueVert
                case HueHorz:               modeFill = .HueHorz
                case LightnessVert:         modeFill = .LightnessVert
                case LightnessHorz:         modeFill = .LightnessHorz
                case SaturationVert:        modeFill = .SaturationVert
                case SaturationHorz:        modeFill = .SaturationHorz
                }

                img.fillHsl(modeFill, Color.fromHsl(hue, sat, lum))
            }

            if texture.valid
                getApp().renderer.updateTexture(texture, img.pixels)
            else
                texture = getApp().renderer.addImage(img)
            computeXY()
        }

        // Clipping
        painter.startClippingRegion()
        theme.drawSubRect(painter, clientRect, &rects.colorPicker_Bk, Argb.White)
        painter.endClippingRegion()

        // Image
        if mode == .AlphaHorz or mode == .AlphaVert
            painter.fillRect(clientRect, Brush.createHatch(.CheckBoardLarge, colors.colorPicker_BackAlpha))
        painter.drawTexture(clientRect, texture)
        painter.resetClippingRegion()

        // Border
        theme.drawSubRect(painter, clientRect, &rects.colorPicker_Border, colors.colorPicker_Border)
    }

    mtd onPostPaintEvent(evt: *PaintEvent)
    {
        bc := evt.bc
        painter     := bc.painter
        theme       := getTheme()
        rects       := getThemeRects()
        clientRect  := getClientRect()
        metrics     := getThemeMetrics()
        colors      := getThemeColors()

        #[Swag.Complete]
        switch mode
        {
        case SaturationLightness:
            size := metrics.colorPicker_RoundPick
            rect := Math.Rectangle{xCur-size, yCur-size, 2*size, 2*size}
            theme.drawSubRect(painter, rect, &rects.colorPicker_RoundPick, Argb.Black)
            rect.inflate(1, 1)
            theme.drawSubRect(painter, rect, &rects.colorPicker_RoundPick, colors.colorPicker_Mark)

        case AlphaVert, HueVert, LightnessVert, SaturationVert:
            size := metrics.colorPicker_SquarePickH
            rect := Math.Rectangle{-size, yCur-size, clientRect.width+2*size, 2*size}
            theme.drawSubRect(painter, rect, &rects.colorPicker_SquarePick, Argb.Black)
            rect.inflate(1, 1)
            theme.drawSubRect(painter, rect, &rects.colorPicker_SquarePick, colors.colorPicker_Mark)

        case AlphaHorz, HueHorz, LightnessHorz, SaturationHorz:
            sizeX := metrics.colorPicker_SquarePickH
            sizeY := metrics.colorPicker_SquarePickV
            rect := Math.Rectangle{xCur-sizeX, -sizeY, 2*sizeX, clientRect.height+2*sizeY}
            theme.drawSubRect(painter, rect, &rects.colorPicker_SquarePick, Argb.Black)
            rect.inflate(1, 1)
            theme.drawSubRect(painter, rect, &rects.colorPicker_SquarePick, colors.colorPicker_Mark)
        }
    }

    mtd onMouseEvent(evt: *MouseEvent)
    {
        pos := surfaceToLocal(evt.surfacePos)
        clientRect := getClientRect()
        pos.x = Math.clamp(pos.x, 0, clientRect.width)
        pos.y = Math.clamp(pos.y, 0, clientRect.height)

        switch evt.kind
        {
        case MousePressed:
            if evt.button == .Left
            {
                isMoving = true
                captureMouse()
                invalidate()
                fallthrough
            }

        case MouseMove:
            if isMoving
            {
                xCur = cast(s32) pos.x
                yCur = cast(s32) pos.y
                pix := img.getPixelColor(xCur, yCur)
                (h, s, l) := pix.toHsl()

                #[Swag.Complete]
                switch mode
                {
                case SaturationLightness:
                    sat = s
                    lum = l
                case HueVert, HueHorz:
                    hue = h
                case LightnessVert, LightnessHorz:
                    lum = l
                case SaturationVert, SaturationHorz:
                    sat = s
                case AlphaHorz, AlphaVert:
                    alpha = pix.a / 255.0
                }

                sigChanged.call(self)
                invalidate()
            }

        case MouseReleased:
            if evt.button == .Left and isMoving
            {
                isMoving = false
                releaseMouse()
                invalidate()
            }
        }
    }
}

impl ColorPicker
{
    #[Swag.Discardable]
    func create(parent: *Wnd, position: Math.Rectangle, id: string = null)->*ColorPicker
    {
        Debug.assert(parent != null)
        ret := Wnd.create'ColorPicker(parent, null, position, id)
        ret.wndFlags |= .PostChildsPaint
        ret.setColor(Argb.Red)
        return ret
    }

    mtd changeHsl(h, s, l: f32 = -1)
    {
        changed := false
        if h >= 0 and h != hue
        {
            hue = h
            changed = true
        }

        if s >= 0 and s != sat
        {
            sat = s
            changed = true
        }

        if l >= 0 and l != lum
        {
            lum = l
            changed = true
        }

        if changed
        {
            dirty = true
            invalidate()
            sigChanged.call(self)
        }
    }

    // Set the base color, and repaint
    mtd setColor(col: Color)
    {
        (hue, sat, lum) = col.toHsl()
        alpha = 255.0 / col.a
        dirty = true
        invalidate()
        sigChanged.call(self)
    }

    // Returns the selected color
    mtd getColor()->Color
    {
        c := Color.fromHsl(hue, sat, lum)
        c.a = cast(u8) (alpha * 255)
        return c
    }

    // Set the base color, and repaint
    mtd setMode(mode: ColorPickerMode)
    {
        self.mode = #self.mode
        dirty = true
        invalidate()
    }
}