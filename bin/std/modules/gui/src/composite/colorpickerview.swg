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

struct ColorPickerView
{
    using frameWnd: FrameWnd
    oldColor:       Color
    color:          Color
    mode            = ColorPickerViewMode.SaturationLightness
    staticRes:      *Wnd
    pickBox:        *ColorPicker
    pickBar:        *ColorPicker
    pickAlpha:      *ColorPicker
    editRed:        *EditBox
    editGreen:      *EditBox
    editBlue:       *EditBox
    editAlpha:      *EditBox
    editHex:        *EditBox
    radioH:         *RadioButton
    radioL:         *RadioButton
    radioS:         *RadioButton
    radioR:         *RadioButton
    radioG:         *RadioButton
    radioB:         *RadioButton
}

impl IWnd for ColorPickerView
{
    mtd onPaintEvent(evt: *PaintEvent)
    {
        frameWnd.IWnd.onPaintEvent(evt)

        rect := staticRes.position
        painter := evt.bc.painter

        brush := Brush.createHatch(.CheckBoardLarge, getThemeColors().colorPicker_BackAlpha)

        if oldColor.a != 255 painter.fillRect(rect, brush)
        painter.fillRect(rect, oldColor)

        rect.x += rect.width
        if color.a != 255 painter.fillRect(rect, brush)
        painter.fillRect(rect, color)
    }
}

private impl ColorPickerView
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
        t := Format.toString("%", color.r)
        editRed.setText(t)
        t = Format.toString("%", color.g)
        editGreen.setText(t)
        t = Format.toString("%", color.b)
        editBlue.setText(t)
        t = Format.toString("%", color.a)
        editAlpha.setText(t)
        t = Format.toString("%{X08}", color.argb)
        editHex.setText(t)
    }

    mtd updateAll()
    {
        updatePickers()
        updateEdits()
        updateMode()
    }
}

impl ColorPickerView
{
    #[Swag.Discardable]
    func create(parent: *Wnd, position: Math.Rectangle, id: string = null)->*ColorPickerView
    {
        v := Wnd.create'ColorPickerView(parent, "", position, id)

        with v
        {
            pickBox = ColorPicker.create(v, @{10, 10, 200, 200})
            pickBox.sigChanged += @|v|(cb) {
                switch v.mode
                {
                case SaturationLightness:
                    c := Color.fromHsl(v.pickBar.hue, cb.sat, cb.lum)
                    v.color.setArgb(v.color.a, c.r, c.g, c.b)
                    v.pickAlpha.change(s: cb.sat, l: cb.lum)
                case HueSaturation:
                    c := Color.fromHsl(cb.hue, cb.sat, v.pickBar.lum)
                    v.color.setArgb(v.color.a, c.r, c.g, c.b)
                    v.pickBar.change(h: cb.hue, s: cb.sat)
                    v.pickAlpha.change(h: cb.hue, s: cb.sat)
                case HueLightness:
                    c := Color.fromHsl(cb.hue, v.pickBar.sat, cb.lum)
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

            pickBar = ColorPicker.create(v, @{10, 215, 200, 16})
            pickBar.sigChanged += @|v|(cb) {
                switch v.mode
                {
                case SaturationLightness:
                    c := Color.fromHsl(v.pickBar.hue, v.pickBox.sat, v.pickBox.lum)
                    v.color.setArgb(v.color.a, c.r, c.g, c.b)
                    v.pickBox.change(h: cb.hue)
                    v.pickAlpha.change(h: cb.hue)
                case HueSaturation:
                    c := Color.fromHsl(v.pickBox.hue, v.pickBox.sat, v.pickBar.lum)
                    v.color.setArgb(v.color.a, c.r, c.g, c.b)
                    v.pickBox.change(l: cb.lum)
                    v.pickAlpha.change(l: cb.lum)
                case HueLightness:
                    c := Color.fromHsl(v.pickBox.hue, v.pickBar.sat, v.pickBox.lum)
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

            pickAlpha = ColorPicker.create(v, @{10, 240, 200, 16})
            pickAlpha.setMode(.AlphaHorz)
            pickAlpha.sigChanged += @|v|(cb) {
                v.color.a = cast(u8) (cb.a * 255)
                v.updateEdits()
            }

            editRed = EditBox.create(v, "", @{315, 10, 50})
            editRed.setInputU64(0, 255)
            editRed.maxLength = 3
            editRed.sigLoseFocus += @|v|(ed) {
                res := catch StrConv.parseU32(ed.text)
                v.color.r = acast Math.clamp(res.value, 0, 255)
                v.updateAll()
            }

            editGreen = EditBox.create(v, "", @{315, 45, 50})
            editGreen.setInputU64(0, 255)
            editGreen.maxLength = 3
            editGreen.sigLoseFocus += @|v|(ed) {
                res := catch StrConv.parseU32(ed.text)
                v.color.g = acast Math.clamp(res.value, 0, 255)
                v.updateAll()
            }

            editBlue = EditBox.create(v, "", @{315, 80, 50})
            editBlue.setInputU64(0, 255)
            editBlue.maxLength = 3
            editBlue.sigLoseFocus += @|v|(ed) {
                res := catch StrConv.parseU32(ed.text)
                v.color.b = acast Math.clamp(res.value, 0, 255)
                v.updateAll()
            }

            editAlpha = EditBox.create(v, "", @{240, 115, 50})
            editAlpha.setInputU64(0, 255)
            editAlpha.maxLength = 3
            editAlpha.sigLoseFocus += @|v|(ed) {
                res := catch StrConv.parseU32(ed.text)
                v.color.a = acast Math.clamp(res.value, 0, 255)
                v.pickAlpha.setColor(v.color)
                v.updateEdits()
            }

            editHex = EditBox.create(v, "", @{240, 150, 90})
            editHex.sigLoseFocus += @|v|(ed) {
                res := catch StrConv.parseX64(ed.text)
                v.color.argb = cast(u32) res.value
                v.pickAlpha.setColor(v.color)
                v.updateAll()
            }

            staticRes = Wnd.create'Wnd(v, "", @{240, 190, 40, 40})

            radioH = RadioButton.create(v, "H", @{220, 10, 40})
            radioH.sigChanged += @|v|(bt) {
                v.setMode(.SaturationLightness)
                v.updateAll()
            }
            radioS = RadioButton.create(v, "S", @{220, 45, 40})
            radioS.sigChanged += @|v|(bt) {
                v.setMode(.HueLightness)
                v.updateAll()
            }
            radioL = RadioButton.create(v, "L", @{220, 80, 40})
            radioL.sigChanged += @|v|(bt) {
                v.setMode(.HueSaturation)
                v.updateAll()
            }

            radioR = RadioButton.create(v, "R", @{270, 10, 40})
            radioR.sigChanged += @|v|(bt) {
                v.setMode(.GreenBlue)
                v.updateAll()
            }
            radioG = RadioButton.create(v, "G", @{270, 45, 40})
            radioG.sigChanged += @|v|(bt) {
                v.setMode(.RedBlue)
                v.updateAll()
            }
            radioB = RadioButton.create(v, "B", @{270, 80, 40})
            radioB.sigChanged += @|v|(bt) {
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
    mtd getColor()->Color
    {
        return self.color
    }

    // Set the selected color
    mtd setColor(color: Color)
    {
        if self.color == #self.color
            return
        self.oldColor = #self.color
        self.color = #self.color
        updateAll()
    }
}