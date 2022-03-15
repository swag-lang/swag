#global public
using Core

struct RichEditLine
{
    chars:          String
    styles:         Array'u8
    dirtyStyle:     bool
    dirtyFold:      bool
}

impl RichEditLine
{
    #[Swag.Inline]
    mtd length() => styles.count

    mtd insertText(insIndex: uint, src: string)
    {
        dirtyStyle = true
        dirtyFold = true
        chars.insert(insIndex, src)
        loop i: @countof(src)
            styles.insertAt(insIndex + i, 0)
    }
}