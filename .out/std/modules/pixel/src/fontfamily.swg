#global public

struct FontFamily
{
    regular:    Font
    bold:       Font
    italic:     Font
    boldItalic: Font
}

enum FontFamilyStyle
{
    Regular
    Bold
    Italic
    BoldItalic
}

impl FontFamily
{
    // Get the font correspondig to the style.
    // If not evailable, get another one.
    func getFont(using self, style: FontFamilyStyle)->const *Font
    {
        switch style
        {
        case Regular:
            if regular.size return regular

        case Bold:
            if bold.size return bold
            if regular.size return regular

        case Italic:
            if italic.size return italic
            if regular.size return regular

        case BoldItalic:
            if boldItalic.size return boldItalic
            if italic.size return italic
            if bold.size return bold
            if regular.size return regular
        }

        return null
    }
}