public interface IRichEditLexer
{
    setup:      func(self, *RichEditCtrl)
    insertRune: func(self, *RichEditCtrl, rune)->bool
    compute:    func(self, *RichEditLine, *RichEditLine, *RichEditLine)
}