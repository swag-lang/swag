#global public
using Core, Pixel

internal const DefaultImageName          = "theme/widgets.png"

internal const DefaultFontRegularName    = "theme/segoeui.ttf"
internal const DefaultFontBoldName       = "theme/segoeuib.ttf"
internal const DefaultFontBoldItalicName = "theme/segoeuiz.ttf"
internal const DefaultFontItalicName     = "theme/segoeuii.ttf"

internal const DefaultFontFsRegularName    = "theme/consola.ttf"
internal const DefaultFontFsBoldName       = "theme/consolab.ttf"
internal const DefaultFontFsBoldItalicName = "theme/consolaz.ttf"
internal const DefaultFontFsItalicName     = "theme/consolai.ttf"

internal const DefaultIcons24Name        = "theme/icons24.png"
internal const DefaultIcons64Name        = "theme/icons64.png"
internal const DefaultSpinName           = "theme/spin.png"

internal const DefaultImage              = @include(DefaultImageName)
internal const DefaultFontRegular        = @include(DefaultFontRegularName)
internal const DefaultFontBold           = @include(DefaultFontBoldName)
internal const DefaultFontBoldItalic     = @include(DefaultFontBoldItalicName)
internal const DefaultFontItalic         = @include(DefaultFontItalicName)
internal const DefaultFontFsRegular      = @include(DefaultFontFsRegularName)
internal const DefaultFontFsBold         = @include(DefaultFontFsBoldName)
internal const DefaultFontFsBoldItalic   = @include(DefaultFontFsBoldItalicName)
internal const DefaultFontFsItalic       = @include(DefaultFontFsItalicName)
internal const DefaultIcons24            = @include(DefaultIcons24Name)
internal const DefaultIcons64            = @include(DefaultIcons64Name)
internal const DefaultSpin               = @include(DefaultSpinName)


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
    internal mtd freeDefault()
    {
        Memory.delete(res.icons24)
        Memory.delete(res.icons64)
        Memory.delete(res.spin)
    }

    internal mtd setupDefault(renderer: *RenderOgl)
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