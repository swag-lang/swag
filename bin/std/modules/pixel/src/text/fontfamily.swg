#global public

struct FontFamily
{
    regular:        *Font
    bold:           *Font
    italic:         *Font
    boldItalic:     *Font
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
    // Get the font corresponding to the style.
    // If not evailable, get another one.
    mtd const getFont(style: FontFamilyStyle)->#null *Font
    {
        switch style
        {
        case Regular:
            if .regular:
                return .regular

        case Bold:
            if .bold:
                return .bold
            if .regular:
                return .regular

        case Italic:
            if .italic:
                return .italic
            if .regular:
                return .regular

        case BoldItalic:
            if .boldItalic:
                return .boldItalic
            if .italic:
                return .italic
            if .bold:
                return .bold
            if .regular:
                return .regular
        }

        return null
    }

    // Set the font corresponding to a given style.
    mtd setFont(fnt: *Font, style: FontFamilyStyle)
    {
        switch style
        {
        case Regular:
            .regular = fnt
        case Bold:
            .bold = fnt
        case Italic:
            .italic = fnt
        case BoldItalic:
            .boldItalic = fnt
        }
    }
}
