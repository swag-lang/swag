#global public
using Core, Pixel

struct RichEdit
{
    using frameWnd: FrameWnd

    lines:      ArrayPtr'RichEditLine
    freeLines:  ArrayPtr'RichEditLine
    scrollWnd:  *ScrollWnd
    edView:     *RichEditView
}

struct CharPos
{
    lineIndex:      uint
    charPosInLine:  uint
}

struct OneStyle
{
    colBk:      Color
    colFg:      Color
    fontMode    = FontFamilyStyle.Regular
}

private impl RichEdit
{
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

    mtd insertTextPrivate(charPos: CharPos, text: string, selRect: bool)
    {
        if lines.isEmpty()
            newLine(0)

        if !selRect
        {
            lineIndex := charPos.lineIndex
            posInLine := charPos.charPosInLine

            // This is just an EOL
            singleEol := false;
            countOf   := @countof(text)
            if countOf == 1 and text[0] == "\n"'u8
                singleEol = true

            rankBeg := 0
            rankEnd := 0
            loop
            {
                line := lines[lineIndex]
                if !text[rankEnd] or text[rankEnd] == "\n"'u8 or text[rankEnd] == "\r"'u8
                {
                    posIns := posInLine
                    if posIns > line.length()
                        posIns = line.length()

                    line.insertText(posIns, @mkstring(@dataof(text) + rankBeg, rankEnd - rankBeg))
                    if !text[rankEnd] or !text[rankEnd + 1]
                        break

                    lineIndex += 1
                    if lineIndex == lines.count
                    {
                        newLine(lineIndex)
                        posInLine = 0
                    }

                    rankEnd += 1
                    rankBeg = rankEnd
                    continue
                }

                if !text[rankEnd]
                    break
                rankEnd += 1
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
        }

        return ed
    }

    mtd isReadOnly() => false

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