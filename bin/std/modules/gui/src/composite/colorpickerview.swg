#global public
using Core, Pixel

struct ColorPickerView
{
    using frameWnd: FrameWnd
    color:          Color
    pickBox:        *ColorPicker
    pickHue:        *ColorPicker
    pickAlpha:      *ColorPicker
    editRed:        *EditBox
    editGreen:      *EditBox
    editBlue:       *EditBox
    editAlpha:      *EditBox
}

private impl ColorPickerView
{
    mtd updateDatas()
    {
        t := Format.toString("%", color.r)
        editRed.setText(t)
        t = Format.toString("%", color.g)
        editGreen.setText(t)
        t = Format.toString("%", color.b)
        editBlue.setText(t)
        t = Format.toString("%", color.a)
        editAlpha.setText(t)
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
                c := Color.fromHsl(v.pickHue.hue, cb.sat, cb.lum)
                v.color.r = c.r
                v.color.g = c.g
                v.color.b = c.b
                v.pickAlpha.changeHsl(s: cb.sat, l: cb.lum)
                v.updateDatas()
            }

            pickHue = ColorPicker.create(v, @{10, 215, 200, 16})
            pickHue.setMode(.HueHorz)
            pickHue.sigChanged += @|v|(cb) {
                c := Color.fromHsl(v.pickHue.hue, v.pickBox.sat, v.pickBox.lum)
                v.color.r = c.r
                v.color.g = c.g
                v.color.b = c.b
                v.pickBox.changeHsl(h: cb.hue)
                v.pickAlpha.changeHsl(h: cb.hue)
                v.updateDatas()
            }

            pickAlpha = ColorPicker.create(v, @{10, 240, 200, 16})
            pickAlpha.setMode(.AlphaHorz)
            pickAlpha.sigChanged += @|v|(cb) {
                v.color.a = cast(u8) (cb.alpha * 255)
                v.updateDatas()
            }

            editRed = EditBox.create(v, "", @{240, 10, 50})
            editRed.setInputU64(0, 255)
            editRed.maxLength = 3
            editRed.sigLoseFocus += @|v|(ed) {
                res := catch StrConv.parseU32(ed.text)
                v.color.r = acast Math.clamp(res.value, 0, 255)
                v.pickBox.setColorInternal(v.color)
                v.pickHue.setColorInternal(v.color)
                v.pickAlpha.setColorInternal(v.color)
                v.updateDatas()
            }

            editGreen = EditBox.create(v, "", @{240, 45, 50})
            editGreen.setInputU64(0, 255)
            editGreen.maxLength = 3
            editGreen.sigLoseFocus += @|v|(ed) {
                res := catch StrConv.parseU32(ed.text)
                v.color.g = acast Math.clamp(res.value, 0, 255)
                v.pickBox.setColorInternal(v.color)
                v.pickHue.setColorInternal(v.color)
                v.pickAlpha.setColorInternal(v.color)
                v.updateDatas()
            }

            editBlue = EditBox.create(v, "", @{240, 80, 50})
            editBlue.setInputU64(0, 255)
            editBlue.maxLength = 3
            editBlue.sigLoseFocus += @|v|(ed) {
                res := catch StrConv.parseU32(ed.text)
                v.color.b = acast Math.clamp(res.value, 0, 255)
                v.pickBox.setColorInternal(v.color)
                v.pickHue.setColorInternal(v.color)
                v.pickAlpha.setColorInternal(v.color)
                v.updateDatas()
            }

            editAlpha = EditBox.create(v, "", @{240, 115, 50})
            editAlpha.setInputU64(0, 255)
            editAlpha.maxLength = 3
            editAlpha.sigLoseFocus += @|v|(ed) {
                res := catch StrConv.parseU32(ed.text)
                v.color.a = acast Math.clamp(res.value, 0, 255)
                v.pickAlpha.setColorInternal(v.color)
                v.updateDatas()
            }
        }

        return v
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
        self.color = #self.color
        pickBox.setColorInternal(self.color)
        pickHue.setColorInternal(self.color)
        pickAlpha.setColorInternal(self.color)
        updateDatas()
    }
}