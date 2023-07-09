#global public
using Core, Pixel

struct PalettePicker
{
    using wnd:      Wnd
    selectedColor:  Color
    pal:            Array'Color
    palPos:         Array'Math.Rectangle
    boxSize:        s32 = 24
    boxMargin:      s32 = 3
    sigChanged:     SigArray'closure(*Self)
    hotIdx:         s32 = Swag.S32.Max
}

impl IWnd for PalettePicker
{
    mtd impl onPaintEvent(evt: *PaintEvent)
    {
        let bc = evt.bc
        let painter = bc.painter
        let colors = getThemeColors()

        var colorBorder: Color = undefined

        var clientRect = getClientRectPadding()
        palPos.resize(pal.count)

        var x = clientRect.x
        var y = clientRect.y
        visit c, i: pal
        {
            if hotIdx == i
                colorBorder = colors.palPicker_HotBorder
            else
                colorBorder = colors.palPicker_Border

            var posBox = Math.Rectangle{x, y, boxSize, boxSize}
            palPos[i] = posBox

            painter.setParams(.Antialiased)
            posBox.inflate(-2)
            if c.a != 255
                painter.fillRoundRect(posBox, 5, 5, Brush.createHatch(.CheckBoardLarge, colors.colorPicker_BackAlpha))
            painter.fillRoundRect(posBox, 5, 5, c)
            posBox.inflate(1)
            painter.drawRoundRect(posBox, 5, 5, colorBorder, 2)

            x += boxSize + boxMargin
            if x + boxSize > clientRect.right()
            {
                x = clientRect.x
                y += boxSize + boxMargin
            }
        }

    }

    mtd impl onMouseEvent(evt: *MouseEvent)
    {
        var pos = surfaceToLocal(evt.surfacePos)
        evt.accepted = false

        // Hit test
        var newHotIdx = Swag.S32.Max
        loop i: palPos
        {
            if palPos[i].contains(pos)
            {
                newHotIdx = acast i
                break
            }
        }

        if newHotIdx != hotIdx
        {
            hotIdx = newHotIdx
            invalidate()
        }

        switch evt.kind
        {
        case MousePressed:
            if evt.button == .Left and hotIdx != Swag.S32.Max
            {
                selectedColor = pal[hotIdx]
                sigChanged.call(self)
                evt.accepted = true
            }

        case MouseLeave:
            if hotIdx != Swag.S32.Max
            {
                hotIdx = Swag.S32.Max
                invalidate()
            }
        }
    }
}

impl PalettePicker
{
    #[Swag.Discardable]
    func create(parent: *Wnd, position: Math.Rectangle = {}, id: WndId = null)->*PalettePicker
    {
        Debug.assert(parent != null)
        let ret = Wnd.create'PalettePicker(parent, null, position, id)
        return ret
    }

    // Add a range of changing 'hue'
    mtd addHueColors(numColors: s32, sat, lum: f32)
    {
        loop i: numColors
        {
            var c: Color
            c.setHsl(i / cast(f32) numColors, sat, lum)
            c.a = 255
            pal.add(c)
        }

        invalidate()
    }

    // Add a range of changing 'luminance'
    mtd addLumColors(numColors: s32, base: Color)
    {
        var (h, s, l) = base.toHsl()

        loop i: numColors
        {
            var c: Color
            c.setHsl(h, s, i / cast(f32) numColors)
            c.a = 255
            pal.add(c)
        }

        invalidate()
    }

    // Add a range of changing 'saturation'
    mtd addSatColors(numColors: s32, base: Color, lum: f32 = 0.5)
    {
        var (h, s, l) = base.toHsl()

        loop i: numColors
        {
            var c: Color
            c.setHsl(h, i / cast(f32) numColors, lum)
            c.a = 255
            pal.add(c)
        }

        invalidate()
    }

    // Add a color
    mtd addColor(c: Color)
    {
        pal.add(c)
        invalidate()
    }
}