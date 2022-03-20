using Core, Gui, Pixel

const Txt = #load("view0.swg")

func createView4(tabView: *Tab, view: *Wnd)
{
    ed := RichEdit.create(view, @{})
    ed.setFrameForm(.Round)
    ed.dockStyle = .Center

    ed.setText(cast(string) Txt)

    ed.clear()
    ed.styles[1].colFg = Argb.Red
    ed.styles[2].colBk = Argb.Blue
    ed.styles[3].fontMode = .Regular
    ed.styles[3].underline = .Underline
    ed.addText("coucou la foule\n", 0)
    ed.addText("comment ça va ? ", 1)
    ed.addText("ça va !\n", 2)
    ed.addText("et même très bien", 3)
}