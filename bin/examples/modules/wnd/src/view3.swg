using Core, Gui, Pixel

func createView3(tabView: *Tab, view: *Wnd)
{
    //{
        sb0 := ColorPickerBox.create(view, @{10, 10, 200, 200})
        sb0.setMode(.SaturationLightness)
        sb0.setColor(Argb.Blue)

        sb1 := ColorPickerBox.create(view, @{240, 10, 20, 200})
        sb1.setMode(.HueVert)
        sb1.sigChanged += @|sb0|(t) {
            sb0.changeHsl(h: t.hue)
        }
        sb1.setColor(Argb.Green)
    //}

    {
        sb := ColorPickerBox.create(view, @{300, 10, 20, 200})
        sb.setMode(.LightnessVert)
        sb.setColor(Argb.Orange)
    }

    {
        sb := ColorPickerBox.create(view, @{360, 10, 20, 200})
        sb.setMode(.SaturationVert)
        sb.setColor(Argb.Purple)
    }

    {
        sb := PalettePicker.create(view, @{420, 10, 240, 200})
        sb.addHueColors(8, 1.0, 0.5)
        sb.addHueColors(8, 1.0, 0.25)
        sb.addHueColors(8, 0.25, 0.5)
        sb.addLumColors(8, Argb.Red, 1.0)
        sb.addSatColors(8, Argb.Green)
        sb.addColor(Argb.Black)
        sb.addColor(Argb.White)
        sb.sigChanged += @|sb0|(t: *PalettePicker) {
            sb0.setColor(t.selectedColor)
        }
    }
}