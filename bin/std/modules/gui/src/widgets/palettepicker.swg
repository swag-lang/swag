#global public
using Core, Pixel

struct PalettePicker
{
    using wnd:         Wnd
    selectedColor:     Color
    pal:               Array'Color
    palPos:            Array'Math.Rectangle
    boxSize:           s32 = 24
    boxMargin:         s32 = 3
    sigChanged:        SigArray'func||(*PalettePicker)
    hotIdx:            s32 = Swag.S32.Max
}

impl IWnd for PalettePicker
{
    mtd impl onPaintEvent(evt: *PaintEvent)
    {
        let bc      = evt.bc
        let painter = bc.painter
        let colors  = .getThemeColors()

        var colorBorder: Color = undefined

        let clientRect = .getClientRectPadding()
        .palPos.resize(.pal.count)

        var x = clientRect.x
        var y = clientRect.y
        foreach c, i in .pal
        {
            if .hotIdx == i do
                colorBorder = colors.palPicker_HotBorder
            else do
                colorBorder = colors.palPicker_Border

            var posBox = Math.Rectangle{x, y, .boxSize, .boxSize}
            .palPos[i] = posBox

            painter.setParams(.Antialiased)
            posBox.inflate(-2)
            if c.a != 255 do
                painter.fillRoundRect(posBox, 5, 5, Brush.createHatch(.CheckBoardLarge, colors.colorPicker_BackAlpha))
            painter.fillRoundRect(posBox, 5, 5, c)
            posBox.inflate(1)
            painter.drawRoundRect(posBox, 5, 5, colorBorder, 2)

            x += .boxSize + .boxMargin
            if x + .boxSize > clientRect.right()
            {
                x = clientRect.x
                y += .boxSize + .boxMargin
            }
        }
    }

    mtd impl onMouseEvent(evt: *MouseEvent)
    {
        let pos = .surfaceToLocal(evt.surfacePos)
        evt.accepted = false

        // Hit test
        var newHotIdx = Swag.S32.Max
        for i in .palPos
        {
            if .palPos[i].contains(pos)
            {
                newHotIdx = cast() i
                break
            }
        }

        if newHotIdx != .hotIdx
        {
            .hotIdx = newHotIdx
            .invalidate()
        }

        switch evt.kind
        {
        case MousePressed:
            if evt.button == .Left and .hotIdx != Swag.S32.Max
            {
                .selectedColor = .pal[.hotIdx]
                .sigChanged.call(me)
                evt.accepted = true
            }

        case MouseLeave:
            if .hotIdx != Swag.S32.Max
            {
                .hotIdx = Swag.S32.Max
                .invalidate()
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
        for i in numColors
        {
            var c: Color
            c.setHsl(i / cast(f32) numColors, sat, lum)
            c.a = 255
            .pal.add(c)
        }

        .invalidate()
    }

    // Add a range of changing 'luminance'
    mtd addLumColors(numColors: s32, base: Color)
    {
        let (h, s, ?) = base.toHsl()

        for i in numColors
        {
            var c: Color
            c.setHsl(h, s, i / cast(f32) numColors)
            c.a = 255
            .pal.add(c)
        }

        .invalidate()
    }

    // Add a range of changing 'saturation'
    mtd addSatColors(numColors: s32, base: Color, lum: f32 = 0.5)
    {
        let (h, ?, ?) = base.toHsl()

        for i in numColors
        {
            var c: Color
            c.setHsl(h, i / cast(f32) numColors, lum)
            c.a = 255
            .pal.add(c)
        }

        .invalidate()
    }

    // Add a color
    mtd addColor(c: Color)
    {
        .pal.add(c)
        .invalidate()
    }
}
