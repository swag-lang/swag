#global public
using Core, Pixel

alias RichEditStyleRef = u8

struct RichEditRuneStyle
{
    colBk:        Color
    colFg:        Color
    fontMode      = FontFamilyStyle.Regular
    underline     = UnderlineStyle.None
}
