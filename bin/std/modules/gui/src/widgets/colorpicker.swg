#global public
using Core, Pixel

enum ColorPickerMode
{
    SaturationLightness
    HueSaturation
    HueLightness
    GreenBlue
    RedBlue
    RedGreen
    HueVert
    HueHorz
    LightnessVert
    LightnessHorz
    SaturationVert
    SaturationHorz
    AlphaHorz
    AlphaVert
    RedHorz
    RedVert
    GreenHorz
    GreenVert
    BlueHorz
    BlueVert
}

struct ColorPicker
{
    using wnd:      Wnd
    hue, sat, lum:  f32
    a, r, g, b:     f32
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
        var clientRect = getClientRect()

        #[Swag.Complete]
        switch mode
        {
        case SaturationLightness:
            xCur = cast(s32) (sat * clientRect.width)
            let l1 = Math.lerp(1, 0.5, sat)
            var l2 = 1 - (lum / l1)
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

        case HueSaturation:
            xCur = cast(s32) (hue * clientRect.width)
            yCur = cast(s32) ((1 - sat) * clientRect.height)
        case HueLightness:
            xCur = cast(s32) (hue * clientRect.width)
            yCur = cast(s32) ((1 - lum) * clientRect.height)
        case GreenBlue:
            xCur = cast(s32) (g * clientRect.width)
            yCur = cast(s32) (b * clientRect.height)
        case RedBlue:
            xCur = cast(s32) (r * clientRect.width)
            yCur = cast(s32) (b * clientRect.height)
        case RedGreen:
            xCur = cast(s32) (r * clientRect.width)
            yCur = cast(s32) (g * clientRect.height)

        case HueVert:
            yCur = cast(s32) (hue * clientRect.height)
        case HueHorz:
            xCur = cast(s32) (hue * clientRect.width)

        case LightnessVert:
            yCur = cast(s32) (lum * clientRect.height)
        case LightnessHorz:
            xCur = cast(s32) (lum * clientRect.width)

        case SaturationVert:
            yCur = cast(s32) (sat * clientRect.height)
        case SaturationHorz:
            xCur = cast(s32) (sat * clientRect.width)

        case AlphaHorz:
            xCur = cast(s32) (a * clientRect.width)
        case AlphaVert:
            yCur = cast(s32) (a * clientRect.height)
        case RedHorz:
            xCur = cast(s32) (r * clientRect.width)
        case RedVert:
            yCur = cast(s32) (r * clientRect.height)
        case GreenHorz:
            xCur = cast(s32) (g * clientRect.width)
        case GreenVert:
            yCur = cast(s32) (g * clientRect.height)
        case BlueHorz:
            xCur = cast(s32) (b * clientRect.width)
        case BlueVert:
            yCur = cast(s32) (b * clientRect.height)
        }
    }

    mtd impl onPaintEvent(evt: *PaintEvent)
    {
        let bc = evt.bc
        let painter = bc.painter
        var clientRect = getClientRect()
        let theme = getTheme()
        let rects = getThemeRects()
        let colors = getThemeColors()

        // Reconstruct image
        if dirty
        {
            dirty = false
            if !img.pixels.buffer
                img = Image.create(cast(s32) clientRect.width, cast(s32) clientRect.height, .RGBA8)

            var modeFill: Image.FillHslType = undefined

            if mode == .AlphaHorz
            {
                var c = Color.fromHsl(hue, sat, lum)
                var c1 = c; c1.a = 0
                var c2 = c; c2.a = 255
                img.fillGradient2(c1, c2)
            }
            elif mode == .AlphaVert
            {
                var c = Color.fromHsl(hue, sat, lum)
                var c1 = c; c1.a = 255
                var c2 = c; c2.a = 0
                img.fillGradient4(c1, c1, c2, c2)
            }
            elif mode == .RedHorz
            {
                var c = Color.fromRgbf(r, g, b)
                var c1 = c; c1.r = 0
                var c2 = c; c2.r = 255
                img.fillGradient2(c1, c2)
            }
            elif mode == .RedVert
            {
                var c = Color.fromRgbf(r, g, b)
                var c1 = c; c1.r = 255
                var c2 = c; c2.r = 0
                img.fillGradient4(c1, c1, c2, c2)
            }
            elif mode == .GreenHorz
            {
                var c = Color.fromRgbf(r, g, b)
                var c1 = c; c1.g = 0
                var c2 = c; c2.g = 255
                img.fillGradient2(c1, c2)
            }
            elif mode == .GreenVert
            {
                var c = Color.fromRgbf(r, g, b)
                var c1 = c; c1.g = 255
                var c2 = c; c2.g = 0
                img.fillGradient4(c1, c1, c2, c2)
            }
            elif mode == .BlueHorz
            {
                var c = Color.fromRgbf(r, g, b)
                var c1 = c; c1.b = 0
                var c2 = c; c2.b = 255
                img.fillGradient2(c1, c2)
            }
            elif mode == .BlueVert
            {
                var c = Color.fromRgbf(r, g, b)
                var c1 = c; c1.b = 255
                var c2 = c; c2.b = 0
                img.fillGradient4(c1, c1, c2, c2)
            }
            else
            {
                switch mode
                {
                case SaturationLightness:   modeFill = .SaturationLightness
                case HueSaturation:         modeFill = .HueSaturation
                case HueLightness:          modeFill = .HueLightness
                case GreenBlue:             modeFill = .GreenBlue
                case RedBlue:               modeFill = .RedBlue
                case RedGreen:              modeFill = .RedGreen
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
                getApp().renderer.updateTexture(texture, img.pixels.buffer)
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

    private mtd computeRGB()
    {
        var c = Color.fromHsl(hue, sat, lum)
        (r, g, b) = c.toRgbf()
    }

    private mtd computeHsl()
    {
        var c = Color.fromRgbf(r, g, b)
        (hue, sat, lum) = c.toHsl()
    }

    mtd impl onPostPaintEvent(evt: *PaintEvent)
    {
        let bc = evt.bc
        let painter = bc.painter
        let theme = getTheme()
        let rects = getThemeRects()
        var clientRect = getClientRect()
        let metrics = getThemeMetrics()
        let colors = getThemeColors()

        #[Swag.Complete]
        switch mode
        {
        case SaturationLightness, HueSaturation, HueLightness, GreenBlue, RedBlue, RedGreen:
            let size = metrics.colorPicker_RoundPick
            var rect = Math.Rectangle{xCur-size, yCur-size, 2*size, 2*size}
            theme.drawSubRect(painter, rect, &rects.colorPicker_RoundPick, colors.colorPicker_Mark)

        case RedVert, GreenVert, BlueVert, AlphaVert, HueVert, LightnessVert, SaturationVert:
            let size = metrics.colorPicker_SquarePick
            var rect = Math.Rectangle{clientRect.horzCenter()-size, yCur-size, 2*size, 2*size}
            theme.drawSubRect(painter, rect, &rects.colorPicker_SquarePick, colors.colorPicker_Mark)

        case RedHorz, GreenHorz, BlueHorz, AlphaHorz, HueHorz, LightnessHorz, SaturationHorz:
            let size = metrics.colorPicker_SquarePick
            var rect = Math.Rectangle{xCur-size, clientRect.vertCenter()-size, 2*size, 2*size}
            theme.drawSubRect(painter, rect, &rects.colorPicker_SquarePick, colors.colorPicker_Mark)
        }
    }

    mtd impl onMouseEvent(evt: *MouseEvent)
    {
        var pos = surfaceToLocal(evt.surfacePos)
        var clientRect = getClientRect()
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
                let x = Math.clamp(pos.x / clientRect.width, 0, 1)
                let y = Math.clamp(pos.y / clientRect.height, 0, 1)

                #[Swag.Complete]
                switch mode
                {
                case SaturationLightness:
                    sat = Math.clamp(x, 0, 1)
                    let l1 = Math.lerp(1, 0.5, sat)
                    lum = Math.lerp(l1, 0, y)

                case HueSaturation:     hue = x; sat = 1 - y; computeRGB()
                case HueLightness:      hue = x; lum = 1 - y; computeRGB()
                case GreenBlue:         g = x; b = y; computeHsl()
                case RedBlue:           r = x; b = y; computeHsl()
                case RedGreen:          r = x; g = y; computeHsl()
                case HueHorz:           hue = x; computeRGB()
                case HueVert:           hue = y; computeRGB()
                case LightnessHorz:     lum = x; computeRGB()
                case LightnessVert:     lum = y; computeRGB()
                case SaturationHorz:    sat = x; computeRGB()
                case SaturationVert:    sat = y; computeRGB()
                case AlphaHorz:         a = x
                case AlphaVert:         a = y
                case RedHorz:           r = x; computeHsl()
                case RedVert:           r = y; computeHsl()
                case GreenHorz:         g = x; computeHsl()
                case GreenVert:         g = y; computeHsl()
                case BlueHorz:          b = x; computeHsl()
                case BlueVert:          b = y; computeHsl()
                }

                computeXY()
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
    func create(parent: *Wnd, position: Math.Rectangle = {}, id: WndId = null)->*ColorPicker
    {
        Debug.assert(parent != null)
        let ret = Wnd.create'ColorPicker(parent, null, position, id)
        ret.wndFlags |= .PostChildsPaint
        ret.setColor(Argb.Red)
        ret.invalidatePadding = {-32, -32, -32, -32}
        return ret
    }

    // Change one component, HSL or alpha
    mtd change(h, s, l, a: f32 = -1)
    {
        var changed = false
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

        if #self.a >= 0 and self.a != #self.a
        {
            self.a = #self.a
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
        (a, r, g, b)    = col.toArgbf()
        dirty = true
        invalidate()
        sigChanged.call(self)
    }

    // Returns the selected color
    mtd getColor()->Color
    {
        return Color.fromArgbf(a, r, g, b)
    }

    // Set the color picker box mode
    mtd setMode(mode: ColorPickerMode)
    {
        self.mode = #self.mode
        dirty = true
        invalidate()
    }
}