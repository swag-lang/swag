#global public
using Core

struct RichChunk
{
    // From 'set'
    slice:      string
    fontStyle:  FontFamilyStyle = Regular
    isSpace:    bool
    colorIdx:   s32

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
            chunk.isSpace  = false
            chunk.colorIdx = colorFg
            if cptBold > 0 and cptItalic > 0
                chunk.fontStyle = .BoldItalic
            else if cptBold > 0
                chunk.fontStyle = .Bold
            else if cptItalic > 0
                chunk.fontStyle = .Italic
            else
                chunk.fontStyle = .Regular
            chunk.slice = @mkstring(raw.buffer + startChunk, idx - startChunk)
            chunks.add(chunk)
            startChunk = byteIdx
        }

        startChunk, endChunk := 0'uint
        var chunk: RichChunk

        byteIdx := 0'uint
        cptBold, cptItalic := 0
        colorFg := 0

        scan := raw.buffer
        while count
        {
            sl := @mkslice(scan, count)
            switch
            {
            case Utf8.startsWith(sl, "<b>"):
                addChunk(byteIdx)
                cptBold += 1
                byteIdx, scan, startChunk += 3
                count -= 3
                continue
            case Utf8.startsWith(sl, "</b>"):
                addChunk(byteIdx)
                if cptBold cptBold -= 1
                byteIdx, scan, startChunk += 4
                count -= 4
                continue
            case Utf8.startsWith(sl, "<i>"):
                addChunk(byteIdx)
                cptItalic += 1
                byteIdx, scan, startChunk += 3
                count -= 3
                continue
            case Utf8.startsWith(sl, "</i>"):
                addChunk(byteIdx)
                if cptItalic cptItalic -= 1
                byteIdx, scan, startChunk += 4
                count -= 4
                continue
            case Utf8.startsWith(sl, "<p1>"):
                addChunk(byteIdx)
                colorFg = 1
                byteIdx, scan, startChunk += 4
                count -= 4
                continue
            case Utf8.startsWith(sl, "</p1>"):
                addChunk(byteIdx)
                colorFg = 0
                byteIdx, scan, startChunk += 5
                count -= 5
                continue
            }

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
}