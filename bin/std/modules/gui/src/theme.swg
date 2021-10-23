#global public
using Pixel

struct Theme
{
    colorWndBackground: Color = 0xFFEEEEEE
}

struct WndColor
{
    themeColor: *Color
    color:      Color
}

impl WndColor
{
    mtd getColor()->Color
    {
        return themeColor ? dref themeColor : color
    }
}