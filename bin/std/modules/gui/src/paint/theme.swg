#global public
using Core, Pixel

internal const DefaultImageName = "theme/widgets.png"

internal const DefaultFontRegularName = "theme/segoeui.ttf"
internal const DefaultFontBoldName = "theme/segoeuib.ttf"
internal const DefaultFontBoldItalicName = "theme/segoeuiz.ttf"
internal const DefaultFontItalicName = "theme/segoeuii.ttf"

internal const DefaultFontFsRegularName = "theme/consola.ttf"
internal const DefaultFontFsBoldName = "theme/consolab.ttf"
internal const DefaultFontFsBoldItalicName = "theme/consolaz.ttf"
internal const DefaultFontFsItalicName = "theme/consolai.ttf"

internal const DefaultIcons24Name = "theme/icons24.png"
internal const DefaultIcons64Name = "theme/icons64.png"
internal const DefaultSpinName = "theme/spin.png"

internal const DefaultImage = #include(DefaultImageName)
internal const DefaultFontRegular = #include(DefaultFontRegularName)
internal const DefaultFontBold = #include(DefaultFontBoldName)
internal const DefaultFontBoldItalic = #include(DefaultFontBoldItalicName)
internal const DefaultFontItalic = #include(DefaultFontItalicName)
internal const DefaultFontFsRegular = #include(DefaultFontFsRegularName)
internal const DefaultFontFsBold = #include(DefaultFontFsBoldName)
internal const DefaultFontFsBoldItalic = #include(DefaultFontFsBoldItalicName)
internal const DefaultFontFsItalic = #include(DefaultFontFsItalicName)
internal const DefaultIcons24 = #include(DefaultIcons24Name)
internal const DefaultIcons64 = #include(DefaultIcons64Name)
internal const DefaultSpin = #include(DefaultSpinName)

struct ThemeResources
{
    imgWidgets:              Texture
    icons24:                 *ImageList
    icons64:                 *ImageList
    spin:                    *ImageList
    defaultTypeFaceR:        *TypeFace
    defaultTypeFaceB:        *TypeFace
    defaultTypeFaceI:        *TypeFace
    defaultTypeFaceBI:       *TypeFace
    defaultFsTypeFaceR:      *TypeFace
    defaultFsTypeFaceB:      *TypeFace
    defaultFsTypeFaceI:      *TypeFace
    defaultFsTypeFaceBI:     *TypeFace
    fontDefault:             FontFamily
    fontDefaultFs:           FontFamily
}

struct Theme
{
    intialized:     bool
    res:            ThemeResources
    rects:          ThemeImageRects
    metrics:        ThemeMetrics
    colors:         ThemeColors
}

impl Theme
{
    internal mtd freeDefault()
    {
        Memory.delete(.res.icons24)
        Memory.delete(.res.icons64)
        Memory.delete(.res.spin)
    }

    internal mtd setupDefault(renderer: *RenderOgl)
    {
        .intialized = true
        .colors.setDark()

        with me.res
        {
            var img = assume Image.decode(DefaultImageName, DefaultImage)
            .imgWidgets = renderer.addImage(img)

            img      = assume Image.decode(DefaultIcons24Name, DefaultIcons24)
            .icons24 = Memory.new'ImageList()
            .icons24.set(renderer.addImage(img), 24, 24)

            img      = assume Image.decode(DefaultIcons64Name, DefaultIcons64)
            .icons64 = Memory.new'ImageList()
            .icons64.set(renderer.addImage(img), 64, 64)

            img   = assume Image.decode(DefaultSpinName, DefaultSpin)
            .spin = Memory.new'ImageList()
            .spin.set(renderer.addImage(img), 48, 48)

            .defaultTypeFaceR  = assume TypeFace.create(DefaultFontRegularName, DefaultFontRegular)
            .defaultTypeFaceB  = assume TypeFace.create(DefaultFontBoldName, DefaultFontBold)
            .defaultTypeFaceI  = assume TypeFace.create(DefaultFontItalicName, DefaultFontItalic)
            .defaultTypeFaceBI = assume TypeFace.create(DefaultFontBoldItalicName, DefaultFontBoldItalic)

            .defaultFsTypeFaceR  = assume TypeFace.create(DefaultFontFsRegularName, DefaultFontFsRegular)
            .defaultFsTypeFaceB  = assume TypeFace.create(DefaultFontFsBoldName, DefaultFontFsBold)
            .defaultFsTypeFaceI  = assume TypeFace.create(DefaultFontFsItalicName, DefaultFontFsItalic)
            .defaultFsTypeFaceBI = assume TypeFace.create(DefaultFontFsBoldItalicName, DefaultFontFsBoldItalic)

            .fontDefault.regular    = Font.create(.defaultTypeFaceR, me.metrics.defaultFontSize)
            .fontDefault.bold       = Font.create(.defaultTypeFaceB, me.metrics.defaultFontSize)
            .fontDefault.italic     = Font.create(.defaultTypeFaceI, me.metrics.defaultFontSize)
            .fontDefault.boldItalic = Font.create(.defaultTypeFaceBI, me.metrics.defaultFontSize)

            .fontDefaultFs.regular    = Font.create(.defaultFsTypeFaceR, me.metrics.defaultFixedFontSize)
            .fontDefaultFs.bold       = Font.create(.defaultFsTypeFaceB, me.metrics.defaultFixedFontSize)
            .fontDefaultFs.italic     = Font.create(.defaultFsTypeFaceI, me.metrics.defaultFixedFontSize)
            .fontDefaultFs.boldItalic = Font.create(.defaultFsTypeFaceBI, me.metrics.defaultFixedFontSize)
        }
    }

    // Create a default font family with the given size
    mtd createDefaultFontfamily(size: u32)->FontFamily
    {
        var result: retval
        result.regular    = Font.create(.res.defaultTypeFaceR, size)
        result.bold       = Font.create(.res.defaultTypeFaceB, size)
        result.italic     = Font.create(.res.defaultTypeFaceI, size)
        result.boldItalic = Font.create(.res.defaultTypeFaceBI, size)
        return result
    }

    // Create a default font with the given size
    mtd createDefaultFont(size: u32, style = FontFamilyStyle.Regular)->*Font
    {
        #[Swag.Complete]
        switch style
        {
        case Regular:
            return Font.create(.res.defaultTypeFaceR, size)
        case Bold:
            return Font.create(.res.defaultTypeFaceB, size)
        case Italic:
            return Font.create(.res.defaultTypeFaceI, size)
        case BoldItalic:
            return Font.create(.res.defaultTypeFaceBI, size)
        }

        unreachable
    }

    // Create a default font (fixed size) with the given size
    mtd createDefaultFontFS(size: u32, style = FontFamilyStyle.Regular)->*Font
    {
        #[Swag.Complete]
        switch style
        {
        case Regular:
            return Font.create(.res.defaultFsTypeFaceR, size)
        case Bold:
            return Font.create(.res.defaultFsTypeFaceB, size)
        case Italic:
            return Font.create(.res.defaultFsTypeFaceI, size)
        case BoldItalic:
            return Font.create(.res.defaultFsTypeFaceBI, size)
        }

        unreachable
    }
}
