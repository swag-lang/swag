#global public
using Core, Pixel

struct RichEditCharStyle
{
    colBk:      Color
    colFg:      Color
    fontMode    = FontFamilyStyle.Regular
    underline   = UnderlineStyle.None
}

impl RichEditCharStyle
{
    mtd opEquals(other: Self)->bool
    {
        return  colBk == other.colBk and
                colFg == other.colFg and
                fontMode == other.fontMode and
                underline == other.underline
    }
}