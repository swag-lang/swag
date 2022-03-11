#global public
using Core, Pixel

struct ColorPickerView
{
    using frameWnd: FrameWnd
    pickBox:        *ColorPicker
    pickHue:        *ColorPicker
    pickAlpha:      *ColorPicker
    editRed:        *EditBox
    editGreen:      *EditBox
    editBlue:       *EditBox
}

private impl ColorPickerView
{
    mtd updateColor()
    {
        c := getColor()
        t := Format.toString("%", c.r)
        editRed.setText(t)
        t = Format.toString("%", c.g)
        editGreen.setText(t)
        t = Format.toString("%", c.b)
        editBlue.setText(t)
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
                v.pickAlpha.changeHsl(s: cb.sat, l: cb.lum)
                v.updateColor()
            }

            pickHue = ColorPicker.create(v, @{10, 215, 200, 16})
            pickHue.setMode(.HueHorz)
            pickHue.sigChanged += @|v|(cb) {
                v.pickBox.changeHsl(h: cb.hue)
                v.pickAlpha.changeHsl(h: cb.hue)
                v.updateColor()
            }

            pickAlpha = ColorPicker.create(v, @{10, 240, 200, 16})
            pickAlpha.setMode(.AlphaHorz)
            pickAlpha.sigChanged += @|v|(cb) {
                v.updateColor()
            }

            editRed = EditBox.create(v, "", @{240, 10, 50})
            editRed.setInputU64(0, 255)
            editRed.maxLength = 3
            editRed.sigLoseFocus += @|v|(ed) {
                res := catch StrConv.parseU32(ed.text)
                c := v.getColor()
                c.r = acast Math.clamp(res.value, 0, 255)
                v.setColor(c)
            }

            editGreen = EditBox.create(v, "", @{240, 45, 50})
            editGreen.setInputU64(0, 255)
            editGreen.maxLength = 3
            editGreen.sigLoseFocus += @|v|(ed) {
                res := catch StrConv.parseU32(ed.text)
                c := v.getColor()
                c.g = acast Math.clamp(res.value, 0, 255)
                v.setColor(c)
            }

            editBlue = EditBox.create(v, "", @{240, 80, 50})
            editBlue.setInputU64(0, 255)
            editBlue.maxLength = 3
            editBlue.sigLoseFocus += @|v|(ed) {
                res := catch StrConv.parseU32(ed.text)
                c := v.getColor()
                c.b = acast Math.clamp(res.value, 0, 255)
                v.setColor(c)
            }
        }

        return v
    }

    mtd getColor()->Color
    {
        c := Color.fromHsl(pickHue.hue, pickBox.sat, pickBox.lum)
        c.a = cast(u8) (pickAlpha.alpha * 255)
        return c
    }

    // Set the selected color
    mtd setColor(color: Color)
    {
        pickBox.setColor(color)
        pickHue.setColor(color)
        pickAlpha.setColor(color)
    }
}