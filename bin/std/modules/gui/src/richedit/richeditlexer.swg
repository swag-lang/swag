public interface IRichEditLexer
{
    setup:      func(self, *RichEdit)
    setState:   func(self, s32)
    getState:   func(self)->s32
    compute:    func(self, *RichEditLine)
}