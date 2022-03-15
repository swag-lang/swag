#global public
using Core

struct RichEditLine
{
    chars:          Array'rune
    styles:         Array'u8
    dirtyStyle:     bool
    dirtyFold:      bool
    hidden:         bool
    drawingRect:    Math.Rectangle
}

impl RichEditLine
{
    #[Swag.Inline]
    {
        mtd length()    => styles.count
        mtd isVisible() => !hidden
    }

    mtd insertRune(insIndex: uint, src: rune)
    {
        dirtyStyle = true
        dirtyFold = true
        chars.insertAt(insIndex, src)
        styles.insertAt(insIndex, 0)
    }

    mtd insertRunes(insIndex: uint, src: RichEditLine, srcIndex, srcCount: uint)
    {
        dirtyStyle = true
        dirtyFold = true

        srcI := srcIndex
        for i := insIndex; i < insIndex + srcCount; i += 1
        {
            chars.insertAt(i, src.chars[srcI]);
            styles.insertAt(i, src.styles[srcI])
            srcI += 1
        }
    }

    mtd setLength(l: uint)
    {
        chars.resize(l)
        styles.resize(l)
    }
}