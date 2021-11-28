#global public
using Core, Pixel

private const DefaultImageName          = "theme/widgets.png"
private const DefaultFontRegularName    = "theme/segoeui.ttf"
private const DefaultFontBoldName       = "theme/segoeuib.ttf"
private const DefaultFontBoldItalicName = "theme/segoeuiz.ttf"
private const DefaultFontItalicName     = "theme/segoeuii.ttf"
private const DefaultIcons24Name        = "theme/icons24.png"
private const DefaultIcons64Name        = "theme/icons64.png"

private const DefaultImage          = #load(DefaultImageName)
private const DefaultFontRegular    = #load(DefaultFontRegularName)
private const DefaultFontBold       = #load(DefaultFontBoldName)
private const DefaultFontBoldItalic = #load(DefaultFontBoldItalicName)
private const DefaultFontItalic     = #load(DefaultFontItalicName)
private const DefaultIcons24        = #load(DefaultIcons24Name)
private const DefaultIcons64        = #load(DefaultIcons64Name)


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
    private mtd setupDefault(renderer: *RenderOgl)
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