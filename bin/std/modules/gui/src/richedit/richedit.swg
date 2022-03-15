#global public
using Core, Pixel

#[Swag.EnumFlags]
enum RichEditFlags
{
    None
    ReadOnly
}

struct RichEdit
{
    using frameWnd: FrameWnd
    richEdFlags     = RichEditFlags.None
    lineSpacing:    f32
    overwriteMode:  bool
    sigIsReadOnly:  SigArray'closure(*bool)

    styles:         [256] RichEditCharStyle
    cursorPos:      RichEditCharPos
    selBeg:         RichEditCharPos
    lines:          ArrayPtr'RichEditLine
    freeLines:      ArrayPtr'RichEditLine
    scrollWnd:      *ScrollWnd
    edView:         *RichEditView
    hasSelRect:     bool

    undoMgr:            ArrayPtr'RichEditUndo
    toUndo:             ArrayPtr'RichEditUndo
    currentUndo:        uint
    undoCounter:        uint
    modifiedUndoMarker: uint
    serialUndo:         uint
    cursorPosUndo:      RichEditCharPos
    selBegUndo:         RichEditCharPos
}

private impl RichEdit
{
    mtd getMarginLeft()->f32
    {
        return padding.x
    }

    mtd setDefaultStyles()
    {
        colors := getThemeColors()
        loop i: @countof(styles)
            styles[i].colFg = colors.richEdit_Text
    }

    mtd clearPrivate()
    {
    }

    mtd resetModified()
    {
    }

    #[Swag.Discardable]
    mtd newLine(index: uint)->*RichEditLine
    {
        if freeLines.count
        {
            p := freeLines.popBack()
            return p
        }

        p := Memory.new'RichEditLine()
        lines.insertAt(index, p)
        return p
    }

    mtd getNextVisibleLine(lineIndex: uint)->uint
    {
        count := lines.count
        if lineIndex == count - 1
            return lineIndex

        i := lineIndex + 1
        loop
        {
            if lines[i].isVisible()
                return i
            if i == count - 1
                break
            i += 1
        }

        return lineIndex
    }

    mtd getRune(cp: RichEditCharPos)->rune
    {
        if cp.lineIndex >= lines.count
            return 0
        len := lines[cp.lineIndex].chars.count
        if cp.charPosInLine > len
            return 0
        if cp.charPosInLine == len
            return "\n"'rune;
        return lines[cp.lineIndex].chars[cp.charPosInLine]
    }

    mtd getTextPrivate(start, end: RichEditCharPos, selRect: bool)->Array'rune
    {
        var result: retval
        if !hasSelRect
        {
            // First line
            line := lines[start.lineIndex]
            count := line.length()
            if count
            {
                pchar := line.chars.buffer
                if start.lineIndex == end.lineIndex
                    count = end.charPosInLine
                for i := start.charPosInLine; i < count; i += 1
                    result.add(pchar[i])
            }

            if start.lineIndex != end.lineIndex
                result.add("\n"'rune)

            // Inside lines
            if end.lineIndex > start.lineIndex + 1
            {
                // Pre allocate string to optimize append
                count = 0
                for i := start.lineIndex + 1; i < end.lineIndex; i += 1
                    count += lines[i].length()
                result.reserve(result.count + count)

                for i := start.lineIndex + 1; i < end.lineIndex; i += 1
                {
                    result.add(lines[i].chars.toSlice())
                    result.add("\n"'rune)
                }
            }

            // Last line
            if start.lineIndex != end.lineIndex
            {
                line = lines[end.lineIndex]
                count = line.length()
                if count
                {
                    pchar := line.chars.buffer
                    for i := 0; i < end.charPosInLine; i += 1
                        result.add(pchar[i])
                }
            }
        }
        else
        {
            Debug.assert(false) // todo
        }

        return result
    }

    mtd insertTextPrivate(cp: *RichEditCharPos, text: string, selRect: bool)
    {
        if lines.isEmpty()
            newLine(0)

        if !selRect
        {
            // This is just an EOL
            singleEol := false;
            countOf   := @countof(text)
            if countOf == 1 and text[0] == "\n"'u8
                singleEol = true

            str  := text
            line := lines[cp.lineIndex]
            loop
            {
                (c, eat) := Utf8.decodeRune(str)
                if !eat
                    break
                str = Tokenize.eatCount(str, cast(uint) eat)

                if c == "\n"'rune or c == "\r"'rune
                {
                    cp.lineIndex += 1
                    if cp.lineIndex == lines.count
                    {
                        line = newLine(cp.lineIndex)
                        cp.charPosInLine = 0
                    }
                }
                else
                {
                    posIns := Math.min(cp.charPosInLine, line.length())
                    line.insertRune(posIns, c)
                    cp.charPosInLine += 1
                }
            }
        }
        else
        {
            Debug.assert(false) // todo
        }
    }

    mtd deleteTextPrivate(posStart, posEnd: RichEditCharPos, selRect: bool)
    {
    }
}

impl RichEdit
{
    #[Swag.Discardable]
    func create(parent: *Wnd, position: Math.Rectangle, id: string = null)->*RichEdit
    {
        ed := Wnd.create'RichEdit(parent, "", position, id)

        with ed
        {
            scrollWnd = createView'ScrollWnd()
            scrollWnd.dockStyle = .Center
            scrollWnd.focusStrategy = .None
            scrollWnd.backgroundStyle = .Transparent
            scrollWnd.setAllPaddings(getThemeMetrics().richEdit_Padding)

            edView = scrollWnd.createView'RichEditView()
            edView.backgroundStyle = .Transparent
            edView.ed = ed
            edView.wndFlags |= .NoScroll

            setDefaultStyles()
        }

        return ed
    }

    mtd isReadOnly()->bool
    {
        ro := false
        sigIsReadOnly.call(&ro)
        return ro or richEdFlags & .ReadOnly
    }

    // Set the edit text
    mtd setText(text: string)
    {
        if isReadOnly() return

        clearPrivate()

        var cp: RichEditCharPos
        insertTextPrivate(&cp, text, false)

        resetModified()
        invalidate()
    }
}