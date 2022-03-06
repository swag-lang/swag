#global public
using Core, Pixel

struct ColorPickerBox
{
    using wnd:      Wnd
    hue, sat, lum:  f32
    mode            = Image.FillHslType.SaturationLightness

    dirty:      bool = true
    isMoving:   bool
    img:        Image
    texture:    Texture
    xCur, yCur: s32
}

impl IWnd for ColorPickerBox
{
    mtd computeXY()
    {
        clientRect := getClientRect()
        switch mode
        {
        case SaturationLightness:
            xCur = cast(s32) (sat * clientRect.width)
            l1 := Math.lerp(1, 0.5, sat)
            l2 := lum / l1
            yCur = cast(s32) (l2 * clientRect.height)
        case HueVert:
            yCur = cast(s32) (hue * clientRect.height)
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
            img.fillHsl(mode, Color.fromHsl(hue, sat, lum))
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
        painter := bc.painter
        clientRect := getClientRect()

        switch mode
        {
        case SaturationLightness:
            rect := Math.Rectangle{xCur-4, yCur-4, 8, 8}
            painter.drawRect(rect, Argb.White, 1)
            rect.inflate(-1, -1)
            painter.drawRect(rect, Argb.Black, 1)
        case HueVert:
            rect := Math.Rectangle{-4, yCur-4, clientRect.width+8, 8}
            painter.drawRect(rect, Argb.White, 1)
            rect.inflate(-1, -1)
            painter.drawRect(rect, Argb.Black, 1)
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
                switch mode
                {
                case SaturationLightness:
                    sat = s
                    lum = l
                case HueVert:
                    hue = h
                }

                computeXY()
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

impl ColorPickerBox
{
    #[Swag.Discardable]
    func create(parent: *Wnd, position: Math.Rectangle, id: string = null)->*ColorPickerBox
    {
        Debug.assert(parent != null)
        ret := Wnd.create'ColorPickerBox(parent, null, position, id)
        ret.wndFlags |= .PostChildsPaint
        ret.setColor(Argb.Red)
        return ret
    }

    // Set the base color, and repaint
    mtd setColor(col: Color)
    {
        (hue, sat, lum) = col.toHsl()
        dirty = true
        invalidate()
    }

    // Set the base color, and repaint
    mtd setMode(mode: Image.FillHslType)
    {
        self.mode = #self.mode
        dirty = true
        invalidate()
    }
}