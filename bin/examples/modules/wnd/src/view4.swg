using Core, Gui, Pixel

const Txt = #load("view0.swg")

func createView4(tabView: *Tab, view: *Wnd)
{
    /*{
        ed := RichEdit.create(view, @{10, 10, 600, 500})
        ed.setFrameForm(.Round)
        ed.setRichEdFlags(.ShowFocusSel | .CurLineBorder)
        ed.setText(cast(string) Txt)
    }*/

    {
        ed := RichEdit.create(view, @{620, 10, 400, 300})
        ed.setFrameForm(.Round)
        ed.setFrameFlags(.FocusBorder)
        ed.style.addStyleSheetColors("richEdit_Bk $edit_Bk")

        ed.styles[1].colFg = Argb.Red
        ed.styles[2].colBk = Argb.Blue
        ed.styles[3].fontMode = .Regular
        ed.styles[3].underline = .Underline
        ed.addText("coucou la foule\n", 0)
        ed.addText("comment ça va ? ", 1)
        ed.addText("ça va !\n", 2)
        ed.addText("et même très bien", 3)
    }

    /*{
        ed := RichEdit.create(view, @{620, 320, 400, 300})
        ed.setFrameForm(.Round)
        ed.setFrameFlags(.FocusBorder)
        ed.setRichEdFlags(.ReadOnly)
        ed.style.addStyleSheetColors("richEdit_Bk $edit_Bk")

        ed.styles[1].colFg = Argb.Red
        ed.styles[2].colBk = Argb.Blue
        ed.styles[3].fontMode = .Regular
        ed.styles[3].underline = .Underline
        ed.addText("coucou la foule\n", 0)
        ed.addText("comment ça va ? ", 1)
        ed.addText("ça va !\n", 2)
        ed.addText("et même très bien", 3)
    }*/
}