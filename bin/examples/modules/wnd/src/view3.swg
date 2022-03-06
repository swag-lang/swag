using Core, Gui, Pixel

func createView3(tabView: *Tab, view: *Wnd)
{
    {
        sb := ColorPickerBox.create(view, @{10, 10, 200, 200})
        sb.setMode(.SaturationLightness)
        sb.setColor(Argb.Blue)
    }

    {
        sb := ColorPickerBox.create(view, @{240, 10, 30, 200})
        sb.setMode(.HueVert)
        sb.setColor(Argb.Green)
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