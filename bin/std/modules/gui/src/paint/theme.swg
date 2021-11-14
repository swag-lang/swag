#global public
using Core, Pixel

protected const DefaultImageName          = "theme/widgets.png"
protected const DefaultFontRegularName    = "theme/segoe_regular.ttf"
protected const DefaultFontBoldName       = "theme/segoe_bold.ttf"
protected const DefaultFontBoldItalicName = "theme/segoe_bold_italic.ttf"
protected const DefaultFontItalicName     = "theme/segoe_italic.ttf"
protected const DefaultIcons24Name        = "theme/icons24.png"
protected const DefaultIcons64Name        = "theme/icons64.png"

protected const DefaultImage          = #load(DefaultImageName)
protected const DefaultFontRegular    = #load(DefaultFontRegularName)
protected const DefaultFontBold       = #load(DefaultFontBoldName)
protected const DefaultFontBoldItalic = #load(DefaultFontBoldItalicName)
protected const DefaultFontItalic     = #load(DefaultFontItalicName)
protected const DefaultIcons24        = #load(DefaultIcons24Name)
protected const DefaultIcons64        = #load(DefaultIcons64Name)


struct ThemeResources
{
    imgWidgets:     Texture
    icons24:        ImageList
    icons64:        ImageList
    typeFaceR:      const *TypeFace
    typeFaceB:      const *TypeFace
    typeFaceI:      const *TypeFace
    typeFaceBI:     const *TypeFace
    fontDefault:    FontFamily
}

struct Theme
{
    res:     ThemeResources
    rects:   ThemeImageRects
    metrics: ThemeMetrics
    colors:  ThemeColors
}

impl Theme
{
    protected mtd setupDefault(renderer: *RenderOgl)
    {
        img := assume Image.decode(DefaultImageName, DefaultImage)
        res.imgWidgets = renderer.addImage(img)

        img = assume Image.decode(DefaultIcons24Name, DefaultIcons24)
        res.icons24.texture = renderer.addImage(img)
        res.icons24.sizeX = 24
        res.icons24.sizeY = 24

        img = assume Image.decode(DefaultIcons64Name, DefaultIcons64)
        res.icons64.texture = renderer.addImage(img)
        res.icons64.sizeX = 64
        res.icons64.sizeY = 64

        res.typeFaceR  = assume TypeFace.create(DefaultFontRegularName, DefaultFontRegular)
        res.typeFaceB  = assume TypeFace.create(DefaultFontBoldName, DefaultFontBold)
        res.typeFaceI  = assume TypeFace.create(DefaultFontItalicName, DefaultFontItalic)
        res.typeFaceBI = assume TypeFace.create(DefaultFontBoldItalicName, DefaultFontBoldItalic)

        res.fontDefault.regular = Font.create(res.typeFaceR, 13)
        res.fontDefault.bold = Font.create(res.typeFaceB, 13)
        res.fontDefault.italic = Font.create(res.typeFaceI, 13)
        res.fontDefault.boldItalic = Font.create(res.typeFaceBI, 13)
    }
}