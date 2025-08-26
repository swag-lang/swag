#global marked
public interface IRichEditLexer
{
    func setup(me, ed: *RichEditCtrl);
    func insertRune(me, ed: *RichEditCtrl, ch: rune)->bool;
    func compute(me, linePrev: #null *RichEditLine, line: *RichEditLine, lineNext: #null *RichEditLine);
}
