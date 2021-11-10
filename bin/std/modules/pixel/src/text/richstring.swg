#global public
using Core

struct RichChunk
{
    // From 'set'
    slice:      string
    fontStyle:  FontFamilyStyle = Regular
    isSpace:    bool

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
        raw = text

        chunks.clear()
        layoutRect.clear()

        count := raw.length
        if !count
            return

        #[Swag.Mixin]
        func addChunk(idx: uint)
        {
            if idx - startChunk == 0
                return
            chunk.isSpace = false
            if cptBold > 0
                chunk.fontStyle = .Bold
            else
                chunk.fontStyle = .Regular
            chunk.slice = @mkstring(raw.buffer + startChunk, idx - startChunk)
            chunks.add(chunk)
            startChunk = byteIdx
        }

        startChunk, endChunk := 0'uint
        var chunk: RichChunk

        byteIdx := 0'uint
        charIdx := 0'uint
        cptBold := 0

        scan := raw.buffer
        while count
        {
            subText := @mkstring(raw.buffer + byteIdx, raw.length - byteIdx)

            if Utf8.startsWith(subText, "<b>")
            {
                addChunk(byteIdx)
                cptBold += 1
                byteIdx, scan, startChunk += 3
                count -= 3
                continue
            }

            if Utf8.startsWith(subText, "</b>")
            {
                addChunk(byteIdx)
                cptBold -= 1
                byteIdx, scan, startChunk += 4
                count -= 4
                continue
            }

            sl := @mkslice(scan, count)
            (c, eat) := Utf8.decodeRune(sl)
            if eat == 0
                break

            if Unicode.isSpace(c)
            {
                addChunk(byteIdx)

                chunk.isSpace = true
                chunk.slice = null
                chunks.add(chunk)

                while eat and Unicode.isSpace(c)
                {
                    startChunk, byteIdx, scan += eat
                    count -= eat
                    sl = @mkslice(scan, count)
                    (c, eat) = Utf8.decodeRune(sl)
                }

                continue
            }

            byteIdx, scan += eat
            count -= eat
        }

        // Remaining chunk
        if startChunk != raw.length
        {
            addChunk(raw.length)
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
            x += 50
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