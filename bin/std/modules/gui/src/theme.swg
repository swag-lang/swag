#global public
using Core, Pixel

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
    mtd load(themePath: string) throw
    {
        path := Path.combine(themePath, "widgets.png")
        widgetsImg = try Image.load(path)
    }
}