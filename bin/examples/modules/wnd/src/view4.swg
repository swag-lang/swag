using Core, Gui, Pixel

const Txt = #load("view0.swg")

func createView4(tabView: *Tab, view: *Wnd)
{
    {
        ed := RichEdit.create(view, @{10, 10, 500, 500})
        ed.setFrameForm(.Round)
        ed.richEdFlags |= .ShowFocusSel
        ed.setText(cast(string) Txt)
    }

    {
        ed := RichEdit.create(view, @{520, 10, 300, 300})
        ed.styles[1].colFg = Argb.Red
        ed.styles[2].colBk = Argb.Blue
        ed.styles[3].fontMode = .Regular
        ed.styles[3].underline = .Underline
        ed.addText("coucou la foule\n", 0)
        ed.addText("comment ça va ? ", 1)
        ed.addText("ça va !\n", 2)
        ed.addText("et même très bien", 3)
    }
}