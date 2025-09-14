#global public
using Core, Pixel

// Enumeration of different color picker display modes.
// Defines how color components are mapped to the 2D picker area or 1D sliders.
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
    using wnd:         Wnd
    hue, sat, lum:     f32
    a, r, g, b:        f32
    mode               = ColorPickerMode.SaturationLightness
    sigChanged:        SigArray'func||(*ColorPicker)

    private
    {
        dirty:          bool = true
        isMoving:       bool
        img:            Image
        texture:        Texture
        xCur, yCur:     s32
    }
}

impl IWnd for ColorPicker
{
    mtd computeXY()
    {
        let clientRect = me.getClientRect()

        with me
        {
            #[Swag.Complete]
            switch .mode
            {
            case SaturationLightness:
                .xCur = cast() (.sat * clientRect.width)
                let l1 = Math.lerp(1, 0.5, .sat)
                var l2 = 1 - (.lum / l1)
                if l2 < 0
                {
                    l2    = -l2
                    l2    = 1 - l2
                    .yCur = 0
                    .xCur = cast() (l2 * clientRect.width)
                }
                else
                {
                    .yCur = cast() (l2 * clientRect.height)
                }

            case HueSaturation:
                .xCur = cast() (.hue * clientRect.width)
                .yCur = cast() ((1 - .sat) * clientRect.height)
            case HueLightness:
                .xCur = cast() (.hue * clientRect.width)
                .yCur = cast() ((1 - .lum) * clientRect.height)
            case GreenBlue:
                .xCur = cast() (.g * clientRect.width)
                .yCur = cast() (.b * clientRect.height)
            case RedBlue:
                .xCur = cast() (.r * clientRect.width)
                .yCur = cast() (.b * clientRect.height)
            case RedGreen:
                .xCur = cast() (.r * clientRect.width)
                .yCur = cast() (.g * clientRect.height)

            case HueVert:
                .yCur = cast() (.hue * clientRect.height)
            case HueHorz:
                .xCur = cast() (.hue * clientRect.width)

            case LightnessVert:
                .yCur = cast() (.lum * clientRect.height)
            case LightnessHorz:
                .xCur = cast() (.lum * clientRect.width)

            case SaturationVert:
                .yCur = cast() (.sat * clientRect.height)
            case SaturationHorz:
                .xCur = cast() (.sat * clientRect.width)

            case AlphaHorz:
                .xCur = cast() (.a * clientRect.width)
            case AlphaVert:
                .yCur = cast() (.a * clientRect.height)
            case RedHorz:
                .xCur = cast() (.r * clientRect.width)
            case RedVert:
                .yCur = cast() (.r * clientRect.height)
            case GreenHorz:
                .xCur = cast() (.g * clientRect.width)
            case GreenVert:
                .yCur = cast() (.g * clientRect.height)
            case BlueHorz:
                .xCur = cast() (.b * clientRect.width)
            case BlueVert:
                .yCur = cast() (.b * clientRect.height)
            }
        }
    }

    mtd impl onPaintEvent(evt: *PaintEvent)
    {
        let bc         = evt.bc
        let painter    = bc.painter
        let clientRect = me.getClientRect()
        let theme      = me.getTheme()
        let rects      = me.getThemeRects()
        let colors     = me.getThemeColors()

        // Reconstruct image
        if me.dirty
        {
            me.dirty = false
            if !me.img.pixels.buffer:
                me.img = Image.create(cast(s32) clientRect.width, cast(s32) clientRect.height, .RGBA8)

            var modeFill: Image.FillHslType = undefined

            if me.mode == .AlphaHorz
            {
                let c  = Color.fromHsl(me.hue, me.sat, me.lum)
                var c1 = c
                c1.a = 0
                var c2 = c
                c2.a = 255
                me.img.fillGradient2(c1, c2)
            }
            elif me.mode == .AlphaVert
            {
                let c  = Color.fromHsl(me.hue, me.sat, me.lum)
                var c1 = c
                c1.a = 255
                var c2 = c
                c2.a = 0
                me.img.fillGradient4(c1, c1, c2, c2)
            }
            elif me.mode == .RedHorz
            {
                let c  = Color.fromRgbf(me.r, me.g, me.b)
                var c1 = c
                c1.r = 0
                var c2 = c
                c2.r = 255
                me.img.fillGradient2(c1, c2)
            }
            elif me.mode == .RedVert
            {
                let c  = Color.fromRgbf(me.r, me.g, me.b)
                var c1 = c
                c1.r = 255
                var c2 = c
                c2.r = 0
                me.img.fillGradient4(c1, c1, c2, c2)
            }
            elif me.mode == .GreenHorz
            {
                let c  = Color.fromRgbf(me.r, me.g, me.b)
                var c1 = c
                c1.g = 0
                var c2 = c
                c2.g = 255
                me.img.fillGradient2(c1, c2)
            }
            elif me.mode == .GreenVert
            {
                let c  = Color.fromRgbf(me.r, me.g, me.b)
                var c1 = c
                c1.g = 255
                var c2 = c
                c2.g = 0
                me.img.fillGradient4(c1, c1, c2, c2)
            }
            elif me.mode == .BlueHorz
            {
                let c  = Color.fromRgbf(me.r, me.g, me.b)
                var c1 = c
                c1.b = 0
                var c2 = c
                c2.b = 255
                me.img.fillGradient2(c1, c2)
            }
            elif me.mode == .BlueVert
            {
                let c  = Color.fromRgbf(me.r, me.g, me.b)
                var c1 = c
                c1.b = 255
                var c2 = c
                c2.b = 0
                me.img.fillGradient4(c1, c1, c2, c2)
            }
            else
            {
                switch me.mode
                {
                case SaturationLightness:
                    modeFill = .SaturationLightness
                case HueSaturation:
                    modeFill = .HueSaturation
                case HueLightness:
                    modeFill = .HueLightness
                case GreenBlue:
                    modeFill = .GreenBlue
                case RedBlue:
                    modeFill = .RedBlue
                case RedGreen:
                    modeFill = .RedGreen
                case HueVert:
                    modeFill = .HueVert
                case HueHorz:
                    modeFill = .HueHorz
                case LightnessVert:
                    modeFill = .LightnessVert
                case LightnessHorz:
                    modeFill = .LightnessHorz
                case SaturationVert:
                    modeFill = .SaturationVert
                case SaturationHorz:
                    modeFill = .SaturationHorz
                }

                me.img.fillHsl(modeFill, Color.fromHsl(me.hue, me.sat, me.lum))
            }

            if me.texture.valid:
                me.getApp().renderer.updateTexture(me.texture, me.img.pixels.buffer)
            else:
                me.texture = me.getApp().renderer.addImage(me.img)
            me.computeXY()
        }

        // Clipping
        painter.startClippingRegion()
        theme.drawSubRect(painter, clientRect, &rects.colorPicker_Bk, Argb.White)
        painter.endClippingRegion()

        // Image
        if me.mode == .AlphaHorz or me.mode == .AlphaVert:
            painter.fillRect(clientRect, Brush.createHatch(.CheckBoardLarge, colors.colorPicker_BackAlpha))
        painter.drawTexture(clientRect, me.texture)
        painter.resetClippingRegion()

        // Border
        theme.drawSubRect(painter, clientRect, &rects.colorPicker_Border, colors.colorPicker_Border)
    }

    internal mtd computeRGB()
    {
        let c = Color.fromHsl(me.hue, me.sat, me.lum)
        (me.r, me.g, me.b) = c.toRgbf()
    }

    internal mtd computeHsl()
    {
        let c = Color.fromRgbf(me.r, me.g, me.b)
        (me.hue, me.sat, me.lum) = c.toHsl()
    }

    mtd impl onPostPaintEvent(evt: *PaintEvent)
    {
        let bc         = evt.bc
        let painter    = bc.painter
        let theme      = me.getTheme()
        let rects      = me.getThemeRects()
        let clientRect = me.getClientRect()
        let metrics    = me.getThemeMetrics()
        let colors     = me.getThemeColors()

        #[Swag.Complete]
        switch me.mode
        {
        case SaturationLightness, HueSaturation, HueLightness, GreenBlue, RedBlue, RedGreen:
            let size = metrics.colorPicker_RoundPick
            let rect = Math.Rectangle{me.xCur - size, me.yCur - size, 2 * size, 2 * size}
            theme.drawSubRect(painter, rect, &rects.colorPicker_RoundPick, colors.colorPicker_Mark)

        case RedVert, GreenVert, BlueVert, AlphaVert, HueVert, LightnessVert, SaturationVert:
            let size = metrics.colorPicker_SquarePick
            let rect = Math.Rectangle{clientRect.horzCenter() - size, me.yCur - size, 2 * size, 2 * size}
            theme.drawSubRect(painter, rect, &rects.colorPicker_SquarePick, colors.colorPicker_Mark)

        case RedHorz, GreenHorz, BlueHorz, AlphaHorz, HueHorz, LightnessHorz, SaturationHorz:
            let size = metrics.colorPicker_SquarePick
            let rect = Math.Rectangle{me.xCur - size, clientRect.vertCenter() - size, 2 * size, 2 * size}
            theme.drawSubRect(painter, rect, &rects.colorPicker_SquarePick, colors.colorPicker_Mark)
        }
    }

    mtd impl onMouseEvent(evt: *MouseEvent)
    {
        var pos        = me.surfaceToLocal(evt.surfacePos)
        let clientRect = me.getClientRect()
        pos.x = Math.clamp(pos.x, 0, clientRect.width)
        pos.y = Math.clamp(pos.y, 0, clientRect.height)

        switch evt.kind
        {
        case MousePressed:
            if evt.button == .Left
            {
                me.isMoving = true
                me.captureMouse()
                me.invalidate()
                fallthrough
            }

        case MouseMove:
            if me.isMoving
            {
                let x = Math.clamp(pos.x / clientRect.width, 0, 1)
                let y = Math.clamp(pos.y / clientRect.height, 0, 1)

                with me
                {
                    #[Swag.Complete]
                    switch .mode
                    {
                    case SaturationLightness:
                        .sat = Math.clamp(x, 0, 1)
                        let l1 = Math.lerp(1, 0.5, .sat)
                        .lum = Math.lerp(l1, 0, y)
                    case HueSaturation:
                        .hue = x
                        .sat = 1 - y
                        .computeRGB()
                    case HueLightness:
                        .hue = x
                        .lum = 1 - y
                        .computeRGB()
                    case GreenBlue:
                        .g = x
                        .b = y
                        .computeHsl()
                    case RedBlue:
                        .r = x
                        .b = y
                        .computeHsl()
                    case RedGreen:
                        .r = x
                        .g = y
                        .computeHsl()
                    case HueHorz:
                        .hue = x
                        .computeRGB()
                    case HueVert:
                        .hue = y
                        .computeRGB()
                    case LightnessHorz:
                        .lum = x
                        .computeRGB()
                    case LightnessVert:
                        .lum = y
                        .computeRGB()
                    case SaturationHorz:
                        .sat = x
                        .computeRGB()
                    case SaturationVert:
                        .sat = y
                        .computeRGB()
                    case AlphaHorz:
                        .a = x
                    case AlphaVert:
                        .a = y
                    case RedHorz:
                        .r = x
                        .computeHsl()
                    case RedVert:
                        .r = y
                        .computeHsl()
                    case GreenHorz:
                        .g = x
                        .computeHsl()
                    case GreenVert:
                        .g = y
                        .computeHsl()
                    case BlueHorz:
                        .b = x
                        .computeHsl()
                    case BlueVert:
                        .b = y
                        .computeHsl()
                    }

                    .computeXY()
                    .sigChanged.call(me)
                    .invalidate()
                }
            }

        case MouseReleased:
            if evt.button == .Left and me.isMoving
            {
                me.isMoving = false
                me.releaseMouse()
                me.invalidate()
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
        if h >= 0 and h != me.hue
        {
            me.hue  = h
            changed = true
        }

        if s >= 0 and s != me.sat
        {
            me.sat  = s
            changed = true
        }

        if l >= 0 and l != me.lum
        {
            me.lum  = l
            changed = true
        }

        if a >= 0 and me.a != a
        {
            me.a    = a
            changed = true
        }

        if changed
        {
            me.dirty = true
            me.invalidate()
            me.sigChanged.call(me)
        }
    }

    // Set the base color, and repaint
    mtd setColor(col: Color)
    {
        with me
        {
            (.hue, .sat, .lum) = col.toHsl()
            (.a, .r, .g, .b) = col.toArgbf()
            .dirty = true
            .invalidate()
            .sigChanged.call(me)
        }
    }

    // Returns the selected color
    mtd getColor()->Color
    {
        return Color.fromArgbf(me.a, me.r, me.g, me.b)
    }

    // Set the color picker box mode
    mtd setMode(mode: ColorPickerMode)
    {
        me.mode  = mode
        me.dirty = true
        me.invalidate()
    }
}
