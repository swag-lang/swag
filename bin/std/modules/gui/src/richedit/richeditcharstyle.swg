#global public
using Core, Pixel

typealias RichEditStyleRef = u8

struct RichEditRuneStyle
{
    colBk:      Color
    colFg:      Color
    fontMode    = FontFamilyStyle.Regular
    underline   = UnderlineStyle.None
}

impl RichEditRuneStyle
{
    mtd opEquals(other: Self)->bool
    {
        return  colBk == other.colBk and
                colFg == other.colFg and
                fontMode == other.fontMode and
                underline == other.underline
    }
}