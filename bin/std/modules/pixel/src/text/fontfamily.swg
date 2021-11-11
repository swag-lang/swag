#global public

struct FontFamily
{
    regular:    *Font
    bold:       *Font
    italic:     *Font
    boldItalic: *Font
}

enum FontFamilyStyle
{
    Default
    Regular
    Bold
    Italic
    BoldItalic
}

impl FontFamily
{
    // Get the font correspondig to the style.
    // If not evailable, get another one.
    mtd getFont(style: FontFamilyStyle)->*Font
    {
        switch style
        {
        case Regular, Default:
            if regular return regular

        case Bold:
            if bold return bold
            if regular return regular

        case Italic:
            if italic return italic
            if regular return regular

        case BoldItalic:
            if boldItalic return boldItalic
            if italic return italic
            if bold return bold
            if regular return regular
        }

        return null
    }
}