using Core, Gui, Pixel

const Txt = #load("view0.swg")

func createView4(tabView: *Tab, view: *Wnd)
{
    ed := RichEdit.create(view, @{})
    ed.setFrameForm(.Round)
    ed.dockStyle = .Center
    ed.setText(cast(string) Txt)
}