#global public
using Core

struct RichChunk
{
    slice:            string
    fontStyle:        FontFamilyStyle = Regular
    hasFontStyle:     bool
    isSpace:          bool
    isEol:            bool
    isUrl:            bool
    isHotUrl:         bool
    colorIdx:         s32

    boundRect:        Math.Rectangle
    pos:              Math.Point
    font:             *Font
    under:            UnderlineStyle = None
}

// Represents a string with embedded formats.
// This kind of string is a specific version of [Core.String] but specific for painting. It can contains
// some special markers to change it's appearence.
// ```swag
// <b>this is bold</b>
// <i>this is italic</i>
// <p1>this will have special color 1</p>
// <p2>this will have special color 2</p>
// <p3>this will have special color 3</p>
// ...
// ```swag
// See [[RichString.set]] for a list of available markers.
struct RichString
{
    raw:            String              // The string as passed by the user
    chunks:         Array'RichChunk     // The corresponding list of chunks
    layoutRect:     Math.Rectangle
    boundRect:      Math.Rectangle
    hasUrl:         bool
    dirty:          bool                // If dirty, 'Compute'
}

impl RichString
{
    mtd opEquals(other: string)->bool
    {
        return .raw == other
    }

    // We need to recompute chunks, as they contains slices
    mtd opPostCopy()
    {
        .chunks.clear()
        .set(.raw)
    }

    #[Swag.Mixin]
    internal mtd addChunk(idx: u64, space, eol, url: bool = false)
    {
        if idx - startChunk == 0 do
            return

        chunk.isSpace      = space
        chunk.isEol        = eol
        chunk.isUrl        = url
        chunk.colorIdx     = colorFg.back()
        chunk.under        = under.back()
        chunk.hasFontStyle = false

        if url do
            .hasUrl = true

        if isBold.back() and isItalic.back()
        {
            chunk.hasFontStyle = true
            chunk.fontStyle    = .BoldItalic
        }
        elif isBold.back()
        {
            chunk.hasFontStyle = true
            chunk.fontStyle    = .Bold
        }
        elif isItalic.back()
        {
            chunk.hasFontStyle = true
            chunk.fontStyle    = .Italic
        }

        chunk.slice = @mkstring(.raw.buffer + startChunk, idx - startChunk)
        .chunks.add(chunk)
        startChunk = idx
    }

    #[Swag.Inline]
    mtd opCount() => .raw.length
    #[Swag.Inline]
    mtd opAffect(str: string) => .set(str)

    // Returns 'true' if the string is empty.
    #[Swag.Inline]
    mtd isEmpty() => .raw.length == 0

    // Force the string to be repainted.
    mtd invalidate()
    {
        .layoutRect.clear()
    }

    // Associate a string.
    // The string can have multiple lines.
    // Accepted markers are:
    // * `<u>` => underline
    // * `<b>` => bold
    // * `<i>` => italic
    // * `<p1>` => color palette 1
    // * `<p2>` => color palette 2
    // * `<p3>` => color palette 3
    // * `<url>` => clickable
    // ```swag
    // <b>this is bold</b> but this is normal <i>and this is italic</i>
    // ```swag
    mtd set(text: #null string)
    {
        if .raw == text and .chunks.count do
            return
        .raw   = text
        .dirty = true
    }

    // Transform the given string to a naked one.
    // Will return the string without the format markers.
    func getNaked(str: string)->String
    {
        var result: retval

        var scan  = @dataof(str)
        var count = @countof(str)
        result.reserve(count)

        while count
        {
            let sl = @mkslice(scan, count)
            switch
            {
            case Utf8.startsWith(sl, "<u>"), Utf8.startsWith(sl, "<b>"), Utf8.startsWith(sl, "<i>"):
                scan += 3
                count -= 3
                continue
            case Utf8.startsWith(sl, "</u>"), Utf8.startsWith(sl, "</b>"), Utf8.startsWith(sl, "</i>"), Utf8.startsWith(sl, "</p>"):
                scan += 4
                count -= 4
                continue
            case Utf8.startsWith(sl, "<p1>"), Utf8.startsWith(sl, "<p2>"), Utf8.startsWith(sl, "<p3>"):
                scan += 4
                count -= 4
                continue
            case Utf8.startsWith(sl, "<url>"):
                scan += 5
                count -= 5
                continue
            case Utf8.startsWith(sl, "</url>"):
                scan += 6
                count -= 6
                continue
            }

            result += dref scan
            scan += 1
            count -= 1
        }

        return result
    }

    // Get the url chunk at position 'at'.
    // If an url does not exists, returns 'null'.
    mtd getUrlChunkAt(at: Math.Point)->#null *RichChunk
    {
        if !.hasUrl do
            return null
        foreach &c in .chunks
        {
            if !c.isUrl do
                continue

            var r = c.boundRect
            r.offset(c.pos)
            r.scale(1)

            if r.contains(at) do
                return c
        }

        return null
    }

    // Compute layout.
    // This will transform the string in a serie of chunks, each chunk having its one style. The string
    // will only be recomputed if it's dirty (the text has changed) or if 'force' is true.
    mtd compute(force = false)
    {
        if !.dirty or force do
            return

        .dirty  = false
        .hasUrl = false
        .chunks.clear()
        .layoutRect.clear()

        var count = .raw.length
        if !count do
            return

        var startChunk = 0'u64
        var chunk:     RichChunk

        var byteIdx   = 0'u64
        var isBold:   Array'bool
        var isItalic: Array'bool
        var colorFg:  Array'u32
        var under:    Array'UnderlineStyle

        isBold.add(false)
        isItalic.add(false)
        colorFg.add(0)
        under.add(UnderlineStyle.None)

        var scan = .raw.buffer
        while count
        {
            var sl = @mkslice(scan, count)
            switch
            {
            case sl[0] != '<':
                break
            case Utf8.startsWith(sl, "<u>"):
                .addChunk(byteIdx)
                under.add(UnderlineStyle.Underline)
                byteIdx, scan, startChunk += 3
                count -= 3
                continue
            case Utf8.startsWith(sl, "</u>"):
                .addChunk(byteIdx)
                if under.count > 1 do
                    under.removeBack()
                byteIdx, scan, startChunk += 4
                count -= 4
                continue

            case Utf8.startsWith(sl, "<b>"):
                .addChunk(byteIdx)
                isBold.add(true)
                byteIdx, scan, startChunk += 3
                count -= 3
                continue
            case Utf8.startsWith(sl, "</b>"):
                .addChunk(byteIdx)
                if isBold.count > 1 do
                    isBold.removeBack()
                byteIdx, scan, startChunk += 4
                count -= 4
                continue

            case Utf8.startsWith(sl, "<i>"):
                .addChunk(byteIdx)
                isItalic.add(true)
                byteIdx, scan, startChunk += 3
                count -= 3
                continue
            case Utf8.startsWith(sl, "</i>"):
                .addChunk(byteIdx)
                if isItalic.count > 1 do
                    isItalic.removeBack()
                byteIdx, scan, startChunk += 4
                count -= 4
                continue

            case Utf8.startsWith(sl, "<p1>"):
                .addChunk(byteIdx)
                colorFg.add(1)
                byteIdx, scan, startChunk += 4
                count -= 4
                continue
            case Utf8.startsWith(sl, "<p2>"):
                .addChunk(byteIdx)
                colorFg.add(2)
                byteIdx, scan, startChunk += 4
                count -= 4
                continue
            case Utf8.startsWith(sl, "<p3>"):
                .addChunk(byteIdx)
                colorFg.add(3)
                byteIdx, scan, startChunk += 4
                count -= 4
                continue

            case Utf8.startsWith(sl, "</p>"):
                .addChunk(byteIdx)
                if colorFg.count > 1 do
                    colorFg.removeBack()
                byteIdx, scan, startChunk += 4
                count -= 4
                continue

            case Utf8.startsWith(sl, "<url>"):
                .addChunk(byteIdx)
                byteIdx, scan, startChunk += 5
                count -= 5
                continue
            case Utf8.startsWith(sl, "</url>"):
                .addChunk(byteIdx, url: true)
                byteIdx, scan, startChunk += 6
                count -= 6
                continue
            }

            var (c, eat) = Utf8.decodeRune(sl)
            if eat == 0 do
                break

            if c == "\n"
            {
                .addChunk(byteIdx)
                byteIdx, scan += eat
                .addChunk(byteIdx, eol: true)
                count -= 1
                continue
            }

            if Unicode.isSpace(c)
            {
                .addChunk(byteIdx)
                while eat and Unicode.isSpace(c)
                {
                    byteIdx, scan += eat
                    count -= eat
                    sl = @mkslice(scan, count)
                    (c, eat) = Utf8.decodeRune(sl)
                }

                .addChunk(byteIdx, space: true)
                continue
            }

            byteIdx, scan += eat
            count -= eat
        }

        // Remaining chunk
        Debug.assert(startChunk <= .raw.length)
        if startChunk < .raw.length do
            .addChunk(.raw.length)
    }
}
