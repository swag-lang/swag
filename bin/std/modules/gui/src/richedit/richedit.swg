#global public
using Core, Pixel

#[Swag.EnumFlags]
enum RichEditFlags
{
    Zero          = 0
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
    key:           Input.Key = undefined
    modifiers:     Input.KeyModifiers = undefined
    command:       RichEditCommand = undefined
}

struct RichEditCtrl
{
    using frameWnd:              FrameWnd
    richEdFlags                  = RichEditFlags.Zero
    lineSpacing:                 f32
    overwriteMode:               bool
    tabSize:                     s32 = 4
    marginLeft:                  f32
    marginRight:                 f32
    lexer:                       IRichEditLexer
    form                         = RichEditForm.RichEditCtrl
    sigIsReadOnly:               SigArray'func||(*bool)
    sigChanged:                  SigArray'func||(*RichEditCtrl)
    sigModified:                 SigArray'func||(*RichEditCtrl)
    bkColorModel:                String

    styles:                      [256] RichEditRuneStyle
    stylesSelect:                [2] RichEditRuneStyle
    cursorPos:                   RichEditCursor
    selBeg:                      RichEditCursor
    lines:                       ArrayPtr'RichEditLine
    freeLines:                   ArrayPtr'RichEditLine
    scrollWnd:                   *ScrollWnd
    edView:                      *RichEditView
    selRectangle:                Math.Rectangle

    hasSelRect:                  bool
    modified:                    bool
    recomputeScroll:             bool
    dirtyFirstLineView:          bool
    forceCaretVisible:           bool
    forceCaretVisibleCenter:     bool

    undoMgr:                     ArrayPtr'RichEditUndo
    toUndo:                      ArrayPtr'RichEditUndo
    mapping:                     Array'RichEditMapping
    currentUndo:                 u64
    undoCounter:                 u64
    modifiedUndoMarker:          u64
    serialUndo:                  u64
    countVisibleLines:           u64
    firstVisibleLineView:        u64
    lastVisibleLineView:         u64
    clipFormat:                  Clipboard.ValueFormat
    cursorPosUndo:               RichEditCursor
    selBegUndo:                  RichEditCursor
}

internal impl IWnd for RichEditCtrl
{
    mtd impl onNotifyEvent(evt: *NotifyEvent)
    {
        if evt.ntfyKind == .ThemeChanged
        {
            .setDefaultStyles()
            if .lexer do
                .lexer.setupTheme(me)
        }
    }
}

internal impl RichEditCtrl
{
    mtd getMarginLeft()->f32
    {
        return .marginLeft
    }

    mtd getMarginRight()->f32
    {
        return .marginRight
    }

    mtd setDefaultStyles()
    {
        let colors = .getThemeColors()
        for i in @countof(.styles) do
            .styles[i].colFg = colors.richEdit_Text
        .setForm(.RichEditCtrl)
    }

    mtd computeScrollY()
    {
        let size = .getXExtent()
        if !.recomputeScroll do
            return

        var y           = 0'f32
        var firstHidden = true
        .countVisibleLines = 0
        for i in .lines
        {
            .lines[i].drawingRect.clear()

            if .lines[i].isVisible()
            {
                .countVisibleLines += 1
                .lines[i].drawingRect.y      = y
                .lines[i].drawingRect.height = size.y
                y += size.y
                firstHidden = true
            }
            elif firstHidden
            {
                .countVisibleLines += 1
                .lines[i].drawingRect.y      = y
                .lines[i].drawingRect.height = size.y
                y += size.y
                firstHidden = false
            }
        }
    }

    mtd setModified(mdf: bool)
    {
        if mdf do
            .sigChanged.call(me)
        if .modified == mdf do
            return
        .modified = mdf
        .sigModified.call(me)
    }

    mtd resetModified()
    {
        .setModified(false)
        .modifiedUndoMarker = .currentUndo
    }

    #[Swag.Discardable]
    mtd newLine(index: u64)->*RichEditLine
    {
        var p: *RichEditLine

        if .freeLines.count do
            p = .freeLines.popBack()
        else do
            p = Memory.new'RichEditLine()

        p.resetInternal()

        if .lines.count
        {
            let insLine = index == .lines.count ? .lines.back() : .lines[index]
            p.drawingRect = insLine.drawingRect
            p.drawingRect.y += p.drawingRect.height
        }

        .lines.insertAt(index, p)
        .recomputeScroll = true

        return p
    }

    #[Swag.Overload]
    mtd deleteLine(lineIndex: u64)
    {
        .freeLines.add(.lines[lineIndex])
        .lines.removeAtOrdered(lineIndex)
        .recomputeScroll = true
    }

    mtd getFirstVisibleLine()->u64
    {
        for i in .lines.count where .lines[i].isVisible() do
            return i
        return 0
    }

    mtd getLastVisibleLine()->u64
    {
        if !.lines.count do
            return 0
        for var i = .lines.count - 1; i >= 0; i -= 0
        {
            if .lines[i].isVisible() do
                return i
        }

        return 0
    }

    mtd getPreviousVisibleLine(lineIndex: u64)->u64
    {
        if !lineIndex do
            return 0
        var i = lineIndex - 1
        for
        {
            if .lines[i].isVisible() do
                return i
            if !i do
                break
            i -= 1
        }

        return lineIndex
    }

    mtd getNextVisibleLine(lineIndex: u64)->u64
    {
        let count = .lines.count
        if lineIndex == count - 1 do
            return lineIndex

        var i = lineIndex + 1
        for
        {
            if .lines[i].isVisible() do
                return i
            if i == count - 1 do
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
        var pos = position
        if pos.height == 0 do
            pos.height = parent.getThemeMetrics().editBox_Height
        if pos.width == 0 do
            pos.width = parent.getThemeMetrics().editBox_Width

        let ed = Wnd.create'RichEditCtrl(parent, "", pos, id)

        with ed
        {
            .scrollWnd               = .createView'ScrollWnd()
            .scrollWnd.dockStyle     = .Center
            .scrollWnd.focusStrategy = .None
            .scrollWnd.setAllPaddings(.getThemeMetrics().richEdit_Padding)
            .scrollWnd.sigScrollPosChanged += func|ed|(scrollWnd, op, np)
            {
                if op.y != np.y do
                    ed.dirtyFirstLineView = true
            }

            .edView    = .scrollWnd.createView'RichEditView()
            .edView.ed = ed
            .edView.wndFlags |= .NoScroll

            .setDefaultMapping()
            .setDefaultStyles()
        }

        return ed
    }

    mtd setFocus()
    {
        .edView.setFocus()
    }

    // Set richedit flags
    mtd setRichEdFlags(flags: RichEditFlags)
    {
        .richEdFlags.set(flags)
        .recomputeScroll = true
        .invalidate()
    }

    // Set form
    mtd setForm(form: RichEditForm)
    {
        .form = form
        let colors = .getThemeColors()

        switch .form
        {
        case RichEditCtrl:
            .style.setFont(.parent.getTheme().res.fontDefaultFs)

            var mdlBk = .bkColorModel
            if .bkColorModel.length == 0 do
                mdlBk = String.from("$richEdit_Bk")

            .frameWnd.style.clearStyleSheetColors()
            .scrollWnd.style.clearStyleSheetColors()

            .frameWnd.style.addStyleSheetColors(Format.toString("frameWnd_FocusBk %", mdlBk))
            .frameWnd.style.addStyleSheetColors(Format.toString("frameWnd_Bk %", mdlBk))
            .scrollWnd.style.addStyleSheetColors(Format.toString("scrollBar_Bk %", mdlBk))

            .stylesSelect[0].colFg = colors.richEdit_SelText
            .stylesSelect[0].colBk = colors.richEdit_SelBk
            .stylesSelect[1].colFg = colors.richEdit_SelTextNoFocus
            .stylesSelect[1].colBk = colors.richEdit_SelBkNoFocus

        case Edit:
            .style.clearFont()

            var mdlBk = .bkColorModel
            if .bkColorModel.length == 0 do
                mdlBk = String.from("$edit_Bk")

            .frameWnd.style.clearStyleSheetColors()
            .scrollWnd.style.clearStyleSheetColors()

            .frameWnd.style.addStyleSheetColors(Format.toString("frameWnd_FocusBk %", mdlBk))
            .frameWnd.style.addStyleSheetColors(Format.toString("frameWnd_Bk %", mdlBk))
            .scrollWnd.style.addStyleSheetColors(Format.toString("scrollBar_Bk %", mdlBk))

            .stylesSelect[0].colFg = colors.edit_Text
            .stylesSelect[0].colBk = colors.edit_SelectedBk
            .stylesSelect[1].colFg = 0
            .stylesSelect[1].colBk = 0
        }

        .invalidate()
    }

    // Returns true if the rich edit is read only
    mtd isReadOnly()->bool
    {
        var ro = false
        .sigIsReadOnly.call(&ro)
        return ro or .richEdFlags.has(.ReadOnly)
    }

    // Simulate a keyboard character pressed
    mtd charPressed(key: Input.Key, ch: rune, mdf: Input.KeyModifiers)
    {
        var isChar = false
        if mdf == .Zero and ch >= '0' and ch <= '9' do
            isChar = true

        // If there is a corresponding command,: nothing
        if !isChar
        {
            let cmd = .getMappingCommand(key, mdf)
            if cmd != .None do
                return
        }

        if .isReadOnly() do
            return

        var c = ch
        if c == '\r' do
            c = '\n'

        if c == '\n' and mdf.equals(.Zero) and .richEdFlags.has(.AutoLoseFocus) do
            return

        if .lexer and .lexer.insertRune(me, c) do
            return

        .insertRune(c)
        .forceCaretVisible = true
    }

    // Simulate a keyboard pressed
    mtd keyPressed(key: Input.Key, mdf: Input.KeyModifiers)
    {
        let cmd = .getMappingCommand(key, mdf)
        if cmd == .None do
            return
        .executeCommand(cmd)
        .forceCaretVisible = true
        .invalidate()
    }

    // Associate a lexer
    mtd setLexer(lex: IRichEditLexer)
    {
        .lexer = lex
        .lexer.setup(me)
        .lexer.setupTheme(me)
    }

    // Lex the full text
    mtd lexAll()
    {
        if !.lexer do
            return

        var prev, cur, next: *RichEditLine
        for i in .lines
        {
            cur  = .lines[i]
            next = i == .lines.count - 1 ? null : .lines[i + 1]
            .lexer.compute(prev, cur, next)
            prev = cur
        }
    }
}
