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
    using wnd:         Wnd
    hue, sat, lum:     f32
    a, r, g, b:        f32
    mode               = ColorPickerMode.SaturationLightness
    sigChanged:        SigArray'func||(*ColorPicker)

    dirty:             bool = true
    isMoving:          bool
    img:               Image
    texture:           Texture
    xCur, yCur:        s32
}

impl IWnd for ColorPicker
{
    mtd computeXY()
    {
        var clientRect = me.getClientRect()

        #[Swag.Complete]
        switch me.mode
        {
        case SaturationLightness:
            me.xCur = cast(s32) (me.sat * clientRect.width)
            let l1 = Math.lerp(1, 0.5, me.sat)
            var l2 = 1 - (me.lum / l1)
            if l2 < 0
            {
                l2      = -l2
                l2      = 1 - l2
                me.yCur = 0
                me.xCur = cast(s32) (l2 * clientRect.width)
            }
            else
            {
                me.yCur = cast(s32) (l2 * clientRect.height)
            }

        case HueSaturation:
            me.xCur = cast(s32) (me.hue * clientRect.width)
            me.yCur = cast(s32) ((1 - me.sat) * clientRect.height)
        case HueLightness:
            me.xCur = cast(s32) (me.hue * clientRect.width)
            me.yCur = cast(s32) ((1 - me.lum) * clientRect.height)
        case GreenBlue:
            me.xCur = cast(s32) (me.g * clientRect.width)
            me.yCur = cast(s32) (me.b * clientRect.height)
        case RedBlue:
            me.xCur = cast(s32) (me.r * clientRect.width)
            me.yCur = cast(s32) (me.b * clientRect.height)
        case RedGreen:
            me.xCur = cast(s32) (me.r * clientRect.width)
            me.yCur = cast(s32) (me.g * clientRect.height)

        case HueVert:
            me.yCur = cast(s32) (me.hue * clientRect.height)
        case HueHorz:
            me.xCur = cast(s32) (me.hue * clientRect.width)

        case LightnessVert:
            me.yCur = cast(s32) (me.lum * clientRect.height)
        case LightnessHorz:
            me.xCur = cast(s32) (me.lum * clientRect.width)

        case SaturationVert:
            me.yCur = cast(s32) (me.sat * clientRect.height)
        case SaturationHorz:
            me.xCur = cast(s32) (me.sat * clientRect.width)

        case AlphaHorz:
            me.xCur = cast(s32) (me.a * clientRect.width)
        case AlphaVert:
            me.yCur = cast(s32) (me.a * clientRect.height)
        case RedHorz:
            me.xCur = cast(s32) (me.r * clientRect.width)
        case RedVert:
            me.yCur = cast(s32) (me.r * clientRect.height)
        case GreenHorz:
            me.xCur = cast(s32) (me.g * clientRect.width)
        case GreenVert:
            me.yCur = cast(s32) (me.g * clientRect.height)
        case BlueHorz:
            me.xCur = cast(s32) (me.b * clientRect.width)
        case BlueVert:
            me.yCur = cast(s32) (me.b * clientRect.height)
        }
    }

    mtd impl onPaintEvent(evt: *PaintEvent)
    {
        let bc         = evt.bc
        let painter    = bc.painter
        var clientRect = me.getClientRect()
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
                var c  = Color.fromHsl(me.hue, me.sat, me.lum)
                var c1 = c
                c1.a = 0
                var c2 = c
                c2.a = 255
                me.img.fillGradient2(c1, c2)
            }
            elif me.mode == .AlphaVert
            {
                var c  = Color.fromHsl(me.hue, me.sat, me.lum)
                var c1 = c
                c1.a = 255
                var c2 = c
                c2.a = 0
                me.img.fillGradient4(c1, c1, c2, c2)
            }
            elif me.mode == .RedHorz
            {
                var c  = Color.fromRgbf(me.r, me.g, me.b)
                var c1 = c
                c1.r = 0
                var c2 = c
                c2.r = 255
                me.img.fillGradient2(c1, c2)
            }
            elif me.mode == .RedVert
            {
                var c  = Color.fromRgbf(me.r, me.g, me.b)
                var c1 = c
                c1.r = 255
                var c2 = c
                c2.r = 0
                me.img.fillGradient4(c1, c1, c2, c2)
            }
            elif me.mode == .GreenHorz
            {
                var c  = Color.fromRgbf(me.r, me.g, me.b)
                var c1 = c
                c1.g = 0
                var c2 = c
                c2.g = 255
                me.img.fillGradient2(c1, c2)
            }
            elif me.mode == .GreenVert
            {
                var c  = Color.fromRgbf(me.r, me.g, me.b)
                var c1 = c
                c1.g = 255
                var c2 = c
                c2.g = 0
                me.img.fillGradient4(c1, c1, c2, c2)
            }
            elif me.mode == .BlueHorz
            {
                var c  = Color.fromRgbf(me.r, me.g, me.b)
                var c1 = c
                c1.b = 0
                var c2 = c
                c2.b = 255
                me.img.fillGradient2(c1, c2)
            }
            elif me.mode == .BlueVert
            {
                var c  = Color.fromRgbf(me.r, me.g, me.b)
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
        var c = Color.fromHsl(me.hue, me.sat, me.lum)
        (me.r, me.g, me.b) = c.toRgbf()
    }

    internal mtd computeHsl()
    {
        var c = Color.fromRgbf(me.r, me.g, me.b)
        (me.hue, me.sat, me.lum) = c.toHsl()
    }

    mtd impl onPostPaintEvent(evt: *PaintEvent)
    {
        let bc         = evt.bc
        let painter    = bc.painter
        let theme      = me.getTheme()
        let rects      = me.getThemeRects()
        var clientRect = me.getClientRect()
        let metrics    = me.getThemeMetrics()
        let colors     = me.getThemeColors()

        #[Swag.Complete]
        switch me.mode
        {
        case SaturationLightness, HueSaturation, HueLightness, GreenBlue, RedBlue, RedGreen:
            let size = metrics.colorPicker_RoundPick
            var rect = Math.Rectangle{me.xCur - size, me.yCur - size, 2 * size, 2 * size}
            theme.drawSubRect(painter, rect, &rects.colorPicker_RoundPick, colors.colorPicker_Mark)

        case RedVert, GreenVert, BlueVert, AlphaVert, HueVert, LightnessVert, SaturationVert:
            let size = metrics.colorPicker_SquarePick
            var rect = Math.Rectangle{clientRect.horzCenter() - size, me.yCur - size, 2 * size, 2 * size}
            theme.drawSubRect(painter, rect, &rects.colorPicker_SquarePick, colors.colorPicker_Mark)

        case RedHorz, GreenHorz, BlueHorz, AlphaHorz, HueHorz, LightnessHorz, SaturationHorz:
            let size = metrics.colorPicker_SquarePick
            var rect = Math.Rectangle{me.xCur - size, clientRect.vertCenter() - size, 2 * size, 2 * size}
            theme.drawSubRect(painter, rect, &rects.colorPicker_SquarePick, colors.colorPicker_Mark)
        }
    }

    mtd impl onMouseEvent(evt: *MouseEvent)
    {
        var pos        = me.surfaceToLocal(evt.surfacePos)
        var clientRect = me.getClientRect()
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

                #[Swag.Complete]
                switch me.mode
                {
                case SaturationLightness:
                    me.sat = Math.clamp(x, 0, 1)
                    let l1 = Math.lerp(1, 0.5, me.sat)
                    me.lum = Math.lerp(l1, 0, y)

                case HueSaturation:
                    me.hue = x
                    me.sat = 1 - y
                    me.computeRGB()
                case HueLightness:
                    me.hue = x
                    me.lum = 1 - y
                    me.computeRGB()
                case GreenBlue:
                    me.g = x
                    me.b = y
                    me.computeHsl()
                case RedBlue:
                    me.r = x
                    me.b = y
                    me.computeHsl()
                case RedGreen:
                    me.r = x
                    me.g = y
                    me.computeHsl()
                case HueHorz:
                    me.hue = x
                    me.computeRGB()
                case HueVert:
                    me.hue = y
                    me.computeRGB()
                case LightnessHorz:
                    me.lum = x
                    me.computeRGB()
                case LightnessVert:
                    me.lum = y
                    me.computeRGB()
                case SaturationHorz:
                    me.sat = x
                    me.computeRGB()
                case SaturationVert:
                    me.sat = y
                    me.computeRGB()
                case AlphaHorz:
                    me.a = x
                case AlphaVert:
                    me.a = y
                case RedHorz:
                    me.r = x
                    me.computeHsl()
                case RedVert:
                    me.r = y
                    me.computeHsl()
                case GreenHorz:
                    me.g = x
                    me.computeHsl()
                case GreenVert:
                    me.g = y
                    me.computeHsl()
                case BlueHorz:
                    me.b = x
                    me.computeHsl()
                case BlueVert:
                    me.b = y
                    me.computeHsl()
                }

                me.computeXY()
                me.sigChanged.call(me)
                me.invalidate()
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

        if #me.a >= 0 and me.a != #me.a
        {
            me.a    = #me.a
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
        (me.hue, me.sat, me.lum) = col.toHsl()
        (me.a, me.r, me.g, me.b) = col.toArgbf()
        me.dirty = true
        me.invalidate()
        me.sigChanged.call(me)
    }

    // Returns the selected color
    mtd getColor()->Color
    {
        return Color.fromArgbf(me.a, me.r, me.g, me.b)
    }

    // Set the color picker box mode
    mtd setMode(mode: ColorPickerMode)
    {
        me.mode  = #me.mode
        me.dirty = true
        me.invalidate()
    }
}
