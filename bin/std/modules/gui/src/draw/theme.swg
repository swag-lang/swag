#global public
using Core, Pixel

protected const DefaultImageName  = "widgets.png"
protected const DefaultImageBytes = #load(DefaultImageName)
protected const DefaultFontRegular = #load("segoe_regular.ttf")
protected const DefaultFontBold = #load("segoe_bold.ttf")
protected const DefaultFontBoldItalic = #load("segoe_bold_italic.ttf")
protected const DefaultFontItalic = #load("segoe_italic.ttf")

struct Theme
{
    widgetsImg:     Texture
    typeFaceR:      const *TypeFace
    typeFaceB:      const *TypeFace
    typeFaceI:      const *TypeFace
    typeFaceBI:     const *TypeFace
    fontSmall:      FontFamily

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
    protected mtd setup(renderer: *RenderOgl) throw
    {
        img := try Image.decode(DefaultImageName, DefaultImageBytes)
        widgetsImg = renderer.addImage(img)

        typeFaceR  = try TypeFace.create(DefaultFontRegular)
        typeFaceB  = try TypeFace.create(DefaultFontBold)
        typeFaceI  = try TypeFace.create(DefaultFontItalic)
        typeFaceBI = try TypeFace.create(DefaultFontBoldItalic)

        const FontSmallSize = 14'u32
        fontSmall.regular = Font.create(typeFaceR, FontSmallSize)
        fontSmall.bold = Font.create(typeFaceB, FontSmallSize)
        fontSmall.italic = Font.create(typeFaceI, FontSmallSize)
        fontSmall.boldItalic = Font.create(typeFaceBI, FontSmallSize)
    }
}