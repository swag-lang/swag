#global public
using Core

struct RichChunk
{
    // From 'set'
    slice:      string
    fontStyle:  FontFamilyStyle = Regular
    isSpace:    bool
    isEol:      bool
    colorIdx:   s32

    // From 'computeLayout'
    boundRect:  Math.Rectangle
    pos:        Math.Point
    font:       *Font
    under:      UnderlineStyle = None
}

struct RichString
{
    raw:        String
    chunks:     Array'RichChunk
    layoutRect: Math.Rectangle
    boundRect:  Math.Rectangle
}

impl RichString
{
    mtd opEquals(other: string)->bool
    {
        return raw == other
    }

    // We need to recompute chunks, as they contains slices
    mtd opPostCopy()
    {
        chunks.clear()
        set(raw)
    }

    #[Swag.Mixin]
    private func addChunk(idx: uint, space, eol: bool = false)
    {
        if idx - startChunk == 0
            return
        chunk.isSpace = space
        chunk.isEol   = eol
        chunk.colorIdx = colorFg
        chunk.under    = under
        if isBold and isItalic
            chunk.fontStyle = .BoldItalic
        elif isBold
            chunk.fontStyle = .Bold
        elif isItalic
            chunk.fontStyle = .Italic
        else
            chunk.fontStyle = .Default
        chunk.slice = @mkstring(raw.buffer + startChunk, idx - startChunk)
        chunks.add(chunk)
        startChunk = idx
    }

    #[Swag.Inline]
    mtd opCount() => raw.length
    #[Swag.Inline]
    mtd opAffect(str: string) = set(str)

    #[Swag.Inline]
    mtd isEmpty() => raw.length == 0

    // Force to be repainted
    mtd invalidate()
    {
        layoutRect.clear()
    }

    // Associate a string (can have multiple lines)
    // Accepted markers are :
    // <u>  ... <u>  => underline
    // <b>  ... <b>  => bold
    // <i>  ... <i>  => italic
    // <p1> ... <p1> => color palette 1
    // <p2> ... <p2> => color palette 2
    // <p3> ... <p3> => color palette 3
    mtd set(text: string)
    {
        if raw == text and chunks.count
            return
        raw = text

        chunks.clear()
        layoutRect.clear()

        count := raw.length
        if !count
            return

        startChunk, endChunk := 0'uint
        var chunk: RichChunk

        byteIdx := 0'uint
        isBold, isItalic := false
        colorFg := 0
        under := UnderlineStyle.None

        scan := raw.buffer
        while count
        {
            sl := @mkslice(scan, count)
            switch
            {
            case Utf8.startsWith(sl, "<u>"):
                addChunk(byteIdx)
                under = under == .None ? .Underline : .None
                byteIdx, scan, startChunk += 3
                count -= 3
                continue
            case Utf8.startsWith(sl, "<b>"):
                addChunk(byteIdx)
                isBold = !isBold
                byteIdx, scan, startChunk += 3
                count -= 3
                continue
            case Utf8.startsWith(sl, "<i>"):
                addChunk(byteIdx)
                isItalic = !isItalic
                byteIdx, scan, startChunk += 3
                count -= 3
                continue
            case Utf8.startsWith(sl, "<p1>"):
                addChunk(byteIdx)
                colorFg = colorFg == 0 ? 1 : 0
                byteIdx, scan, startChunk += 4
                count -= 4
                continue
            case Utf8.startsWith(sl, "<p2>"):
                addChunk(byteIdx)
                colorFg = colorFg == 0 ? 2 : 0
                byteIdx, scan, startChunk += 4
                count -= 4
                continue
            case Utf8.startsWith(sl, "<p3>"):
                addChunk(byteIdx)
                colorFg = colorFg == 0 ? 3 : 0
                byteIdx, scan, startChunk += 4
                count -= 4
                continue
            }

            (c, eat) := Utf8.decodeRune(sl)
            if eat == 0
                break

            if c == "\n"
            {
                addChunk(byteIdx)
                byteIdx, scan += eat
                addChunk(byteIdx, eol: true)
                count -= 1
                continue
            }

            if Unicode.isSpace(c)
            {
                addChunk(byteIdx)
                while eat and Unicode.isSpace(c)
                {
                    byteIdx, scan += eat
                    count -= eat
                    sl = @mkslice(scan, count)
                    (c, eat) = Utf8.decodeRune(sl)
                }

                addChunk(byteIdx, space: true)
                continue
            }

            byteIdx, scan += eat
            count -= eat
        }

        // Remaining chunk
        Debug.assert(startChunk <= raw.length)
        if startChunk < raw.length
            addChunk(raw.length)
    }
}