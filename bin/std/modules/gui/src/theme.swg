#global public
using Core, Pixel

protected const DefaultImage = #load("widgets.png")

struct Theme
{
    widgetsImg:         Image
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

impl Theme
{
    mtd setup()
    {
        widgetsImg = assume Image.decode("widgets.png", DefaultImage)
    }
}