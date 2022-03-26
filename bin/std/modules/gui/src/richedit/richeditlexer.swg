public interface IRichEditLexer
{
    setup:      func(self, *RichEdit)
    insertRune: func(self, *RichEdit, rune)->bool
    compute:    func(self, *RichEditLine, *RichEditLine, *RichEditLine)
}