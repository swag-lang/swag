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
        Memory.delete(me.res.icons24)
        Memory.delete(me.res.icons64)
        Memory.delete(me.res.spin)
    }

    internal mtd setupDefault(renderer: *RenderOgl)
    {
        me.intialized = true

        var img = assume Image.decode(DefaultImageName, DefaultImage)
        me.res.imgWidgets = renderer.addImage(img)

        img            = assume Image.decode(DefaultIcons24Name, DefaultIcons24)
        me.res.icons24 = Memory.new'ImageList()
        me.res.icons24.set(renderer.addImage(img), 24, 24)

        img            = assume Image.decode(DefaultIcons64Name, DefaultIcons64)
        me.res.icons64 = Memory.new'ImageList()
        me.res.icons64.set(renderer.addImage(img), 64, 64)

        img         = assume Image.decode(DefaultSpinName, DefaultSpin)
        me.res.spin = Memory.new'ImageList()
        me.res.spin.set(renderer.addImage(img), 48, 48)

        me.res.defaultTypeFaceR  = assume TypeFace.create(DefaultFontRegularName, DefaultFontRegular)
        me.res.defaultTypeFaceB  = assume TypeFace.create(DefaultFontBoldName, DefaultFontBold)
        me.res.defaultTypeFaceI  = assume TypeFace.create(DefaultFontItalicName, DefaultFontItalic)
        me.res.defaultTypeFaceBI = assume TypeFace.create(DefaultFontBoldItalicName, DefaultFontBoldItalic)

        me.res.defaultFsTypeFaceR  = assume TypeFace.create(DefaultFontFsRegularName, DefaultFontFsRegular)
        me.res.defaultFsTypeFaceB  = assume TypeFace.create(DefaultFontFsBoldName, DefaultFontFsBold)
        me.res.defaultFsTypeFaceI  = assume TypeFace.create(DefaultFontFsItalicName, DefaultFontFsItalic)
        me.res.defaultFsTypeFaceBI = assume TypeFace.create(DefaultFontFsBoldItalicName, DefaultFontFsBoldItalic)

        me.res.fontDefault.regular    = Font.create(me.res.defaultTypeFaceR, me.metrics.defaultFontSize)
        me.res.fontDefault.bold       = Font.create(me.res.defaultTypeFaceB, me.metrics.defaultFontSize)
        me.res.fontDefault.italic     = Font.create(me.res.defaultTypeFaceI, me.metrics.defaultFontSize)
        me.res.fontDefault.boldItalic = Font.create(me.res.defaultTypeFaceBI, me.metrics.defaultFontSize)

        me.res.fontDefaultFs.regular    = Font.create(me.res.defaultFsTypeFaceR, me.metrics.defaultFixedFontSize)
        me.res.fontDefaultFs.bold       = Font.create(me.res.defaultFsTypeFaceB, me.metrics.defaultFixedFontSize)
        me.res.fontDefaultFs.italic     = Font.create(me.res.defaultFsTypeFaceI, me.metrics.defaultFixedFontSize)
        me.res.fontDefaultFs.boldItalic = Font.create(me.res.defaultFsTypeFaceBI, me.metrics.defaultFixedFontSize)
    }

    // Create a default font family with the given size
    mtd createDefaultFontfamily(size: u32)->FontFamily
    {
        var result: retval
        result.regular    = Font.create(me.res.defaultTypeFaceR, size)
        result.bold       = Font.create(me.res.defaultTypeFaceB, size)
        result.italic     = Font.create(me.res.defaultTypeFaceI, size)
        result.boldItalic = Font.create(me.res.defaultTypeFaceBI, size)
        return result
    }

    // Create a default font with the given size
    mtd createDefaultFont(size: u32, style = FontFamilyStyle.Regular)->*Font
    {
        #[Swag.Complete]
        switch style
        {
        case Regular:
            return Font.create(me.res.defaultTypeFaceR, size)
        case Bold:
            return Font.create(me.res.defaultTypeFaceB, size)
        case Italic:
            return Font.create(me.res.defaultTypeFaceI, size)
        case BoldItalic:
            return Font.create(me.res.defaultTypeFaceBI, size)
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
            return Font.create(me.res.defaultFsTypeFaceR, size)
        case Bold:
            return Font.create(me.res.defaultFsTypeFaceB, size)
        case Italic:
            return Font.create(me.res.defaultFsTypeFaceI, size)
        case BoldItalic:
            return Font.create(me.res.defaultFsTypeFaceBI, size)
        }

        unreachable
    }
}
