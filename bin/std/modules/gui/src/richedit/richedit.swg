#global public
using Core, Pixel

#[Swag.EnumFlags]
enum RichEditFlags
{
    None
    ReadOnly
    ShowFocusSel
    CurLineBorder
    DrawBlanks
    TabToSpaces
    WordWrap
    AutoLoseFocus
}

enum RichEditForm
{
    RichEditCtrl
    Edit
}

struct RichEditMapping
{
    key:        Input.Key = undefined
    modifiers:  Input.KeyModifiers = undefined
    command:    RichEditCommand = undefined
}

struct RichEditCtrl
{
    using frameWnd: FrameWnd
    richEdFlags     = RichEditFlags.None
    lineSpacing:    f32
    overwriteMode:  bool
    tabSize:        s32 = 4
    marginLeft:     f32
    marginRight:    f32
    lexer:          IRichEditLexer
    form            = RichEditForm.RichEditCtrl
    sigIsReadOnly:  SigArray'closure(*bool)
    sigChanged:     SigArray'closure(*Self)
    sigModified:    SigArray'closure(*Self)
    bkColorModel:   String

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

    undoMgr:                ArrayPtr'RichEditUndo
    toUndo:                 ArrayPtr'RichEditUndo
    mapping:                Array'RichEditMapping
    currentUndo:            u64
    undoCounter:            u64
    modifiedUndoMarker:     u64
    serialUndo:             u64
    countVisibleLines:      u64
    firstVisibleLineView:   u64
    lastVisibleLineView:    u64
    clipFormat:             Clipboard.Format
    cursorPosUndo:          RichEditCursor
    selBegUndo:             RichEditCursor
}

private impl RichEditCtrl
{
    mtd getMarginLeft()->f32
    {
        return marginLeft
    }

    mtd getMarginRight()->f32
    {
        return marginRight
    }

    mtd setDefaultStyles()
    {
        colors := getThemeColors()
        loop i: @countof(styles)
            styles[i].colFg = colors.richEdit_Text
        setForm(.RichEditCtrl)
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
    mtd newLine(index: u64)->*RichEditLine
    {
        var p: *RichEditLine

        if freeLines.count
            p = freeLines.popBack()
        else
            p = Memory.new'RichEditLine()

        p.resetInternal()

        if lines.count
        {
            insLine := index == lines.count ? lines.back() : lines[index]
            p.drawingRect = insLine.drawingRect
            p.drawingRect.y += p.drawingRect.height
        }

        lines.insertAt(index, p)
        recomputeScroll = true

        return p
    }

    mtd deleteLine(lineIndex: u64)
    {
        freeLines.add(lines[lineIndex])
        lines.removeAtOrdered(lineIndex)
        recomputeScroll = true
    }

    mtd getFirstVisibleLine()->u64
    {
        loop i: lines.count
        {
            if lines[i].isVisible()
                return i
        }

        return 0
    }

    mtd getLastVisibleLine()->u64
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

    mtd getPreviousVisibleLine(lineIndex: u64)->u64
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

    mtd getNextVisibleLine(lineIndex: u64)->u64
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

impl RichEditCtrl
{
    #[Swag.Discardable]
    func create(parent: *Wnd, position: Math.Rectangle = {}, id: WndId = null)->*RichEditCtrl
    {
        pos := position
        if pos.height == 0
            pos.height = parent.getThemeMetrics().editBox_Height
        if pos.width == 0
            pos.width = parent.getThemeMetrics().editBox_Width

        ed := Wnd.create'RichEditCtrl(parent, "", pos, id)

        with ed
        {
            .scrollWnd = .createView'ScrollWnd()
            .scrollWnd.dockStyle = .Center
            .scrollWnd.focusStrategy = .None
            .scrollWnd.setAllPaddings(.getThemeMetrics().richEdit_Padding)
            .scrollWnd.sigScrollPosChanged += closure|ed|(scrollWnd, op, np) {
                if op.y != np.y
                    ed.dirtyFirstLineView = true
            }

            .edView = .scrollWnd.createView'RichEditView()
            .edView.ed = ed
            .edView.wndFlags |= .NoScroll

            .setDefaultMapping()
            .setDefaultStyles()
        }

        return ed
    }

    mtd setFocus()
    {
        edView.setFocus()
    }

    // Set richedit flags
    mtd setRichEdFlags(flags: RichEditFlags)
    {
        richEdFlags = flags
        recomputeScroll = true
        invalidate()
    }

    // Set form
    mtd setForm(form: RichEditForm)
    {
        self.form = #self.form
        colors := getThemeColors()

        switch self.form
        {
        case RichEditCtrl:
            font = parent.getTheme().res.fontDefaultFs

            mdlBk := bkColorModel
            if bkColorModel.length == 0
                mdlBk = String.from("$richEdit_Bk")

            frameWnd.style.clearStyleSheetColors()
            scrollWnd.style.clearStyleSheetColors()

            frameWnd.style.addStyleSheetColors(Format.toString("frameWnd_FocusBk %", mdlBk))
            frameWnd.style.addStyleSheetColors(Format.toString("frameWnd_Bk %", mdlBk))
            scrollWnd.style.addStyleSheetColors(Format.toString("scrollBar_Bk %", mdlBk))

            stylesSelect[0].colFg = colors.richEdit_SelText
            stylesSelect[0].colBk = colors.richEdit_SelBk
            stylesSelect[1].colFg = colors.richEdit_SelTextNoFocus
            stylesSelect[1].colBk = colors.richEdit_SelBkNoFocus

        case Edit:
            font = parent.getTheme().res.fontDefault

            mdlBk := bkColorModel
            if bkColorModel.length == 0
                mdlBk = String.from("$edit_Bk")

            frameWnd.style.clearStyleSheetColors()
            scrollWnd.style.clearStyleSheetColors()

            frameWnd.style.addStyleSheetColors(Format.toString("frameWnd_FocusBk %", mdlBk))
            frameWnd.style.addStyleSheetColors(Format.toString("frameWnd_Bk %", mdlBk))
            scrollWnd.style.addStyleSheetColors(Format.toString("scrollBar_Bk %", mdlBk))

            stylesSelect[0].colFg = colors.edit_SelectedText
            stylesSelect[0].colBk = colors.edit_SelectedBk
            stylesSelect[1].colFg = 0
            stylesSelect[1].colBk = 0
        }

        invalidate()
    }

    // Returns true if the rich edit is read only
    mtd isReadOnly()->bool
    {
        ro := false
        sigIsReadOnly.call(&ro)
        return ro or richEdFlags & .ReadOnly
    }

    // Simulate a keyboard character pressed
    mtd charPressed(key: Input.Key, ch: rune, mdf: Input.KeyModifiers)
    {
        isChar := false
        if mdf == .None and ch >= "0"'rune and ch <= "9"'rune
            isChar = true

        // If there is a corresponding command, do nothing
        if !isChar
        {
            cmd := getMappingCommand(key, mdf)
            if cmd != .None
                return
        }

        if isReadOnly()
            return

        c := ch
        if c == "\r"'rune
            c = "\n"'rune

        if c == "\n"'rune and mdf == .None and richEdFlags & .AutoLoseFocus
            return

        if lexer and lexer.insertRune(self, c)
            return

        insertRune(c)
        forceCaretVisible = true
    }

    // Simulate a keyboard pressed
    mtd keyPressed(key: Input.Key, mdf: Input.KeyModifiers)
    {
        cmd := getMappingCommand(key, mdf)
        if cmd == .None
            return
        executeCommand(cmd)
        forceCaretVisible = true
        invalidate()
    }

    // Associate a lexer
    mtd setLexer(lex: IRichEditLexer)
    {
        lexer = lex
        lexer.setup(self)
    }
}