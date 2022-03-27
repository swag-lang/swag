using Core

#[Swag.Opaque]
public struct RichEditLine
{
    chars:              Array'rune
    styles:             Array'RichEditStyleRef
    dirtyStyle:         bool
    hidden:             uint
    drawingRect:        Math.Rectangle
    lexerEndingState:   s32
    lexerCptCmt:        s32
    wordWrap:           bool
}

impl RichEditLine
{
    #[Swag.Inline]
    {
        mtd length()    => styles.count
        mtd isVisible() => hidden == 0
    }

    mtd resetInternal()
    {
        dirtyStyle = true
        hidden = 0
        chars.clear()
        styles.clear()
    }

    mtd insertRune(insIndex: uint, c: rune, txtStyle: u8)
    {
        dirtyStyle = true
        chars.insertAt(insIndex, c)
        styles.insertAt(insIndex, txtStyle)
    }

    mtd insertRunes(insIndex: uint, src: RichEditLine, srcIndex, srcCount: uint)
    {
        dirtyStyle = true
        srcI := srcIndex
        for i := insIndex; i < insIndex + srcCount; i += 1
        {
            chars.insertAt(i, src.chars[srcI])
            styles.insertAt(i, src.styles[srcI])
            srcI += 1
        }
    }

    mtd insertRunes(insIndex: uint, runes: const [..] rune, styles: const [..] u8)
    {
        dirtyStyle = true
        srcI := 0'uint
        for i := insIndex; i < insIndex + @countof(runes); i += 1
        {
            self.chars.insertAt(i, #self.runes[srcI])
            self.styles.insertAt(i, #self.styles[srcI])
            srcI += 1
        }
    }

    mtd removeRunes(srcIndex, count: uint)
    {
        if !count return
        dirtyStyle = true
        chars.remove(srcIndex, count)
        styles.remove(srcIndex, count)
    }

    mtd setLength(l: uint)
    {
        chars.resize(l)
        styles.resize(l)
    }
}