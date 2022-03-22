#global public
using Core, Pixel

#[Swag.EnumFlags]
enum RichEditFlags
{
    None
    ReadOnly
    ShowFocusSel
}

struct RichEditMapping
{
    key:        Input.Key = ?
    modifiers:  Input.KeyModifiers = ?
    command:    RichEditCommand = ?
}

struct RichEdit
{
    using frameWnd: FrameWnd
    fontFamily:     FontFamily
    richEdFlags     = RichEditFlags.None
    lineSpacing:    f32
    overwriteMode:  bool
    tabSize:        s32 = 4
    sigIsReadOnly:  SigArray'closure(*bool)
    sigChanged:     SigArray'closure(*Self)
    sigModified:    SigArray'closure(*Self)

    styles:         [256] RichEditRuneStyle
    stylesSelect:   [2] RichEditRuneStyle
    cursorPos:      RichEditCursor
    selBeg:         RichEditCursor
    lines:          ArrayPtr'RichEditLine
    freeLines:      ArrayPtr'RichEditLine
    scrollWnd:      *ScrollWnd
    edView:         *RichEditView
    selRectangle:   Math.Rectangle

    hasSelRect:                 bool
    modified:                   bool
    recomputeScroll:            bool
    dirtyFirstLineView:         bool
    forceCaretVisible:          bool
    forceCaretVisibleCenter:    bool

    undoMgr:            ArrayPtr'RichEditUndo
    toUndo:             ArrayPtr'RichEditUndo
    mapping:            Array'RichEditMapping
    currentUndo:            uint
    undoCounter:            uint
    modifiedUndoMarker:     uint
    serialUndo:             uint
    countVisibleLines:      uint
    firstVisibleLineView:   uint
    lastVisibleLineView:    uint
    cursorPosUndo:      RichEditCursor
    selBegUndo:         RichEditCursor
}

private impl RichEdit
{
    mtd getMarginLeft()->f32
    {
        return padding.x
    }

    mtd getMarginRight()->f32
    {
        return padding.z
    }

    mtd setDefaultStyles()
    {
        colors := getThemeColors()
        loop i: @countof(styles)
            styles[i].colFg = colors.richEdit_Text

        stylesSelect[0].colFg = colors.richEdit_SelText
        stylesSelect[0].colBk = colors.richEdit_SelBk
        stylesSelect[1].colFg = colors.richEdit_SelTextNoFocus
        stylesSelect[1].colBk = colors.richEdit_SelBkNoFocus
    }

    mtd computeScrollY()
    {
        size := getXExtent()
        if !recomputeScroll
            return

        y := 0'f32
        firstHidden := true
        countVisibleLines = 0
        loop i: lines
        {
            lines[i].drawingRect.clear()

            if lines[i].isVisible()
            {
                countVisibleLines += 1
                lines[i].drawingRect.y = y
                lines[i].drawingRect.height = size.y
                y += size.y
                firstHidden = true
            }
            elif firstHidden
            {
                countVisibleLines += 1
                lines[i].drawingRect.y = y
                lines[i].drawingRect.height = size.y
                y += size.y
                firstHidden = false
            }
        }

        sy := countVisibleLines * size.y
    }

    mtd setModified(mdf: bool)
    {
        if mdf
            sigChanged.call(self)
        if modified == mdf
            return
        modified = mdf
        sigModified.call(self)
    }

    mtd resetModified()
    {
        setModified(false)
        modifiedUndoMarker = currentUndo
    }

    #[Swag.Discardable]
    mtd newLine(index: uint)->*RichEditLine
    {
        var p: *RichEditLine

        if freeLines.count
            p = freeLines.popBack()
        else
            p = Memory.new'RichEditLine()

        p.resetInternal()
        lines.insertAt(index, p)
        recomputeScroll = true
        return p
    }

    mtd deleteLine(lineIndex: uint)
    {
        freeLines.add(lines[lineIndex])
        lines.removeAtOrdered(lineIndex)
        recomputeScroll = true
    }

    mtd getFirstVisibleLine()->uint
    {
        loop i: lines.count
        {
            if lines[i].isVisible()
                return i
        }

        return 0
    }

    mtd getLastVisibleLine()->uint
    {
        if !lines.count
            return 0
        for i := lines.count - 1; i >= 0; i -= 0
        {
            if lines[i].isVisible()
                return i
        }

        return 0
    }

    mtd getPreviousVisibleLine(lineIndex: uint)->uint
    {
        if !lineIndex
            return 0
        i := lineIndex - 1
        loop
        {
            if lines[i].isVisible()
                return i
            if !i
                break
            i -= 1
        }

        return lineIndex
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
}

impl RichEdit
{
    #[Swag.Discardable]
    func create(parent: *Wnd, position: Math.Rectangle, id: string = null)->*RichEdit
    {
        ed := Wnd.create'RichEdit(parent, "", position, id)
        ed.frameWnd.style.addStyleSheetColors("frameWnd_FocusBk $richEdit_Bk")
        ed.frameWnd.style.addStyleSheetColors("frameWnd_Bk $richEdit_Bk")
        ed.fontFamily = parent.getTheme().res.fontDefaultFs

        with ed
        {
            scrollWnd = createView'ScrollWnd()
            scrollWnd.dockStyle = .Center
            scrollWnd.focusStrategy = .None
            scrollWnd.backgroundStyle = .Transparent
            scrollWnd.setAllPaddings(getThemeMetrics().richEdit_Padding)
            scrollWnd.sigScrollPosChanged += @|ed|(scrollWnd, op, np) {
                if op.y != np.y
                    ed.dirtyFirstLineView = true
            }
            scrollWnd.style.addStyleSheetColors("scrollBar_Bk $richEdit_Bk")

            edView = scrollWnd.createView'RichEditView()
            edView.backgroundStyle = .Transparent
            edView.ed = ed
            edView.wndFlags |= .NoScroll

            setDefaultMapping()
            setDefaultStyles()
        }

        return ed
    }

    // Returns true if the rich edit is read only
    mtd isReadOnly()->bool
    {
        ro := false
        sigIsReadOnly.call(&ro)
        return ro or richEdFlags & .ReadOnly
    }

    // Simulate a keyboard character pressed
    mtd charPressed(key: Input.Key, ch: rune, mdf: Input.KeyModifiers)->bool
    {
        cmd := getMappingCommand(key, mdf)
        if cmd != .None
            return true

        c := ch
        if c == "\r"'rune
            c = "\n"'rune

        insertRune(c)
        ensureCaretIsVisible()
        return true
    }

    // Simulate a keyboard pressed
    mtd keyPressed(key: Input.Key, mdf: Input.KeyModifiers)->bool
    {
        cmd := getMappingCommand(key, mdf)
        if cmd == .None
            return true
        executeCommand(cmd)
        ensureCaretIsVisible()
        return true
    }
}