using Core, Gui, Pixel

func createView3(tabView: *Tab, view: *Wnd)
{
    {
        sb0 := ColorPickerBox.create(view, @{10, 10, 200, 200})
        sb0.setMode(.SaturationLightness)
        sb0.setColor(Argb.Blue)

        sb1 := ColorPickerBox.create(view, @{240, 10, 30, 200})
        sb1.setMode(.HueVert)
        sb1.setColor(Argb.Green)
        sb1.sigChanged += @|sb0|(t) {
            sb0.hue = t.hue
            sb0.dirty = true
            sb0.invalidate()
        }
    }

    {
        sb := ColorPickerBox.create(view, @{300, 10, 30, 200})
        sb.setMode(.LightnessVert)
        sb.setColor(Argb.Orange)
    }

    {
        sb := ColorPickerBox.create(view, @{360, 10, 30, 200})
        sb.setMode(.SaturationVert)
        sb.setColor(Argb.Purple)
    }
}