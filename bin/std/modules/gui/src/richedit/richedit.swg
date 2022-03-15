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
    sigIsReadOnly:  SigArray'closure(*bool)

    styles:     [256] RichEditCharStyle
    lines:      ArrayPtr'RichEditLine
    freeLines:  ArrayPtr'RichEditLine
    scrollWnd:  *ScrollWnd
    edView:     *RichEditView
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

    mtd insertTextPrivate(cp: RichEditCharPos, text: string, selRect: bool)
    {
        if lines.isEmpty()
            newLine(0)

        if !selRect
        {
            lineIndex := cp.lineIndex
            posInLine := cp.charPosInLine

            // This is just an EOL
            singleEol := false;
            countOf   := @countof(text)
            if countOf == 1 and text[0] == "\n"'u8
                singleEol = true

            str  := text
            line := lines[lineIndex]
            loop
            {
                (c, eat) := Utf8.decodeRune(str)
                if !eat
                    break
                str = Tokenize.eatCount(str, cast(uint) eat)

                if c == "\n"'rune or c == "\r"'rune
                {
                    lineIndex += 1
                    if lineIndex == lines.count
                    {
                        line = newLine(lineIndex)
                        posInLine = 0
                    }
                }
                else
                {
                    posIns := Math.min(posInLine, line.length())
                    line.insertRune(posIns, c)
                    posInLine += 1
                }
            }
        }
        else
        {
            Debug.assert(false) // todo
        }
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
        insertTextPrivate(@{}, text, false)
        resetModified()
        invalidate()
    }
}