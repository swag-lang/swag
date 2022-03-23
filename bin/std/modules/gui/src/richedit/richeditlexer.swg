public interface IRichEditLexer
{
    setup:          func(self, *RichEdit)
    setStyleRef:    func(self, RichEditStyleRef)
    getStyleRef:    func(self)->RichEditStyleRef
    compute:        func(self, *RichEditLine)
}