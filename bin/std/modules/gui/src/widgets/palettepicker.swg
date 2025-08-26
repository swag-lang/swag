#global marked
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
        let colors  = me.getThemeColors()

        var colorBorder: Color = undefined

        var clientRect = me.getClientRectPadding()
        me.palPos.resize(me.pal.count)

        var x = clientRect.x
        var y = clientRect.y
        foreach c, i in me.pal
        {
            if me.hotIdx == i:
                colorBorder = colors.palPicker_HotBorder
            else:
                colorBorder = colors.palPicker_Border

            var posBox = Math.Rectangle{x, y, me.boxSize, me.boxSize}
            me.palPos[i] = posBox

            painter.setParams(.Antialiased)
            posBox.inflate(-2)
            if c.a != 255:
                painter.fillRoundRect(posBox, 5, 5, Brush.createHatch(.CheckBoardLarge, colors.colorPicker_BackAlpha))
            painter.fillRoundRect(posBox, 5, 5, c)
            posBox.inflate(1)
            painter.drawRoundRect(posBox, 5, 5, colorBorder, 2)

            x += me.boxSize + me.boxMargin
            if x + me.boxSize > clientRect.right()
            {
                x = clientRect.x
                y += me.boxSize + me.boxMargin
            }
        }
    }

    mtd impl onMouseEvent(evt: *MouseEvent)
    {
        var pos = me.surfaceToLocal(evt.surfacePos)
        evt.accepted = false

        // Hit test
        var newHotIdx = Swag.S32.Max
        for i in me.palPos
        {
            if me.palPos[i].contains(pos)
            {
                newHotIdx = cast() i
                break
            }
        }

        if newHotIdx != me.hotIdx
        {
            me.hotIdx = newHotIdx
            me.invalidate()
        }

        switch evt.kind
        {
        case MousePressed:
            if evt.button == .Left and me.hotIdx != Swag.S32.Max
            {
                me.selectedColor = me.pal[me.hotIdx]
                me.sigChanged.call(me)
                evt.accepted = true
            }

        case MouseLeave:
            if me.hotIdx != Swag.S32.Max
            {
                me.hotIdx = Swag.S32.Max
                me.invalidate()
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
            me.pal.add(c)
        }

        me.invalidate()
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
            me.pal.add(c)
        }

        me.invalidate()
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
            me.pal.add(c)
        }

        me.invalidate()
    }

    // Add a color
    mtd addColor(c: Color)
    {
        me.pal.add(c)
        me.invalidate()
    }
}
