using Core, Gui, Pixel

func createView3(_tabView: *Tab, view: *Wnd)
{
    {
        sb0 := ColorPicker.create(view, {10, 10, 200, 200})
        sb0.setMode(.SaturationLightness)
        sb0.setColor(Argb.Blue)

        sb1 := ColorPicker.create(view, {240, 10, 20, 200})
        sb1.setMode(.HueVert)
        sb1.sigChanged += closure|sb0|(t) {
            sb0.change(h = t.hue)
        }
        sb1.setColor(Argb.Green)

        sb := PalettePicker.create(view, {420, 10, 240, 200})
        sb.addHueColors(8, 1.0, 0.5)
        sb.addHueColors(8, 1.0, 0.25)
        sb.addHueColors(8, 0.25, 0.5)
        sb.addLumColors(8, Argb.Red)
        sb.addSatColors(8, Argb.Green)
        sb.addColor(Argb.Black)
        sb.addColor(Argb.White)
        sb.sigChanged += closure|sb0|(t) {
            sb0.setColor(t.selectedColor)
        }
    }

    {
        sb := ColorPicker.create(view, {300, 10, 20, 200})
        sb.setMode(.LightnessVert)
        sb.setColor(Argb.Orange)

        sb1 := ColorPicker.create(view, {330, 10, 20, 200})
        sb1.setMode(.SaturationVert)
        sb1.setColor(Argb.Purple)

        sb2 := ColorPicker.create(view, {360, 10, 20, 200})
        sb2.setMode(.AlphaVert)
        sb2.setColor(Argb.Purple)
    }

    {
        sb := ColorPicker.create(view, {10, 250, 200, 20})
        sb.setMode(.HueHorz)
        sb1 := ColorPicker.create(view, {10, 280, 200, 20})
        sb1.setMode(.LightnessHorz)
        sb2 := ColorPicker.create(view, {10, 310, 200, 20})
        sb2.setMode(.SaturationHorz)
        sb3 := ColorPicker.create(view, {10, 340, 200, 20})
        sb3.setMode(.AlphaHorz)
    }

    {
        pv := ColorPickerCtrl.create(view, {10, 450})
        pv.setFrameForm(.Round)
        pv.setColor(Argb.Purple)
    }
}