#global public
using Core

struct RichEditLine
{
    chars:          Array'rune
    styles:         Array'u8
    dirtyStyle:     bool
    dirtyFold:      bool
    drawingRect:    Math.Rectangle
}

impl RichEditLine
{
    #[Swag.Inline]
    mtd length() => styles.count

    mtd insertRune(insIndex: uint, src: rune)
    {
        dirtyStyle = true
        dirtyFold = true
        chars.insertAt(insIndex, src)
        styles.insertAt(insIndex, 0)
    }
}