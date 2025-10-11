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
        mtd length()    => .styles.count
        mtd isVisible() => .hidden == 0
    }

    mtd resetInternal()
    {
        .dirtyStyle = true
        .wordWrap   = false
        .hidden     = 0
        .chars.clear()
        .styles.clear()
    }

    mtd insertRune(insIndex: u64, c: rune, txtStyle: u8)
    {
        .dirtyStyle = true
        .chars.insertAt(insIndex, c)
        .styles.insertAt(insIndex, txtStyle)
    }

    #[Swag.Overload]
    mtd insertRunes(insIndex: u64, src: RichEditLine, srcIndex, srcCount: u64)
    {
        .dirtyStyle = true
        var srcI = srcIndex
        for var i = insIndex; i < insIndex + srcCount; i += 1
        {
            .chars.insertAt(i, src.chars[srcI])
            .styles.insertAt(i, src.styles[srcI])
            srcI += 1
        }
    }

    #[Swag.Overload]
    mtd insertRunes(insIndex: u64, runes: const [..] rune, styles: const [..] u8)
    {
        .dirtyStyle = true
        var srcI = 0'u64
        for var i = insIndex; i < insIndex + @countof(runes); i += 1
        {
            .chars.insertAt(i, runes[srcI])
            .styles.insertAt(i, styles[srcI])
            srcI += 1
        }
    }

    mtd removeRunes(srcIndex, count: u64)
    {
        if !count do
            return
        .dirtyStyle = true
        .chars.remove(srcIndex, count)
        .styles.remove(srcIndex, count)
    }

    mtd setLength(l: u64)
    {
        .chars.resize(l)
        .styles.resize(l)
    }
}
