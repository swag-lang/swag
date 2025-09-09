public interface IRichEditLexer
{
    mtd setup(ed: *RichEditCtrl);
    mtd setupTheme(ed: *RichEditCtrl);
    mtd insertRune(ed: *RichEditCtrl, ch: rune)->bool;
    mtd compute(linePrev: #null *RichEditLine, line: *RichEditLine, lineNext: #null *RichEditLine);
}
