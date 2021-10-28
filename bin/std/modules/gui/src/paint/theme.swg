#global public
using Core, Pixel

protected const DefaultImageName  = "theme/widgets.png"
protected const DefaultImage      = #load(DefaultImageName)

protected const DefaultFontRegularName    = "theme/segoe_regular.ttf"
protected const DefaultFontBoldName       = "theme/segoe_bold.ttf"
protected const DefaultFontBoldItalicName = "theme/segoe_bold_italic.ttf"
protected const DefaultFontItalicName     = "theme/segoe_italic.ttf"
protected const DefaultFontRegular    = #load(DefaultFontRegularName)
protected const DefaultFontBold       = #load(DefaultFontBoldName)
protected const DefaultFontBoldItalic = #load(DefaultFontBoldItalicName)
protected const DefaultFontItalic     = #load(DefaultFontItalicName)

protected const DefaultIcons24Name  = "theme/icons24.png"
protected const DefaultIcons24      = #load(DefaultIcons24Name)

struct Theme
{
    imgWidgets:     Texture
    icons24:        ImageList
    typeFaceR:      const *TypeFace
    typeFaceB:      const *TypeFace
    typeFaceI:      const *TypeFace
    typeFaceBI:     const *TypeFace
    fontSmall:      FontFamily
    palette:        Palette

    windowBorderSize:   f32 = 3
    buttonMargins:      f32 = 5
    bigCaptionCY:       f32 = 30
    smallCaptionCY:     f32 = 24
}

impl Theme
{
    protected mtd setup(renderer: *RenderOgl)
    {
        img := assume Image.decode(DefaultImageName, DefaultImage)
        imgWidgets = renderer.addImage(img)

        img = assume Image.decode(DefaultIcons24Name, DefaultIcons24)
        icons24.texture = renderer.addImage(img)
        icons24.sizeX = 24
        icons24.sizeY = 24

        typeFaceR  = assume TypeFace.create(DefaultFontRegularName, DefaultFontRegular)
        typeFaceB  = assume TypeFace.create(DefaultFontBoldName, DefaultFontBold)
        typeFaceI  = assume TypeFace.create(DefaultFontItalicName, DefaultFontItalic)
        typeFaceBI = assume TypeFace.create(DefaultFontBoldItalicName, DefaultFontBoldItalic)

        const FontSmallSize = 13'u32
        fontSmall.regular = Font.create(typeFaceR, FontSmallSize)
        fontSmall.bold = Font.create(typeFaceB, FontSmallSize)
        fontSmall.italic = Font.create(typeFaceI, FontSmallSize)
        fontSmall.boldItalic = Font.create(typeFaceBI, FontSmallSize)
    }
}