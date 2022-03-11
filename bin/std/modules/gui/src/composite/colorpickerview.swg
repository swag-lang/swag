#global public
using Core, Pixel

struct ColorPickerView
{
    using frameWnd: FrameWnd
    pickBox:        *ColorPicker
    pickHue:        *ColorPicker
    pickAlpha:      *ColorPicker
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
            }

            pickHue = ColorPicker.create(v, @{10, 215, 200, 16})
            pickHue.setMode(.HueHorz)
            pickHue.sigChanged += @|v|(cb) {
                v.pickBox.changeHsl(h: cb.hue)
                v.pickAlpha.changeHsl(h: cb.hue)
            }

            pickAlpha = ColorPicker.create(v, @{10, 240, 200, 16})
            pickAlpha.setMode(.AlphaHorz)
        }

        return v
    }

    // Set the selected color
    mtd setColor(color: Color)
    {
        pickBox.setColor(color)
        pickHue.setColor(color)
        pickAlpha.setColor(color)
    }
}