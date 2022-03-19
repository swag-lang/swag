#global public
using Core, Pixel

private const DefaultImageName          = "theme/widgets.png"

private const DefaultFontRegularName    = "theme/segoeui.ttf"
private const DefaultFontBoldName       = "theme/segoeuib.ttf"
private const DefaultFontBoldItalicName = "theme/segoeuiz.ttf"
private const DefaultFontItalicName     = "theme/segoeuii.ttf"

private const DefaultFontFsRegularName    = "theme/consola.ttf"
private const DefaultFontFsBoldName       = "theme/consolab.ttf"
private const DefaultFontFsBoldItalicName = "theme/consolaz.ttf"
private const DefaultFontFsItalicName     = "theme/consolai.ttf"

private const DefaultIcons24Name        = "theme/icons24.png"
private const DefaultIcons64Name        = "theme/icons64.png"

private const DefaultImage              = #load(DefaultImageName)
private const DefaultFontRegular        = #load(DefaultFontRegularName)
private const DefaultFontBold           = #load(DefaultFontBoldName)
private const DefaultFontBoldItalic     = #load(DefaultFontBoldItalicName)
private const DefaultFontItalic         = #load(DefaultFontItalicName)
private const DefaultFontFsRegular      = #load(DefaultFontFsRegularName)
private const DefaultFontFsBold         = #load(DefaultFontFsBoldName)
private const DefaultFontFsBoldItalic   = #load(DefaultFontFsBoldItalicName)
private const DefaultFontFsItalic       = #load(DefaultFontFsItalicName)
private const DefaultIcons24            = #load(DefaultIcons24Name)
private const DefaultIcons64            = #load(DefaultIcons64Name)


struct ThemeResources
{
    imgWidgets:             Texture
    icons24:                *ImageList
    icons64:                *ImageList
    defaultTypeFaceR:       const *TypeFace
    defaultTypeFaceB:       const *TypeFace
    defaultTypeFaceI:       const *TypeFace
    defaultTypeFaceBI:      const *TypeFace
    defaultFsTypeFaceR:     const *TypeFace
    defaultFsTypeFaceB:     const *TypeFace
    defaultFsTypeFaceI:     const *TypeFace
    defaultFsTypeFaceBI:    const *TypeFace
    fontDefault:            FontFamily
    fontDefaultFs:          FontFamily
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
        res.icons24 = Memory.new'ImageList()
        res.icons24.texture = renderer.addImage(img)
        res.icons24.sizeX = 24
        res.icons24.sizeY = 24

        img = assume Image.decode(DefaultIcons64Name, DefaultIcons64)
        res.icons64 = Memory.new'ImageList()
        res.icons64.texture = renderer.addImage(img)
        res.icons64.sizeX = 64
        res.icons64.sizeY = 64

        res.defaultTypeFaceR    = assume TypeFace.create(DefaultFontRegularName, DefaultFontRegular)
        res.defaultTypeFaceB    = assume TypeFace.create(DefaultFontBoldName, DefaultFontBold)
        res.defaultTypeFaceI    = assume TypeFace.create(DefaultFontItalicName, DefaultFontItalic)
        res.defaultTypeFaceBI   = assume TypeFace.create(DefaultFontBoldItalicName, DefaultFontBoldItalic)

        res.defaultFsTypeFaceR  = assume TypeFace.create(DefaultFontFsRegularName, DefaultFontFsRegular)
        res.defaultFsTypeFaceB  = assume TypeFace.create(DefaultFontFsBoldName, DefaultFontFsBold)
        res.defaultFsTypeFaceI  = assume TypeFace.create(DefaultFontFsItalicName, DefaultFontFsItalic)
        res.defaultFsTypeFaceBI = assume TypeFace.create(DefaultFontFsBoldItalicName, DefaultFontFsBoldItalic)

        res.fontDefault.regular = Font.create(res.defaultTypeFaceR, metrics.defaultFontSize)
        res.fontDefault.bold = Font.create(res.defaultTypeFaceB, metrics.defaultFontSize)
        res.fontDefault.italic = Font.create(res.defaultTypeFaceI, metrics.defaultFontSize)
        res.fontDefault.boldItalic = Font.create(res.defaultTypeFaceBI, metrics.defaultFontSize)

        res.fontDefaultFs.regular = Font.create(res.defaultFsTypeFaceR, metrics.defaultFontSize)
        res.fontDefaultFs.bold = Font.create(res.defaultFsTypeFaceB, metrics.defaultFontSize)
        res.fontDefaultFs.italic = Font.create(res.defaultFsTypeFaceI, metrics.defaultFontSize)
        res.fontDefaultFs.boldItalic = Font.create(res.defaultFsTypeFaceBI, metrics.defaultFontSize)
    }
}