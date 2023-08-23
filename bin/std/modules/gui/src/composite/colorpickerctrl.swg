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

struct ColorPickerCtrl
{
    using frameWnd: FrameWnd
    sigChanged:     SigArray'closure(*Self)
    oldColor:       Color
    color:          Color
    oldColorDone:   bool
    mode            = ColorPickerViewMode.SaturationLightness
    staticRes:      *Wnd
    pickBox:        *ColorPicker
    pickBar:        *ColorPicker
    pickAlpha:      *ColorPicker
    editR:          *EditBox
    editG:          *EditBox
    editB:          *EditBox
    editA:          *EditBox
    editH:          *EditBox
    radioH:         *RadioButton
    radioL:         *RadioButton
    radioS:         *RadioButton
    radioR:         *RadioButton
    radioG:         *RadioButton
    radioB:         *RadioButton
}

impl IWnd for ColorPickerCtrl
{
    mtd impl onPaintEvent(evt: *PaintEvent)
    {
        frameWnd.IWnd.onPaintEvent(evt)

        var rect = staticRes.position
        let painter = evt.bc.painter

        var brush = Brush.createHatch(.CheckBoardLarge, getThemeColors().colorPicker_BackAlpha)

        if oldColor.a != 255
            painter.fillRect(rect, brush)
        painter.fillRect(rect, oldColor)

        rect.x += rect.width
        if color.a != 255
            painter.fillRect(rect, brush)
        painter.fillRect(rect, color)
    }
}

internal impl ColorPickerCtrl
{
    mtd updateMode()
    {
        radioH.setChecked(mode == .SaturationLightness)
        radioL.setChecked(mode == .HueSaturation)
        radioS.setChecked(mode == .HueLightness)
        radioR.setChecked(mode == .GreenBlue)
        radioG.setChecked(mode == .RedBlue)
        radioB.setChecked(mode == .RedGreen)
    }

    mtd updatePickers()
    {
        pickBox.sigChanged.disable()
        pickBox.setColor(color)
        pickBox.sigChanged.enable()

        pickBar.sigChanged.disable()
        pickBar.setColor(color)
        pickBar.sigChanged.enable()

        pickAlpha.sigChanged.disable()
        pickAlpha.setColor(color)
        pickAlpha.sigChanged.enable()
    }

    mtd updateEdits()
    {
        var t = Format.toString("%", color.r)
        editR.setText(t)
        t = Format.toString("%", color.g)
        editG.setText(t)
        t = Format.toString("%", color.b)
        editB.setText(t)
        t = Format.toString("%", color.a)
        editA.setText(t)
        t = Format.toString("%{X08}", color.argb)
        editH.setText(t)

        staticRes.invalidate()
        sigChanged.call(self)
    }

    mtd updateAll()
    {
        updatePickers()
        updateEdits()
        updateMode()
    }
}

impl ColorPickerCtrl
{
    #[Swag.Discardable]
    func create(parent: *Wnd, position: Math.Rectangle, id: WndId = null)->*ColorPickerCtrl
    {
        var pos = position
        if pos.width == 0
            pos.width = 340
        if pos.height == 0
            pos.height = 330
        let v = Wnd.create'ColorPickerCtrl(parent, "", pos, id)

        with v
        {
            .pickBox   = ColorPicker.create(v, {10, 10, 260, 185})
            .pickBar   = ColorPicker.create(v, {10, 215, 220, 16})
            .pickAlpha = ColorPicker.create(v, {10, 240, 220, 16})

            .staticRes = Wnd.create'Wnd(v, "", {245, 215, 40, 40})
            let picker = IconButton.create(v, v.getTheme().getIcon24(.Picker), {245, 215, 40, 40})

            .editR = EditBox.create(v, "", {10, 265, 50})
            .editG = EditBox.create(v, "", {65, 265, 50})
            .editB = EditBox.create(v, "", {120, 265, 50})
            .editA = EditBox.create(v, "", {185, 265, 50})
            .editH = EditBox.create(v, "", {240, 265, 90})

            var l = Label.create(v, "R", {.editR.position.x, .editR.position.bottom() + 0, .editR.position.width})
            l.labelFlags |= .LightText
            l = Label.create(v, "G", {.editG.position.x, .editG.position.bottom() + 0, .editG.position.width})
            l.labelFlags |= .LightText
            l = Label.create(v, "B", {.editB.position.x, .editB.position.bottom() + 0, .editB.position.width})
            l.labelFlags |= .LightText
            l = Label.create(v, "A", {.editA.position.x, .editA.position.bottom() + 0, .editA.position.width})
            l.labelFlags |= .LightText
            l = Label.create(v, "HEX", {.editH.position.x, .editH.position.bottom() + 0, .editH.position.width})
            l.labelFlags |= .LightText

            .radioH = RadioButton.create(v, "H", {280, 10, 40})
            .radioS = RadioButton.create(v, "S", {280, 40, 40})
            .radioL = RadioButton.create(v, "L", {280, 70, 40})
            .radioR = RadioButton.create(v, "R", {280, 100, 40})
            .radioG = RadioButton.create(v, "G", {280, 130, 40})
            .radioB = RadioButton.create(v, "B", {280, 160, 40})

            picker.sigPressed += closure|v|(cb) {
                var app = cb.getApp()
                app.pushCursor(Cursor.from(.Cross))

                var save = v.getColor()
                var sfade = app.fadeDisabledSurface
                app.fadeDisabledSurface = false
                discard app.doModalLoop(null, closure|v,save|(app) {
                    if app.mouse.isButtonPressed(.Right)
                    {
                        v.setColor(save)
                        app.exitModal()
                        return
                    }

                    var col = Application.pickColorAtMouse()
                    v.setColor(col)

                    if app.mouse.isButtonPressed(.Left)
                        app.exitModal()
                })

                app.fadeDisabledSurface = sfade
                app.popCursor()
            }

            .pickBox.sigChanged += closure|v|(cb) {
                switch v.mode
                {
                case SaturationLightness:
                    var c = Color.fromHsl(v.pickBar.hue, cb.sat, cb.lum)
                    v.color.setArgb(v.color.a, c.r, c.g, c.b)
                    v.pickAlpha.change(s: cb.sat, l: cb.lum)
                case HueSaturation:
                    var c = Color.fromHsl(cb.hue, cb.sat, v.pickBar.lum)
                    v.color.setArgb(v.color.a, c.r, c.g, c.b)
                    v.pickBar.change(h: cb.hue, s: cb.sat)
                    v.pickAlpha.change(h: cb.hue, s: cb.sat)
                case HueLightness:
                    var c = Color.fromHsl(cb.hue, v.pickBar.sat, cb.lum)
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

            .pickBar.sigChanged += closure|v|(cb) {
                switch v.mode
                {
                case SaturationLightness:
                    var c = Color.fromHsl(v.pickBar.hue, v.pickBox.sat, v.pickBox.lum)
                    v.color.setArgb(v.color.a, c.r, c.g, c.b)
                    v.pickBox.change(h: cb.hue)
                    v.pickAlpha.change(h: cb.hue)
                case HueSaturation:
                    var c = Color.fromHsl(v.pickBox.hue, v.pickBox.sat, v.pickBar.lum)
                    v.color.setArgb(v.color.a, c.r, c.g, c.b)
                    v.pickBox.change(l: cb.lum)
                    v.pickAlpha.change(l: cb.lum)
                case HueLightness:
                    var c = Color.fromHsl(v.pickBox.hue, v.pickBar.sat, v.pickBox.lum)
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
            .pickAlpha.sigChanged += closure|v|(cb) {
                v.color.a = cast(u8) (cb.a * 255)
                v.updateEdits()
            }

            .editR.setInputU64(0, 255)
            .editR.maxLength = 3
            .editR.sigLoseFocus += closure|v|(ed) {
                var res = catch StrConv.parseU32(ed.text)
                v.color.r = acast Math.clamp(res.value, 0, 255)
                v.updateAll()
            }

            .editG.setInputU64(0, 255)
            .editG.maxLength = 3
            .editG.sigLoseFocus += closure|v|(ed) {
                var res = catch StrConv.parseU32(ed.text)
                v.color.g = acast Math.clamp(res.value, 0, 255)
                v.updateAll()
            }

            .editB.setInputU64(0, 255)
            .editB.maxLength = 3
            .editB.sigLoseFocus += closure|v|(ed) {
                var res = catch StrConv.parseU32(ed.text)
                v.color.b = acast Math.clamp(res.value, 0, 255)
                v.updateAll()
            }

            .editA.setInputU64(0, 255)
            .editA.maxLength = 3
            .editA.sigLoseFocus += closure|v|(ed) {
                var res = catch StrConv.parseU32(ed.text)
                v.color.a = acast Math.clamp(res.value, 0, 255)
                v.pickAlpha.setColor(v.color)
                v.updateEdits()
            }

            .editH.sigLoseFocus += closure|v|(ed) {
                var res = catch StrConv.parseX64(ed.text)
                v.color.argb = cast(u32) res.value
                v.pickAlpha.setColor(v.color)
                v.updateAll()
            }

            .radioH.sigChanged += closure|v|(bt) {
                v.setMode(.SaturationLightness)
                v.updateAll()
            }
            .radioS.sigChanged += closure|v|(bt) {
                v.setMode(.HueLightness)
                v.updateAll()
            }
            .radioL.sigChanged += closure|v|(bt) {
                v.setMode(.HueSaturation)
                v.updateAll()
            }

            .radioR.sigChanged += closure|v|(bt) {
                v.setMode(.GreenBlue)
                v.updateAll()
            }
            .radioG.sigChanged += closure|v|(bt) {
                v.setMode(.RedBlue)
                v.updateAll()
            }
            .radioB.sigChanged += closure|v|(bt) {
                v.setMode(.RedGreen)
                v.updateAll()
            }
        }

        v.setMode(v.mode)
        return v
    }

    // Change the display mode
    mtd setMode(mode: ColorPickerViewMode)
    {
        self.mode = #self.mode
        switch self.mode
        {
        case SaturationLightness:
            pickBox.setMode(.SaturationLightness)
            pickBar.setMode(.HueHorz)
        case HueSaturation:
            pickBox.setMode(.HueSaturation)
            pickBar.setMode(.LightnessHorz)
        case HueLightness:
            pickBox.setMode(.HueLightness)
            pickBar.setMode(.SaturationHorz)
        case GreenBlue:
            pickBox.setMode(.GreenBlue)
            pickBar.setMode(.RedHorz)
        case RedBlue:
            pickBox.setMode(.RedBlue)
            pickBar.setMode(.GreenHorz)
        case RedGreen:
            pickBox.setMode(.RedGreen)
            pickBar.setMode(.BlueHorz)
        }
    }

    // Returns the selected color
    #[Swag.Inline]
    mtd getColor()->Color
    {
        return color
    }

    // Set the selected color
    mtd setColor(color: Color)
    {
        if self.color == #self.color
            return

        if !oldColorDone
        {
            oldColor = #self.color
            oldColorDone = true
        }

        self.color = #self.color
        sigChanged.call(self)

        updateAll()
    }
}