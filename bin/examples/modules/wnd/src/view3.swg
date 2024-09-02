using Core, Gui, Pixel

func createView3(_tabView: *Tab, view: *Wnd)
{
    {
        let sb0 = ColorPicker.create(view, {10, 10, 200, 200})
        sb0.setMode(.SaturationLightness)
        sb0.setColor(Argb.Blue)

        let sb1 = ColorPicker.create(view, {240, 10, 20, 200})
        sb1.setMode(.HueVert)
        sb1.sigChanged += func|sb0|(t)
        {
            sb0.change(h: t.hue)
        }
        sb1.setColor(Argb.Green)

        let sb = PalettePicker.create(view, {420, 10, 240, 200})
        sb.addHueColors(8, 1.0, 0.5)
        sb.addHueColors(8, 1.0, 0.25)
        sb.addHueColors(8, 0.25, 0.5)
        sb.addLumColors(8, Argb.Red)
        sb.addSatColors(8, Argb.Green)
        sb.addColor(Argb.Black)
        sb.addColor(Argb.White)
        sb.sigChanged += func|sb0|(t)
        {
            sb0.setColor(t.selectedColor)
        }
    }

    {
        let sb = ColorPicker.create(view, {300, 10, 20, 200})
        sb.setMode(.LightnessVert)
        sb.setColor(Argb.Orange)

        let sb1 = ColorPicker.create(view, {330, 10, 20, 200})
        sb1.setMode(.SaturationVert)
        sb1.setColor(Argb.Purple)

        let sb2 = ColorPicker.create(view, {360, 10, 20, 200})
        sb2.setMode(.AlphaVert)
        sb2.setColor(Argb.Purple)
    }

    {
        let sb = ColorPicker.create(view, {10, 250, 200, 20})
        sb.setMode(.HueHorz)
        let sb1 = ColorPicker.create(view, {10, 280, 200, 20})
        sb1.setMode(.LightnessHorz)
        let sb2 = ColorPicker.create(view, {10, 310, 200, 20})
        sb2.setMode(.SaturationHorz)
        let sb3 = ColorPicker.create(view, {10, 340, 200, 20})
        sb3.setMode(.AlphaHorz)
    }

    {
        let pv = ColorPickerCtrl.create(view, {10, 450})
        pv.setFrameForm(.Round)
        pv.setColor(Argb.Purple)
    }

    {
        let pv = ColorPickerCtrl.create(view, {410, 450}, .All & ~.Alpha)
        pv.setFrameForm(.Round)
        pv.setColor(Argb.Purple)
    }
}
