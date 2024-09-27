using Core, Gui, Pixel

const Txt  = #include("view0.swg")
const Txt1 = #include("view4.swg")

var g_LexerSwag: RichEditLexerSwag

func createView4(_tabView: *Tab, view: *Wnd)
{
    {
        let ed = RichEditCtrl.create(view, {10, 10, 600, 500})
        with ed
        {
            .setFrameForm(.Round)
            .setRichEdFlags(.ShowFocusSel | .CurLineBorder | .TabToSpaces | .WordWrap)
            .setLexer(cast(IRichEditLexer) g_LexerSwag)
            .setText(cast(string) Txt)
            .marginLeft  = 20
            .marginRight = 40
        }
    }

    {
        let ed = RichEditCtrl.create(view, {10, 520, 600, 300})
        with ed
        {
            .setFrameForm(.Round)
            .setRichEdFlags(.ShowFocusSel | .CurLineBorder | .DrawBlanks | .TabToSpaces)
            .setLexer(cast(IRichEditLexer) g_LexerSwag)
            .setText(cast(string) Txt1)
        }
    }

    {
        let ed = RichEditCtrl.create(view, {620, 10, 400, 300})
        with ed
        {
            .setFrameForm(.Round)
            .setFrameFlags(.FocusBorder)
            .setRichEdFlags(.WordWrap)
            .setForm(.Edit)

            .styles[1].colFg     = Argb.Red
            .styles[2].colBk     = Argb.Blue
            .styles[3].fontMode  = .Regular
            .styles[3].underline = .Underline
            .addText("RichEditCtrl coucou la foule ", 0)
            .addText("comment ça va ? ", 1)
            .addText("ça va !", 2)
            .addText(" et même très bien ", 3)
            .addText("coucou la foule ", 0)
            .addText("comment ça va ? ", 1)
            .addText("ça va !", 2)
            .addText(" et même très bien ", 3)
        }
    }

    {
        let ed = RichEditCtrl.create(view, {620, 320, 400, 150})
        with ed
        {
            .setForm(.Edit)
            .setFrameForm(.Round)
            .setFrameFlags(.FocusBorder)
            .setRichEdFlags(.ReadOnly)
            .setText("read only\n")
        }
    }
}
