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
private const DefaultSpinName           = "theme/spin.png"

private const DefaultImage              = #include(DefaultImageName)
private const DefaultFontRegular        = #include(DefaultFontRegularName)
private const DefaultFontBold           = #include(DefaultFontBoldName)
private const DefaultFontBoldItalic     = #include(DefaultFontBoldItalicName)
private const DefaultFontItalic         = #include(DefaultFontItalicName)
private const DefaultFontFsRegular      = #include(DefaultFontFsRegularName)
private const DefaultFontFsBold         = #include(DefaultFontFsBoldName)
private const DefaultFontFsBoldItalic   = #include(DefaultFontFsBoldItalicName)
private const DefaultFontFsItalic       = #include(DefaultFontFsItalicName)
private const DefaultIcons24            = #include(DefaultIcons24Name)
private const DefaultIcons64            = #include(DefaultIcons64Name)
private const DefaultSpin               = #include(DefaultSpinName)


struct ThemeResources
{
    imgWidgets:             Texture
    icons24:                *ImageList
    icons64:                *ImageList
    spin:                   *ImageList
    defaultTypeFaceR:       *TypeFace
    defaultTypeFaceB:       *TypeFace
    defaultTypeFaceI:       *TypeFace
    defaultTypeFaceBI:      *TypeFace
    defaultFsTypeFaceR:     *TypeFace
    defaultFsTypeFaceB:     *TypeFace
    defaultFsTypeFaceI:     *TypeFace
    defaultFsTypeFaceBI:    *TypeFace
    fontDefault:            FontFamily
    fontDefaultFs:          FontFamily
}

struct Theme
{
    intialized: bool
    res:        ThemeResources
    rects:      ThemeImageRects
    metrics:    ThemeMetrics
    colors:     ThemeColors
}

impl Theme
{
    private mtd freeDefault()
    {
        Memory.delete(res.icons24)
        Memory.delete(res.icons64)
        Memory.delete(res.spin)
    }

    private mtd setupDefault(renderer: *RenderOgl)
    {
        intialized = true

        var img = assume Image.decode(DefaultImageName, DefaultImage)
        res.imgWidgets = renderer.addImage(img)

        img = assume Image.decode(DefaultIcons24Name, DefaultIcons24)
        res.icons24 = Memory.new'ImageList()
        res.icons24.set(renderer.addImage(img), 24, 24)

        img = assume Image.decode(DefaultIcons64Name, DefaultIcons64)
        res.icons64 = Memory.new'ImageList()
        res.icons64.set(renderer.addImage(img), 64, 64)

        img = assume Image.decode(DefaultSpinName, DefaultSpin)
        res.spin = Memory.new'ImageList()
        res.spin.set(renderer.addImage(img), 48, 48)

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

        res.fontDefaultFs.regular = Font.create(res.defaultFsTypeFaceR, metrics.defaultFixedFontSize)
        res.fontDefaultFs.bold = Font.create(res.defaultFsTypeFaceB, metrics.defaultFixedFontSize)
        res.fontDefaultFs.italic = Font.create(res.defaultFsTypeFaceI, metrics.defaultFixedFontSize)
        res.fontDefaultFs.boldItalic = Font.create(res.defaultFsTypeFaceBI, metrics.defaultFixedFontSize)
    }

    // Create a default font with the given size
    mtd createDefaultFont(size: u32, style = FontFamilyStyle.Regular)->*Font
    {
        switch style
        {
        case Regular:       return Font.create(res.defaultTypeFaceR, size)
        case Bold:          return Font.create(res.defaultTypeFaceB, size)
        case Italic:        return Font.create(res.defaultTypeFaceI, size)
        case BoldItalic:    return Font.create(res.defaultTypeFaceBI, size)
        }

        Debug.assert(false, "invalid font style")
        return null
    }

    // Create a default font (fixed size) with the given size
    mtd createDefaultFontFS(size: u32, style = FontFamilyStyle.Regular)->*Font
    {
        switch style
        {
        case Regular:       return Font.create(res.defaultFsTypeFaceR, size)
        case Bold:          return Font.create(res.defaultFsTypeFaceB, size)
        case Italic:        return Font.create(res.defaultFsTypeFaceI, size)
        case BoldItalic:    return Font.create(res.defaultFsTypeFaceBI, size)
        }

        Debug.assert(false, "invalid font style")
        return null
    }
}