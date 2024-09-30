public interface IRichEditLexer
{
    func setup(self, ed: *RichEditCtrl);
    func insertRune(self, ed: *RichEditCtrl, ch: rune)->bool;
    func compute(self, linePrev: nl *RichEditLine, line: *RichEditLine, lineNext: nl *RichEditLine);
}
