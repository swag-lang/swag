#global public
using Core, Pixel

protected const DefaultImageName  = "widgets.png"
protected const DefaultImageBytes = #load(DefaultImageName)

struct Theme
{
    widgetsImg:         Texture
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
    protected mtd setup(renderer: *RenderOgl)
    {
        img := assume Image.decode(DefaultImageName, DefaultImageBytes)
        widgetsImg = renderer.addImage(img)
    }

    func drawTexture(painter: *Painter, texture: Texture, dstRect, srcRect: Math.Rectangle, cornerX, cornerY: s32, color: Color)
    {
        if !cornerX and !cornerY
        {
            painter.drawTexture(dstRect, srcRect, texture, color)
            return
        }

        loop i: 9
        {
        }
    }
}