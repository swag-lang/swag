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
    mtd getFont(style: FontFamilyStyle)->#null *Font
    {
        switch style
        {
        case Regular:
            if me.regular:
                return me.regular

        case Bold:
            if me.bold:
                return me.bold
            if me.regular:
                return me.regular

        case Italic:
            if me.italic:
                return me.italic
            if me.regular:
                return me.regular

        case BoldItalic:
            if me.boldItalic:
                return me.boldItalic
            if me.italic:
                return me.italic
            if me.bold:
                return me.bold
            if me.regular:
                return me.regular
        }

        return null
    }

    // Set the font corresponding to a given style.
    mtd setFont(fnt: *Font, style: FontFamilyStyle)
    {
        switch style
        {
        case Regular:
            me.regular = fnt
        case Bold:
            me.bold = fnt
        case Italic:
            me.italic = fnt
        case BoldItalic:
            me.boldItalic = fnt
        }
    }
}
