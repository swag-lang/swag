#global public
using Core, Pixel

enum ColorPickerViewMode
{
    SaturationLightness
    HueSaturation
    HueLightness
    GreenBlue
    RedBlue
    RedGreen
}

#[Swag.EnumFlags]
enum ColorPickerFlags
{
    Zero
    Alpha
    All   = Alpha
}

struct ColorPickerCtrl
{
    using frameWnd:     FrameWnd

    sigChanged:         SigArray'func||(*ColorPickerCtrl)
    oldColor:           Color
    color:              Color
    pickerFlags         = ColorPickerFlags.All

    oldColorDone:       bool
    mode                = ColorPickerViewMode.SaturationLightness
    staticRes:          *Wnd
    pickBox:            *ColorPicker
    pickBar:            *ColorPicker
    pickAlpha:          *ColorPicker
    editA:              *EditBox
    editR:              *EditBox
    editG:              *EditBox
    editB:              *EditBox
    editH:              *EditBox
    radioH:             *RadioButton
    radioL:             *RadioButton
    radioS:             *RadioButton
    radioR:             *RadioButton
    radioG:             *RadioButton
    radioB:             *RadioButton
}

impl IWnd for ColorPickerCtrl
{
    mtd impl onPaintEvent(evt: *PaintEvent)
    {
        .frameWnd.IWnd.onPaintEvent(evt)

        var rect    = .staticRes.position
        let painter = evt.bc.painter

        let brush = Brush.createHatch(.CheckBoardLarge, .getThemeColors().colorPicker_BackAlpha)

        if .oldColor.a != 255 do
            painter.fillRect(rect, brush)
        painter.fillRect(rect, .oldColor)

        rect.x += rect.width
        if .color.a != 255 do
            painter.fillRect(rect, brush)
        painter.fillRect(rect, .color)
    }
}

internal impl ColorPickerCtrl
{
    mtd updateMode()
    {
        .radioH.setChecked(.mode == .SaturationLightness)
        .radioL.setChecked(.mode == .HueSaturation)
        .radioS.setChecked(.mode == .HueLightness)
        .radioR.setChecked(.mode == .GreenBlue)
        .radioG.setChecked(.mode == .RedBlue)
        .radioB.setChecked(.mode == .RedGreen)
    }

    mtd updatePickers()
    {
        .pickBox.sigChanged.disable()
        .pickBox.setColor(.color)
        .pickBox.sigChanged.enable()

        .pickBar.sigChanged.disable()
        .pickBar.setColor(.color)
        .pickBar.sigChanged.enable()

        .pickAlpha.sigChanged.disable()
        .pickAlpha.setColor(.color)
        .pickAlpha.sigChanged.enable()
    }

    mtd updateEdits()
    {
        var t = Format.toString("%", .color.r)
        .editR.setText(t)
        t = Format.toString("%", .color.g)
        .editG.setText(t)
        t = Format.toString("%", .color.b)
        .editB.setText(t)
        t = Format.toString("%", .color.a)
        .editA.setText(t)

        if .pickerFlags.has(.Alpha) do
            t = Format.toString("%{X08}", .color.argb)
        else do
            t = Format.toString("%{X02}%{X02}%{X02}", .color.r, .color.g, .color.b)
        .editH.setText(t)

        .staticRes.invalidate()
        .sigChanged.call(me)
    }

    mtd updateAll()
    {
        .updatePickers()
        .updateEdits()
        .updateMode()
    }
}

impl ColorPickerCtrl
{
    #[Swag.Discardable]
    func create(parent: *Wnd, position: Math.Rectangle, flags = ColorPickerFlags.All, id: WndId = null)->*ColorPickerCtrl
    {
        var pos = position
        if pos.width == 0 do
            pos.width = 340
        if pos.height == 0 do
            pos.height = 330

        with let v = Wnd.create'ColorPickerCtrl(parent, "", pos, id)
        {
            .pickerFlags = flags

            .pickBox   = ColorPicker.create(v, {10, 10, 260, 185})
            .pickBar   = ColorPicker.create(v, {10, 215, 220, 16})
            .pickAlpha = ColorPicker.create(v, {10, 240, 220, 16})

            .staticRes = Wnd.create'Wnd(v, "", {245, 215, 40, 40})
            var icon = v.getTheme().getIcon24(.Picker)
            icon.color = Argb.White
            let picker = IconButton.create(v, icon, {245, 215, 40, 40})

            .editA = EditBox.create(v, "", {10, 265, 50})
            .editR = EditBox.create(v, "", {70, 265, 50})
            .editG = EditBox.create(v, "", {125, 265, 50})
            .editB = EditBox.create(v, "", {180, 265, 50})
            .editH = EditBox.create(v, "", {240, 265, 90})

            var l = Label.create(v, "A", {.editA.position.x, .editA.position.bottom() + 0, .editA.position.width})
            l.labelFlags |= .LightText
            l = Label.create(v, "R", {.editR.position.x, .editR.position.bottom() + 0, .editR.position.width})
            l.labelFlags |= .LightText
            l = Label.create(v, "G", {.editG.position.x, .editG.position.bottom() + 0, .editG.position.width})
            l.labelFlags |= .LightText
            l = Label.create(v, "B", {.editB.position.x, .editB.position.bottom() + 0, .editB.position.width})
            l.labelFlags |= .LightText

            l = Label.create(v, flags.has(.Alpha) ? "ARGB" : "RGB", {.editH.position.x, .editH.position.bottom() + 0, .editH.position.width})
            l.labelFlags |= .LightText

            .radioH = RadioButton.create(v, "H", {280, 10, 40})
            .radioS = RadioButton.create(v, "S", {280, 40, 40})
            .radioL = RadioButton.create(v, "L", {280, 70, 40})
            .radioR = RadioButton.create(v, "R", {280, 100, 40})
            .radioG = RadioButton.create(v, "G", {280, 130, 40})
            .radioB = RadioButton.create(v, "B", {280, 160, 40})

            picker.sigPressed += func|v|(cb)
            {
                var app   = cb.getApp()
                var save  = v.getColor()
                var sfade = app.fadeDisabledSurface
                app.fadeDisabledSurface = false
                discard app.doModalLoop(null, func|v, save|(app)
                {
                    app.setFrameCursor(Cursor.from(.Cross))

                    if app.mouse.isButtonPressed(.Right)
                    {
                        v.setColor(save)
                        app.exitModal()
                        return
                    }

                    var col = Application.pickColorAtMouse()
                    v.setColor(col)

                    if app.mouse.isButtonPressed(.Left) do
                        app.exitModal()
                })

                app.fadeDisabledSurface = sfade
            }

            .pickBox.sigChanged += func|v|(cb)
            {
                if !v.pickerFlags.has(.Alpha) do
                    v.color.a = 255
                switch v.mode
                {
                case SaturationLightness:
                    let c = Color.fromHsl(v.pickBar.hue, cb.sat, cb.lum)
                    v.color.setArgb(v.color.a, c.r, c.g, c.b)
                    v.pickAlpha.change(s: cb.sat, l: cb.lum)
                case HueSaturation:
                    let c = Color.fromHsl(cb.hue, cb.sat, v.pickBar.lum)
                    v.color.setArgb(v.color.a, c.r, c.g, c.b)
                    v.pickBar.change(h: cb.hue, s: cb.sat)
                    v.pickAlpha.change(h: cb.hue, s: cb.sat)
                case HueLightness:
                    let c = Color.fromHsl(cb.hue, v.pickBar.sat, cb.lum)
                    v.color.setArgb(v.color.a, c.r, c.g, c.b)
                    v.pickBar.change(h: cb.hue, l: cb.lum)
                    v.pickAlpha.change(h: cb.hue, l: cb.lum)
                case GreenBlue:
                    v.color.g = cast(u8) (cb.g * 255)
                    v.color.b = cast(u8) (cb.b * 255)
                    v.updatePickers()
                case RedBlue:
                    v.color.r = cast(u8) (cb.r * 255)
                    v.color.b = cast(u8) (cb.b * 255)
                    v.updatePickers()
                case RedGreen:
                    v.color.r = cast(u8) (cb.r * 255)
                    v.color.g = cast(u8) (cb.g * 255)
                    v.updatePickers()
                }

                v.updateEdits()
            }

            .pickBar.sigChanged += func|v|(cb)
            {
                switch v.mode
                {
                case SaturationLightness:
                    let c = Color.fromHsl(v.pickBar.hue, v.pickBox.sat, v.pickBox.lum)
                    v.color.setArgb(v.color.a, c.r, c.g, c.b)
                    v.pickBox.change(h: cb.hue)
                    v.pickAlpha.change(h: cb.hue)
                case HueSaturation:
                    let c = Color.fromHsl(v.pickBox.hue, v.pickBox.sat, v.pickBar.lum)
                    v.color.setArgb(v.color.a, c.r, c.g, c.b)
                    v.pickBox.change(l: cb.lum)
                    v.pickAlpha.change(l: cb.lum)
                case HueLightness:
                    let c = Color.fromHsl(v.pickBox.hue, v.pickBar.sat, v.pickBox.lum)
                    v.color.setArgb(v.color.a, c.r, c.g, c.b)
                    v.pickBox.change(s: cb.sat)
                    v.pickAlpha.change(s: cb.sat)
                case GreenBlue:
                    v.color.r = cast(u8) (cb.r * 255)
                    v.updatePickers()
                case RedBlue:
                    v.color.g = cast(u8) (cb.g * 255)
                    v.updatePickers()
                case RedGreen:
                    v.color.b = cast(u8) (cb.b * 255)
                    v.updatePickers()
                }
                v.updateEdits()
            }

            .pickAlpha.setMode(.AlphaHorz)
            .pickAlpha.sigChanged += func|v|(cb)
            {
                v.color.a = cast(u8) (cb.a * 255)
                v.updateEdits()
            }

            .editR.setInputU64(0, 255)
            .editR.maxLength = 3
            .editR.sigLoseFocus += func|v|(ed)
            {
                var res = catch StrConv.parseU32(ed.text)
                v.color.r = cast() Math.clamp(res.value, 0, 255)
                v.updateAll()
            }

            .editG.setInputU64(0, 255)
            .editG.maxLength = 3
            .editG.sigLoseFocus += func|v|(ed)
            {
                var res = catch StrConv.parseU32(ed.text)
                v.color.g = cast() Math.clamp(res.value, 0, 255)
                v.updateAll()
            }

            .editB.setInputU64(0, 255)
            .editB.maxLength = 3
            .editB.sigLoseFocus += func|v|(ed)
            {
                var res = catch StrConv.parseU32(ed.text)
                v.color.b = cast() Math.clamp(res.value, 0, 255)
                v.updateAll()
            }

            .editA.setInputU64(0, 255)
            .editA.maxLength = 3
            .editA.sigLoseFocus += func|v|(ed)
            {
                var res = catch StrConv.parseU32(ed.text)
                v.color.a = cast() Math.clamp(res.value, 0, 255)
                v.pickAlpha.setColor(v.color)
                v.updateEdits()
            }

            .editH.setInputX64(0, 0xFFFFFFFF)
            .editH.maxLength = 8
            .editH.sigLoseFocus += func|v|(ed)
            {
                var res = catch StrConv.parseX64(ed.text)
                if !v.pickerFlags.has(.Alpha) do
                    res.value |= 0xFF000000
                v.color.argb = cast(u32) res.value
                v.pickAlpha.setColor(v.color)
                v.updateAll()
            }

            .radioH.sigChanged += func|v|(bt) { v.setMode(.SaturationLightness); v.updateAll(); }
            .radioS.sigChanged += func|v|(bt) { v.setMode(.HueLightness); v.updateAll(); }
            .radioL.sigChanged += func|v|(bt) { v.setMode(.HueSaturation); v.updateAll(); }
            .radioR.sigChanged += func|v|(bt) { v.setMode(.GreenBlue); v.updateAll(); }
            .radioG.sigChanged += func|v|(bt) { v.setMode(.RedBlue); v.updateAll(); }
            .radioB.sigChanged += func|v|(bt) { v.setMode(.RedGreen); v.updateAll(); }

            if !.pickerFlags.has(.Alpha)
            {
                .editA.enable(false)
                .pickAlpha.hide()
                .pickBar.position.height = 40
                .editH.setInputX64(0, 0x00FFFFFF)
                .editH.maxLength = 6
            }
        }

        v.setMode(v.mode)
        return v
    }

    // Creates a color picker with a palette on the top.
    // 'pal' should contain the palette colors. Maximum is 9 colors.
    // If less colors are provided, then default colors will be added instead (so if you keep 'pal' empty, all defaults colors will be added).
    #[Swag.Discardable]
    func createWithPalette(parent: *Wnd, position: Math.Rectangle, pal: Array'Color, flags = ColorPickerFlags.All, id: WndId = null)->*ColorPickerCtrl
    {
        let picker = ColorPickerCtrl.create(parent, position, flags, id)

        // Setup palette
        with let palette = PalettePicker.create(parent, {16, 16, picker.position.width - 16, 48})
        {
            for i in 9
            {
                if i >= pal.count
                {
                    switch i
                    {
                    case 0:
                        .addColor(Argb.White)
                    case 1:
                        .addColor(Argb.Gray)
                    case 2:
                        .addColor(Argb.Black)
                    case 3:
                        .addColor(Argb.Red)
                    case 4:
                        .addColor(Argb.Green)
                    case 5:
                        .addColor(Argb.Blue)
                    case 6:
                        .addColor(Argb.Yellow)
                    case 7:
                        .addColor(Argb.Purple)
                    case 8:
                        if flags.has(.Alpha) do
                            .addColor(Color.fromArgb(0))
                        else do
                            .addColor(Argb.Gold)
                    }
                }
                else
                {
                    palette.addColor(pal[i])
                }
            }

            .boxSize = 32
            .sigChanged += func|picker|(pal)
            {
                if !pal.selectedColor.a or !picker.color.a do
                    picker.setColor(pal.selectedColor)
                else do
                    picker.setColorKeepAlpha(pal.selectedColor)
            }
        }

        return picker
    }

    // Change the display mode
    mtd setMode(mode: ColorPickerViewMode)
    {
        .mode = mode
        switch .mode
        {
        case SaturationLightness:
            .pickBox.setMode(.SaturationLightness)
            .pickBar.setMode(.HueHorz)
        case HueSaturation:
            .pickBox.setMode(.HueSaturation)
            .pickBar.setMode(.LightnessHorz)
        case HueLightness:
            .pickBox.setMode(.HueLightness)
            .pickBar.setMode(.SaturationHorz)
        case GreenBlue:
            .pickBox.setMode(.GreenBlue)
            .pickBar.setMode(.RedHorz)
        case RedBlue:
            .pickBox.setMode(.RedBlue)
            .pickBar.setMode(.GreenHorz)
        case RedGreen:
            .pickBox.setMode(.RedGreen)
            .pickBar.setMode(.BlueHorz)
        }
    }

    // Returns the selected color
    #[Swag.Inline]
    mtd getColor()->Color
    {
        return .color
    }

    // Set the selected color but does not change alpha
    mtd setColorKeepAlpha(color: Color)
    {
        var col = color
        col.a = .color.a
        .setColor(col)
    }

    // Set the selected color
    mtd setColor(color: Color)
    {
        var col = color
        if !.pickerFlags.has(.Alpha) do
            col.a = 255

        if !.oldColorDone
        {
            .oldColor     = col
            .oldColorDone = true
        }
        elif .color == col
        {
            return
        }

        .color = col
        .sigChanged.call(me)

        .updateAll()
    }
}
