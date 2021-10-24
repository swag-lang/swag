#global public
using Core, Pixel

protected const DefaultImageName          = "widgets.png"
protected const DefaultFontRegularName    = "segoe_regular.ttf"
protected const DefaultFontBoldName       = "segoe_bold.ttf"
protected const DefaultFontBoldItalicName = "segoe_bold_italic.ttf"
protected const DefaultFontItalicName     = "segoe_italic.ttf"

protected const DefaultImage          = #load(DefaultImageName)
protected const DefaultFontRegular    = #load(DefaultFontRegularName)
protected const DefaultFontBold       = #load(DefaultFontBoldName)
protected const DefaultFontBoldItalic = #load(DefaultFontBoldItalicName)
protected const DefaultFontItalic     = #load(DefaultFontItalicName)

struct Theme
{
    widgetsImg:     Texture
    typeFaceR:      const *TypeFace
    typeFaceB:      const *TypeFace
    typeFaceI:      const *TypeFace
    typeFaceBI:     const *TypeFace
    fontSmall:      FontFamily

    colorWndBackground: Color = 0xFF333333
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
        img := try Image.decode(DefaultImageName, DefaultImage)
        widgetsImg = renderer.addImage(img)

        typeFaceR  = try TypeFace.create(DefaultFontRegularName, DefaultFontRegular)
        typeFaceB  = try TypeFace.create(DefaultFontBoldName, DefaultFontBold)
        typeFaceI  = try TypeFace.create(DefaultFontItalicName, DefaultFontItalic)
        typeFaceBI = try TypeFace.create(DefaultFontBoldItalicName, DefaultFontBoldItalic)

        const FontSmallSize = 13'u32
        fontSmall.regular = Font.create(typeFaceR, FontSmallSize)
        fontSmall.bold = Font.create(typeFaceB, FontSmallSize)
        fontSmall.italic = Font.create(typeFaceI, FontSmallSize)
        fontSmall.boldItalic = Font.create(typeFaceBI, FontSmallSize)
    }
}