public interface IRichEditLexer
{
    setup:      func(self, *RichEdit)
    compute:    func(self, *RichEditLine, *RichEditLine, *RichEditLine)
}