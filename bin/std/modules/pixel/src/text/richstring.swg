#global public
using Core

struct RichChunk
{
    // From 'set'
    slice:      string
    fontStyle:  FontFamilyStyle = Regular

    // From 'computeLayout'
    pos:        Math.Point
    font:       *Font
    color:      Color
    under:      UnderlineStyle = None
}

struct RichString
{
    raw:        String
    chunks:     Array'RichChunk
    format:     RichStringFormat
    layoutRect: Math.Rectangle
}

impl RichString
{
    // Associate a string
    mtd set(text: string)
    {
        if raw == text
            return
        chunks.clear()
        layoutRect.clear()

        #[Swag.Mixin]
        func addChunk(idx: uint)
        {
            chunk.slice = @mkstring(@dataof(raw) + startChunk, idx - startChunk)
            chunks.add(chunk)
        }

        raw = text

        startChunk, endChunk := 0'uint
        var chunk: RichChunk

        lastIsSpace := false
        Utf8.visitRunes(|c, ci, bi| text)
        {
            subText := @mkstring(@dataof(text) + bi, @countof(text) - bi)
            if Utf8.startsWith(subText, "<b>")
            {
                @print("x")
                chunk.fontStyle = .Bold
            }

            if Unicode.isSpace(c)
            {
                if !lastIsSpace
                {
                    lastIsSpace = true
                    addChunk(bi)
                }

                continue
            }

            if lastIsSpace
                startChunk = bi
            lastIsSpace = false
        }

        // Remaining chunk
        if startChunk != @countof(text)
        {
            addChunk(@countof(text))
        }
    }

    // Recompute the layout for a given rectangle
    mtd computeLayout(pos: Math.Rectangle)
    {
        layoutRect = pos

        x := layoutRect.x
        visit *c: chunks
        {
            c.pos.x = x
            x += 100
            c.pos.y = layoutRect.y

            switch c.fontStyle
            {
            case Regular:
                c.font = format.font.regular
            case Bold:
                c.font = format.font.bold
            case Italic:
                c.font = format.font.italic
            case BoldItalic:
                c.font = format.font.boldItalic
            }

            c.color = Argb.Black
        }
    }
}