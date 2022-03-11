#global public
using Core, Pixel

enum ColorPickerMode
{
    SaturationLightness
    HueVert
    LightnessVert
    SaturationVert
}

struct ColorPicker
{
    using wnd:      Wnd
    hue, sat, lum:  f32
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
            yCur = cast(s32) ((1 - hue) * clientRect.height)

        case LightnessVert:
            yCur = cast(s32) ((1 - lum) * clientRect.height)

        case SaturationVert:
            yCur = cast(s32) ((1 - sat) * clientRect.height)
        }
    }

    mtd onPaintEvent(evt: *PaintEvent)
    {
        bc := evt.bc
        painter := bc.painter
        clientRect := getClientRect()

        // Reconstruct image
        if dirty
        {
            dirty = false
            if !img.pixels
                img = Image.create(cast(s32) clientRect.width, cast(s32) clientRect.height, .RGBA8)

            var modeFill: Image.FillHslType = ?

            #[Swag.Complete]
            switch mode
            {
            case SaturationLightness:   modeFill = .SaturationLightness
            case HueVert:               modeFill = .HueVert
            case LightnessVert:         modeFill = .LightnessVert
            case SaturationVert:        modeFill = .SaturationVert
            }

            img.fillHsl(modeFill, Color.fromHsl(hue, sat, lum))

            if texture.valid
                getApp().renderer.updateTexture(texture, img.pixels)
            else
                texture = getApp().renderer.addImage(img)
            computeXY()
        }

        painter.drawTexture(clientRect, texture)
    }

    mtd onPostPaintEvent(evt: *PaintEvent)
    {
        bc := evt.bc
        painter     := bc.painter
        theme       := getTheme()
        rects       := getThemeRects()
        clientRect  := getClientRect()

        #[Swag.Complete]
        switch mode
        {
        case SaturationLightness:
            size := getThemeMetrics().colorPicker_RoundPick
            rect := Math.Rectangle{xCur-size, yCur-size, 2*size, 2*size}
            theme.drawSubRect(painter, rect, &rects.colorPicker_RoundPick, Argb.Black)
            rect.inflate(1, 1)
            theme.drawSubRect(painter, rect, &rects.colorPicker_RoundPick, Argb.White)

        case HueVert, LightnessVert, SaturationVert:
            size := getThemeMetrics().colorPicker_SquarePick
            rect := Math.Rectangle{-size, yCur-size, clientRect.width+2*size, 2*size}
            theme.drawSubRect(painter, rect, &rects.colorPicker_SquarePick, Argb.Black)
            rect.inflate(1, 1)
            theme.drawSubRect(painter, rect, &rects.colorPicker_SquarePick, Argb.White)
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
                case HueVert:
                    hue = h
                case LightnessVert:
                    lum = l
                case SaturationVert:
                    sat = s
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
        dirty = true
        invalidate()
        sigChanged.call(self)
    }

    // Returns the selected color
    mtd getColor()->Color
    {
        return Color.fromHsl(hue, sat, lum)
    }

    // Set the base color, and repaint
    mtd setMode(mode: ColorPickerMode)
    {
        self.mode = #self.mode
        dirty = true
        invalidate()
    }
}