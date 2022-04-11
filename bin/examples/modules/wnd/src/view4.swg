using Core, Gui, Pixel

const Txt = #load("view0.swg")
const Txt1 = #load("view4.swg")

var g_LexerSwag: RichEditLexerSwag

func createView4(tabView: *Tab, view: *Wnd)
{
    {
        ed := RichEdit.create(view, @{10, 10, 600, 500})
        with ed
        {
            setFrameForm(.Round)
            setRichEdFlags(.ShowFocusSel | .CurLineBorder | .TabToSpaces | .WordWrap)
            setLexer(cast(IRichEditLexer) g_LexerSwag)
            setText(cast(string) Txt)
            marginLeft = 20
            marginRight = 40
        }
    }

    {
        ed := RichEdit.create(view, @{10, 520, 600, 300})
        ed.setFrameForm(.Round)
        ed.setRichEdFlags(.ShowFocusSel | .CurLineBorder | .DrawBlanks | .TabToSpaces)
        ed.setLexer(cast(IRichEditLexer) g_LexerSwag)
        ed.setText(cast(string) Txt1)
    }

    {
        ed := RichEdit.create(view, @{620, 10, 400, 300})
        with ed
        {
            setFrameForm(.Round)
            setFrameFlags(.FocusBorder)
            setRichEdFlags(.WordWrap)
            setForm(.Edit)

            styles[1].colFg = Argb.Red
            styles[2].colBk = Argb.Blue
            styles[3].fontMode = .Regular
            styles[3].underline = .Underline
            addText("RichEdit coucou la foule ", 0)
            addText("comment ça va ? ", 1)
            addText("ça va !", 2)
            addText(" et même très bien ", 3)
            addText("coucou la foule ", 0)
            addText("comment ça va ? ", 1)
            addText("ça va !", 2)
            addText(" et même très bien ", 3)
        }
    }

    {
        ed := RichEdit.create(view, @{620, 320, 400, 150})
        ed.setForm(.Edit)
        ed.setFrameForm(.Round)
        ed.setFrameFlags(.FocusBorder)
        ed.setRichEdFlags(.ReadOnly)
        ed.setText("read only\n")
    }
}