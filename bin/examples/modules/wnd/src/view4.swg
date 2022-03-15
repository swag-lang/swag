using Core, Gui, Pixel

func createView4(tabView: *Tab, view: *Wnd)
{
    ed := RichEdit.create(view, @{})
    ed.setFrameForm(.Round)
    ed.dockStyle = .Center
    ed.setText("coucou Ϣ la foule ψψψ\nother line")
}