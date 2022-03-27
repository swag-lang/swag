using Core, Gui, Pixel

const Txt = #load("view0.swg")

var g_LexerSwag: RichEditLexerSwag

func createView4(tabView: *Tab, view: *Wnd)
{
    {
        ed := RichEdit.create(view, @{10, 10, 600, 500})
        ed.setFrameForm(.Round)
        ed.setRichEdFlags(.ShowFocusSel | .CurLineBorder | .TabToSpaces | .WordWrap)
        ed.setLexer(cast(IRichEditLexer) g_LexerSwag)
        ed.setText(cast(string) Txt)
        ed.marginLeft = 20
        ed.marginRight = 40
    }

    {
        ed := RichEdit.create(view, @{10, 520, 600, 300})
        ed.setFrameForm(.Round)
        ed.setRichEdFlags(.ShowFocusSel | .CurLineBorder | .DrawBlanks | .TabToSpaces)
        ed.setLexer(cast(IRichEditLexer) g_LexerSwag)
        ed.setText(cast(string) Txt)
        ed.dockStyle = .Bottom
    }

    {
        ed := RichEdit.create(view, @{620, 10, 400, 300})
        ed.setFrameForm(.Round)
        ed.setFrameFlags(.FocusBorder)
        ed.setRichEdFlags(.WordWrap)
        ed.style.addStyleSheetColors("richEdit_Bk $edit_Bk")

        ed.styles[1].colFg = Argb.Red
        ed.styles[2].colBk = Argb.Blue
        ed.styles[3].fontMode = .Regular
        ed.styles[3].underline = .Underline
        ed.addText("coucou la foule ", 0)
        ed.addText("comment ça va ? ", 1)
        ed.addText("ça va !", 2)
        ed.addText(" et même très bien ", 3)
        ed.addText("coucou la foule ", 0)
        ed.addText("comment ça va ? ", 1)
        ed.addText("ça va !", 2)
        ed.addText(" et même très bien ", 3)
    }

    {
        ed := RichEdit.create(view, @{620, 320, 400, 150})
        ed.setFrameForm(.Round)
        ed.setFrameFlags(.FocusBorder)
        ed.setRichEdFlags(.ReadOnly)
        ed.style.addStyleSheetColors("richEdit_Bk $edit_Bk")
        ed.setText("read only\n")
    }
}