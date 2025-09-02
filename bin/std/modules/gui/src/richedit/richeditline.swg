using Core

#[Swag.Opaque]
public struct RichEditLine
{
    chars:                Array'rune
    styles:               Array'RichEditStyleRef
    dirtyStyle:           bool
    hidden:               u64
    drawingRect:          Math.Rectangle
    lexerEndingState:     s32
    lexerCptCmt:          s32
    wordWrap:             bool
}

impl RichEditLine
{
    #[Swag.Inline]
    {
        mtd length()    => me.styles.count
        mtd isVisible() => me.hidden == 0
    }

    mtd resetInternal()
    {
        me.dirtyStyle = true
        me.wordWrap   = false
        me.hidden     = 0
        me.chars.clear()
        me.styles.clear()
    }

    mtd insertRune(insIndex: u64, c: rune, txtStyle: u8)
    {
        me.dirtyStyle = true
        me.chars.insertAt(insIndex, c)
        me.styles.insertAt(insIndex, txtStyle)
    }

    #[Swag.Overload]
    mtd insertRunes(insIndex: u64, src: RichEditLine, srcIndex, srcCount: u64)
    {
        me.dirtyStyle = true
        var srcI = srcIndex
        for var i = insIndex; i < insIndex + srcCount; i += 1
        {
            me.chars.insertAt(i, src.chars[srcI])
            me.styles.insertAt(i, src.styles[srcI])
            srcI += 1
        }
    }

    #[Swag.Overload]
    mtd insertRunes(insIndex: u64, runes: const [..] rune, styles: const [..] u8)
    {
        me.dirtyStyle = true
        var srcI = 0'u64
        for var i = insIndex; i < insIndex + @countof(runes); i += 1
        {
            me.chars.insertAt(i, runes[srcI])
            me.styles.insertAt(i, styles[srcI])
            srcI += 1
        }
    }

    mtd removeRunes(srcIndex, count: u64)
    {
        if !count:
            return
        me.dirtyStyle = true
        me.chars.remove(srcIndex, count)
        me.styles.remove(srcIndex, count)
    }

    mtd setLength(l: u64)
    {
        me.chars.resize(l)
        me.styles.resize(l)
    }
}
