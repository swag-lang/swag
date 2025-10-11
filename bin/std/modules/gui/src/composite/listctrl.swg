#global public
using Core, Pixel

enum ListSelectionMode
{
    None
    Single
    Multi
}

#[Swag.EnumFlags]
enum ListFlags
{
    Zero                  = 0
    HideSelection
    AlternateLines
    HorzLines
    VertLines
    ForceMarginExpandMark         // Force one margin even if no expand mark
    NoMouseEmptySel               // Click does not clear selection if not on an item
    HotTrack
}

struct ListItem
{
    using item:     Item
    sortKey:        String
    colorBk:        Color
    colorFg:        Color
    column:         u32
    horzAlign       = StringHorzAlignment.Left
}

struct ListLine
{
    colorBk:             Color
    colorFg:             Color
    userData0:           *void
    userData1:           *void
    userData2:           *void
    userData3:           *void
    leftTextMargin:      f32
    forceExpandMark:     bool
    canCheck:            bool
    separator:           bool
    iconMargin:          bool

    lines:               ArrayPtr'ListLine
    items:               ArrayPtr'ListItem
    parent:              *ListLine
    index:               u32
    level:               u32
    expanded:            bool
}

struct ListCtrl
{
    using frameWnd:         FrameWnd

    horizontalExtent:       f32
    lineHeight:             f32
    indentWidth:            f32 = 22
    iconSize:               f32
    iconMarginFirstCol      = true
    listFlags               = ListFlags.Zero
    selectionMode           = ListSelectionMode.Single
    leftTextMargin:         f32

    sigSelChanged:          SigArray'func||(*ListCtrl)
    sigCheckChanged:        SigArray'func||(*ListCtrl)
    sigRightClick:          SigArray'func||(*ListCtrl, Math.Point)
    sigLeftDoubleClick:     SigArray'func||(*ListCtrl, Math.Point)
    sigExpand:              SigArray'func||(*ListCtrl, *ListLine)
    sigCollapse:            SigArray'func||(*ListCtrl, *ListLine)
    sigKeyPressed:          SigArray'func||(*ListCtrl, *KeyEvent)
    sigVirtualFill:         func(*ListCtrl, *ListLine, u32)
    sigSort:                func(*ListLine, *ListLine, u32, bool)->s32

    toFreeLines:            ArrayPtr'ListLine
    lines:                  Array'*ListLine
    idxLineToIdxList:       HashTable'(u32, u32)
    selModel:               SelModel
    checkModel:             SelModel
    focusIndex              = Swag.U32.Max
    hotIndex                = Swag.U32.Max
    scrollWnd:              *ScrollWnd
    listView:               *ListView
    header:                 *Header
    virtualCount:           u32
    isVirtual:              bool
}

struct ListView
{
    using wnd:               Wnd
    list:                    *ListCtrl
    discardMouseRelease:     bool
}

impl ListLine
{
    mtd isParentOf(other: *ListLine)->bool
    {
        var l = other.parent
        while l and l != me do
            l = l.parent
        return l ? true : false
    }

    #[Swag.Inline]
    mtd canExpand()->bool
    {
        return .forceExpandMark or .lines.count > 0
    }

    // Get an item by index
    mtd getItem(column: u32)->*ListItem
    {
        Debug.assert(column < .items.count)
        foreach item in .items
        {
            if item.column == column do
                return item
        }

        unreachable
    }

    // Initialize a given column item
    #[Swag.Discardable]
    mtd setItem(column: u32, name: string, icon: Icon = {})->*ListItem
    {
        foreach item in .items
        {
            if item.column == column
            {
                item.name = name
                item.icon = icon
                return item
            }
        }

        let item = .items.newPtr()
        item.column = column
        item.name   = name
        item.icon   = icon
        .items.add(item)
        return item
    }
}

impl IWnd for ListView
{
    mtd impl onResizeEvent(evt: *ResizeEvent)
    {
        if .list do
            .list.computeLayout()
    }

    mtd impl onKeyEvent(evt: *KeyEvent)
    {
        evt.accepted = true
        if evt.kind != .KeyPressed do
            return

        .list.sigKeyPressed.call(.list, evt)

        let lineCount = .list.getLineCount()
        if !lineCount do
            return

        var focusLine: *ListLine
        if .list.focusIndex < .list.lines.count do
            focusLine = .list.lines[.list.focusIndex]

        var shift = cast(bool) (evt.modifiers & .Shift)
        let ctrl  = cast(bool) (evt.modifiers & .Control)

        if .list.focusIndex == Swag.U32.Max
        {
            if evt.key != .Down do
                return
        }

        var selChanged = false
        var startSelIdx, endSelIdx: u32

        switch evt.key
        {
        case Space where focusLine:
            if focusLine.canCheck
            {
                foreach l in .list.selModel
                {
                    let idx = .list.idxLineToIdxList.find(l).value
                    if !.list.lines[idx].canCheck do
                        continue
                    let checked = !.list.checkModel.isSelected(l)
                    .list.checkModel.select(l, checked)
                }

                .list.sigCheckChanged.call(.list)
                .invalidate()
            }
            else
            {
                .list.toggleExpand(focusLine)
            }

        case Enter where focusLine:
            if focusLine.canCheck
            {
                foreach l in .list.selModel
                {
                    let idx = .list.idxLineToIdxList.find(l).value
                    if !.list.lines[idx].canCheck do
                        continue
                    .list.checkModel.select(l, true)
                }

                .list.sigCheckChanged.call(.list)
                .invalidate()
            }
            else
            {
                .list.toggleExpand(focusLine)
            }

        case A:
            if !ctrl or .list.selectionMode == .Single do
                break
            shift = true
            .list.selModel.unselectAll()
            endSelIdx   = 0
            startSelIdx = .list.getLineCount() - 1
            selChanged  = true

        case Home:
            startSelIdx = .list.focusIndex
            endSelIdx   = 0
            selChanged  = true

        case End:
            startSelIdx = .list.focusIndex
            endSelIdx   = lineCount - 1
            selChanged  = true

        case Up:
            if .list.focusIndex == 0 do
                startSelIdx, endSelIdx = 0
            else do
                startSelIdx, endSelIdx = .list.focusIndex - 1
            selChanged = true

        case Down:
            if .list.focusIndex == lineCount - 1 do
                startSelIdx, endSelIdx = lineCount - 1
            elif .list.focusIndex == Swag.U32.Max do
                .list.focusIndex = 0
            else do
                startSelIdx, endSelIdx = .list.focusIndex + 1
            selChanged = true

        case Left where focusLine:
            .list.collapse(focusLine)
        case Right where focusLine:
            .list.expand(focusLine)

        case PageDown:
            let first = .list.getFirstVisibleLineIndex()
            let last  = .list.getLastVisibleLineIndex()
            startSelIdx, endSelIdx = .list.focusIndex + (last - first)
            startSelIdx = Math.min(startSelIdx, lineCount - 1)
            endSelIdx   = Math.min(endSelIdx, lineCount - 1)
            selChanged  = true

        case PageUp:
            let first  = .list.getFirstVisibleLineIndex()
            let last   = .list.getLastVisibleLineIndex()
            let offset = last - first
            if offset > .list.focusIndex do
                startSelIdx, endSelIdx = 0
            else do
                startSelIdx, endSelIdx = .list.focusIndex - offset
            selChanged = true
        }

        if selChanged
        {
            if !shift or .list.selectionMode == .Single
            {
                .list.selModel.unselectAll()
                startSelIdx = endSelIdx
            }

            for i in startSelIdx to endSelIdx do
                .list.selectLine(i, true)
            .list.focusIndex = endSelIdx
            .list.ensureVisibleLine(.list.focusIndex)
        }
    }

    mtd impl onMouseEvent(evt: *MouseEvent)
    {
        if evt.kind == .MouseLeave
        {
            .list.hotIndex = Swag.U32.Max
            .invalidate()
            evt.accepted = false
            return
        }

        let idx = .hitTest(evt.surfacePos)
        if idx != .list.hotIndex and .list.listFlags.has(.HotTrack)
        {
            .list.hotIndex = idx
            .invalidate()
        }

        if evt.kind != .MousePressed and evt.kind != .MouseReleased and evt.kind != .MouseDoubleClick
        {
            evt.accepted = false
            return
        }

        let metrics = .getThemeMetrics()

        let ctrl  = cast(bool) (evt.modifiers & .Control)
        let shift = cast(bool) (evt.modifiers & .Shift)

        // Right mouse button
        // ============================

        if evt.button == .Right
        {
            if idx != Swag.U32.Max and !shift and !ctrl
            {
                if evt.kind == .MousePressed
                {
                    let idxLine = .list.isVirtual ? idx : .list.lines[idx].index
                    if !.list.selModel.isSelected(idxLine)
                    {
                        .list.selModel.unselectAll()
                        .list.selModel.select(idxLine, true)
                        .list.focusIndex = idx
                        .invalidate()
                    }
                }
                elif evt.kind == .MouseReleased
                {
                    let pt = .surfaceToLocal(evt.surfacePos)
                    .list.sigRightClick.call(.list, pt)
                }
            }

            return
        }

        if evt.button != .Left do
            return

        if idx != Swag.U32.Max and !.list.isVirtual
        {
            let hitLine  = .list.lines[idx]
            let idxLine  = hitLine.index
            var localPos = .surfaceToLocal(evt.surfacePos)
            localPos.x += .scrollPos.x
            var widthLeft = hitLine.level * .list.indentWidth
            if localPos.x >= widthLeft
            {
                // Expand
                // ============================

                widthLeft += Math.min(metrics.btnCheck_Size, .list.getLineHeight())
                if .list.lines[idx].canExpand()
                {
                    var eatCheck = false
                    if localPos.x < widthLeft do
                        eatCheck = true
                    widthLeft += Math.min(metrics.btnCheck_Size, .list.getLineHeight())

                    if eatCheck
                    {
                        if evt.kind == .MouseReleased do
                            return
                        .list.toggleExpand(.list.lines[idx])
                        return
                    }
                }

                // Check boxes
                // ============================

                // On checkbox
                if hitLine.canCheck
                {
                    var eatCheck = false
                    if evt.kind == .MouseDoubleClick do
                        eatCheck = true
                    elif localPos.x < widthLeft do
                        eatCheck = true

                    if eatCheck
                    {
                        if evt.kind == .MouseReleased do
                            return

                        let checked = !.list.checkModel.isSelected(idxLine)
                        if .list.selModel.isSelected(idxLine)
                        {
                            foreach l in .list.selModel do
                                .list.checkModel.select(l, checked)
                        }
                        else do
                            .list.checkModel.select(idxLine, checked)
                        .list.sigCheckChanged.call(.list)
                        .invalidate()
                        return
                    }
                }
            }
        }

        // User double click
        // ============================

        if evt.kind == .MouseDoubleClick
        {
            if idx != Swag.U32.Max do
                .list.toggleExpand(.list.lines[idx])
            let pt = .surfaceToLocal(evt.surfacePos)
            .list.sigLeftDoubleClick.call(.list, pt)
            return
        }

        // Selection by left button
        // ============================

        if .list.selectionMode == .None do
            return

        // We select by mouse release when the hittest line is selected
        if evt.kind == .MousePressed and idx != Swag.U32.Max and .list.isLineSelected(idx)
        {
            .discardMouseRelease = false
            return
        }

        if evt.kind == .MouseReleased and .discardMouseRelease
        {
            return
        }

        .discardMouseRelease = evt.kind == .MousePressed

        let canUnsel = idx != Swag.U32.Max or !.list.listFlags.has(.NoMouseEmptySel)
        if canUnsel and (.list.selectionMode == .Single or (!ctrl and !shift)) do
            .list.selModel.unselectAll()

        if idx != Swag.U32.Max
        {
            // Range selection
            if shift and .list.selectionMode == .Multi
            {
                let startIdx = .list.focusIndex == Swag.U32.Max ? 0 : .list.focusIndex
                let idx0     = Math.min(startIdx, idx)
                let idx1     = Math.max(startIdx, idx)
                for i in idx0 to idx1
                {
                    let idxLine = .list.isVirtual ? i : .list.lines[i].index
                    .list.selModel.select(idxLine, true)
                }
            }

            // Invert selection
            else
            {
                let idxLine = .list.isVirtual ? idx : .list.lines[idx].index
                let toSel   = !.list.selModel.isSelected(idxLine)
                .list.selModel.select(idxLine, toSel)
            }
        }

        if canUnsel do
            .list.focusIndex = idx
        .invalidate()
        if canUnsel or idx != Swag.U32.Max do
            .list.sigSelChanged.call(.list)
    }

    mtd impl onPaintEvent(evt: *PaintEvent)
    {
        if .list.getLineCount() == 0 do
            return

        let bc      = evt.bc
        let painter = bc.painter
        let colors  = .getThemeColors()
        let metrics = .getThemeMetrics()

        var rect: Math.Rectangle
        rect.width  = .getDisplayWidth()
        rect.height = .list.getLineHeight()

        // Clipping, take care of padding
        var clipRect = .getSurfaceRect()
        clipRect.applyPadding(.padding)

        let firstIdx = .list.getFirstVisibleLineIndex()
        let lastIdx  = .list.getLastVisibleLineIndex()

        // Do not display with bigs coordinates to avoid precision issues
        // So we: the scroll by hand, and draw will small coordinates
        var y = cast(f64) firstIdx * .list.getLineHeight()
        y -= .scrollPos.y
        rect.y = cast(f32) y

        if .list.isVirtual
        {
            Debug.assert(.list.sigVirtualFill != null)
            for i in firstIdx to lastIdx
            {
                var line: ListLine
                .list.sigVirtualFill(&line, i)
                .paintLine(bc, i, &line, rect)
                rect.y = rect.bottom()

                if .list.listFlags.has(.HorzLines) do
                    painter.drawLine(rect.x, rect.y, rect.right(), rect.y, colors.listItem_Line, 1)
            }
        }
        else
        {
            for i in firstIdx to lastIdx
            {
                let line = .list.lines[i]
                .paintLine(bc, i, line, rect)
                rect.y = rect.bottom()

                if .list.listFlags.has(.HorzLines) do
                    painter.drawLine(rect.x, rect.y, rect.right(), rect.y, colors.listItem_Line, 1)
            }
        }

        if .list.header and .list.listFlags.has(.VertLines)
        {
            rect = .getClientRectPadding()
            for idx in 1 until .list.header.items.count
            {
                var x = .getXPos(.list.header.items[idx].column)
                x -= .scrollPos.x
                x -= metrics.header_Separator * 0.5
                painter.drawLine(x, rect.y, x, rect.bottom(), colors.listItem_Line, 1)
            }

            let last = .list.header.items.back()
            var x    = .getXPos(last.column)
            x -= .scrollPos.x
            x += last.width
            x += metrics.header_Separator * 0.5
            painter.drawLine(x, rect.y, x, rect.bottom(), colors.listItem_Line, 1)
        }
    }
}

internal impl ListView
{
    mtd getXPos(column: u32)->f32
    {
        var x = .list.header.getItemPositionByCol(column)
        x += .list.header.getSurfaceRect().x
        x -= .getSurfaceRect().x
        return x
    }

    mtd paintLine(bc: PaintContext, idx: u32, line: *ListLine, rect: Math.Rectangle)
    {
        let painter = bc.painter
        let theme   = .getTheme()
        let colors  = .getThemeColors()
        let metrics = .getThemeMetrics()
        let rects   = .getThemeRects()

        // Selection box
        let isSelected  = .list.isLineSelected(idx)
        let canHotTrack = .list.isVirtual or !line.separator
        let hideSel     = .list.listFlags.has(.HideSelection)
        if isSelected and (!hideSel or .hasFocus()) and !bc.isDisabled
        {
            let colorSel = .hasFocus() ? colors.listItem_FocusSelectedBk : colors.listItem_SelectedBk
            painter.fillRect(rect, colorSel)
        }

        // Hot box
        elif idx == .list.hotIndex and .list.listFlags.has(.HotTrack) and canHotTrack
        {
            painter.fillRect(rect, colors.listItem_HotBk)
        }

        // User background
        elif line.colorBk.a
        {
            painter.fillRect(rect, line.colorBk)
        }

        // Alternate lines
        elif .list.listFlags.has(.AlternateLines) and idx % 2 == 1
        {
            painter.fillRect(rect, colors.listItem_AlternateBk)
        }

        var colorText, colorExpand: Color = undefined
        switch
        {
        case bc.isDisabled:
            colorText, colorExpand = colors.listItem_DisabledText
        case isSelected:
            colorText, colorExpand = .hasFocus() ? colors.listItem_FocusSelectedText : colors.listItem_SelectedText
        case line.colorFg.a:
            colorText   = line.colorFg
            colorExpand = colors.listItem_Text
        default:
            if line.separator do
                colorText, colorExpand = colors.listItem_Separator
            else do
                colorText, colorExpand = colors.listItem_Text
        }

        var format: RichStringFormat
        foreach item, idxItem in line.items
        {
            var colorFg: Color = undefined
            if item.colorFg.a and !isSelected do
                colorFg = item.colorFg
            else do
                colorFg = colorText
            theme.setTextColors(&format, colorFg)
            format.horzAlignment = item.horzAlign
            format.vertAlignment = .Center
            format.flags         = .Clip
            format.font          = .list.getFont()

            var labelPos = rect

            // Right
            if .list.header
            {
                labelPos.x     = .getXPos(item.column)
                labelPos.width = .list.header.getItemWidthByCol(item.column)
            }

            labelPos.x -= .scrollPos.x
            labelPos.x += .list.leftTextMargin + line.leftTextMargin

            if idxItem == 0
            {
                // Ident level
                labelPos.moveLeft(line.level * .list.indentWidth)

                // Expand/collapse mark
                let canExpand = line.canExpand()
                if canExpand or .list.listFlags.has(.ForceMarginExpandMark)
                {
                    if canExpand
                    {
                        let expand = theme.getIcon24(line.expanded ? .Expanded : .Collapsed)
                        expand.paint(painter, labelPos.x, labelPos.vertCenter() - expand.sizeY * 0.5, colorExpand)
                    }

                    labelPos.moveLeft(metrics.list_ExpandSize)
                }
            }

            // Background color ?
            if item.colorBk.a and !isSelected
            {
                var rectPos = labelPos
                rectPos.width += metrics.header_Separator * 0.5
                painter.fillRect(rectPos, item.colorBk)
            }

            painter.pushClippingRect(labelPos)
            defer painter.popClippingRect()

            // Checkbox
            if idxItem == 0 and line.canCheck
            {
                var subRectBorder: *ThemeImageRect
                var subRectMark:   *ThemeImageRect

                let idxLine = .list.isVirtual ? idx : line.index
                let checked = .list.checkModel.isSelected(idxLine)

                let colorBorder = colorFg
                let colorMark   = colorFg

                if checked
                {
                    subRectBorder = &rects.btnCheck_CheckedBorder
                    subRectMark   = &rects.btnCheck_CheckedMark
                }
                else
                {
                    subRectBorder = &rects.btnCheck_NormalBorder
                }

                let widthCheck = Math.min(metrics.btnCheck_Size, .list.getLineHeight())
                var posBox     = labelPos
                posBox.height = widthCheck
                posBox.y      = labelPos.vertCenter() - (posBox.height * 0.5)
                posBox.width  = posBox.height
                theme.drawSubRect(painter, posBox, subRectMark, colorMark)
                theme.drawSubRect(painter, posBox, subRectBorder, colorBorder)

                labelPos.moveLeft(widthCheck + 4)
            }

            // Take care of header item margin
            if .list.header do
                labelPos.moveLeft(.list.header.marginItems)

            // Left margin, or icon
            var icon = item.icon
            if .list.iconSize > 0
            {
                if icon.isValid() do
                    icon.sizeX, icon.sizeY = .list.iconSize
                else
                {
                    var iconMargin = .list.iconMarginFirstCol
                    if !idxItem and line.parent do
                        iconMargin = line.parent.iconMargin
                    elif idxItem do
                        iconMargin = .list.header.getItem(cast(u32) idxItem).iconMargin
                    if iconMargin do
                        labelPos.moveLeft(.list.iconSize + metrics.iconText_Margin)
                }
            }

            Theme.drawIconText(painter, labelPos, icon, &item.name, format, metrics.iconText_Margin)
        }
    }

    mtd hitTest(surfacePos: Math.Point)->u32
    {
        let lc  = .list.getLineCount()
        var res = .surfaceToLocal(surfacePos)
        if res.x < 0 or res.y < 0 do
            return Swag.U32.Max
        res.y += .scrollPos.y
        let idx = cast(u32) (res.y / .list.getLineHeight())
        if idx >= lc do
            return Swag.U32.Max
        if !.list.isVirtual and .list.lines[idx].separator do
            return Swag.U32.Max
        return idx
    }

    mtd getDisplayWidth()->f32
    {
        if .list.horizontalExtent > 0 do
            return .list.horizontalExtent
        return .position.width
    }
}

impl ListCtrl
{
    #[Swag.Discardable]
    func createSimple(parent: *Wnd, position: Math.Rectangle = {}, id: WndId = null)->*ListCtrl
    {
        with let list = Wnd.create'ListCtrl(parent, "", position, id)
        {
            .scrollWnd               = .createView'ScrollWnd()
            .scrollWnd.dockStyle     = .Center
            .scrollWnd.focusStrategy = .None
            .scrollWnd.setAllPaddings(.getThemeMetrics().list_Padding)

            .listView      = .scrollWnd.createView'ListView()
            .listView.list = list
            .listView.wndFlags |= .NoScroll
        }

        return list
    }

    #[Swag.Discardable]
    func createMultiColumns(parent: *Wnd, position: Math.Rectangle = {}, id: WndId = null)->*ListCtrl
    {
        let list = Wnd.create'ListCtrl(parent, "", position, id)

        let fwnd = list.createView'Wnd()
        fwnd.dockStyle = .Center
        fwnd.wndFlags ^= .ClipChildren

        // Adapt header to the frame container
        list.hookOnEvent = func(wnd, evt)
        {
            if !evt is ResizeEvent do
                return false

            var list   = cast(*ListCtrl) wnd
            var header = list.header
            if header.form == .Transparent do
                return false
            switch list.frameForm
            {
            case Round:
                header.form   = .Round
                header.margin = -wnd.getThemeMetrics().frameWnd_RoundPadding
            case Square:
                header.form   = .Flat
                header.margin = -wnd.getThemeMetrics().frameWnd_SquarePadding
            case Transparent:
                header.form   = .Flat
                header.margin = -wnd.getThemeMetrics().frameWnd_FlatPadding
            }

            header.setMargin(bottom: 0)
            return false
        }

        with list
        {
            // Header
            // Need to recompute layout when header size has changed
            .header                  = Header.create(fwnd, {})
            .header.dockStyle        = .Top
            .header.form             = .Round
            .header.sigLayoutChanged = func|list|(header)
            {
                list.computeLayout()
            }

            .header.sigClicked = func|list|(header, col)
            {
                list.sort(col, header.sortMark <= 0 ? false : true, true)
            }

            // Scroll that contains the list view
            .scrollWnd                 = Wnd.create'ScrollWnd(fwnd, {})
            .scrollWnd.dockStyle       = .Center
            .scrollWnd.focusStrategy   = .None
            .scrollWnd.backgroundStyle = .Transparent
            .scrollWnd.setAllPaddings(.getThemeMetrics().list_Padding)
            .scrollWnd.sigScrollPosChanged += func|list.header|(scrollWnd, op, np)
            {
                header.scrollPos.x = np.x
                header.invalidate()
            }

            // ListCtrl view
            .listView      = .scrollWnd.createView'ListView()
            .listView.list = list
            .listView.wndFlags |= .NoScroll
        }

        return list
    }

    // Unselect all lines
    mtd unselectAll()
    {
        if .selModel.isEmpty() do
            return
        .selModel.unselectAll()
        .invalidate()
        .sigSelChanged.call(me)
    }

    // Returns
    mtd isLineSelected(idx: u32)->bool
    {
        Debug.assert(idx < .getLineCount())
        if .selectionMode == .None do
            return false
        if .isVirtual do
            return .selModel.isSelected(idx)
        return !.lines[idx].separator and .selModel.isSelected(.lines[idx].index)
    }

    // Select of unselect the given line
    mtd selectLine(idx: u32, select: bool = true)
    {
        let lineCount = .getLineCount()
        Debug.assert(idx < lineCount)

        if .selectionMode == .None or lineCount == 0 do
            return
        if !.isVirtual and .lines[idx].separator do
            return

        if .selectionMode == .Single do
            .selModel.unselectAll()

        let idxLine = .isVirtual ? idx : .lines[idx].index
        .selModel.select(idxLine, select)
        .invalidate()

        .sigSelChanged.call(me)
    }

    // Select a range of lines
    mtd selectLines(start, end: u32, select: bool = true)
    {
        let lineCount = .getLineCount()
        Debug.assert(start < lineCount)
        Debug.assert(end < lineCount)

        if .selectionMode == .None or lineCount == 0 do
            return

        for i in start to end
        {
            let idxLine = .isVirtual ? i : .lines[i].index
            .selModel.select(idxLine, select)
        }

        .invalidate()

        .sigSelChanged.call(me)
    }

    // Compute global layout, once all lines have been added
    mtd computeLayout()
    {
        let clientRect = .scrollWnd.getClientRectPadding()

        var scrollWidth: f32 = undefined
        if .horizontalExtent > 0 do
            scrollWidth = .horizontalExtent
        elif .header do
            scrollWidth = .header.getTotalItemsWidth()
        else do
            scrollWidth = clientRect.width
        let scrollHeight = .getLineCount() * .getLineHeight()
        .scrollWnd.setScrollSize(scrollWidth, scrollHeight)
        .invalidate()
    }

    // Get the height of one line
    mtd getLineHeight()->f32
    {
        return .lineHeight orelse .getFont().regular.height + 8
    }

    // Get a line by index
    mtd getLine(idx: u32)->*ListLine
    {
        return .lines[idx]
    }

    // Returns the number of lines in the list
    mtd getLineCount()->u32
    {
        if .isVirtual do
            return .virtualCount
        return cast(u32) .lines.count
    }

    // Add one new line
    #[Swag.Overload]
    mtd createLine(parentLine: #null *ListLine)->*ListLine
    {
        Debug.assert(!.isVirtual, "createLine cannot be called on a virtual list")

        if parentLine
        {
            let line = parentLine.lines.newPtr()
            line.parent = parentLine
            line.level  = parentLine.level + 1
            parentLine.lines.add(line)
            return line
        }

        let line = .toFreeLines.newPtr()
        line.index = cast(u32) .lines.count
        .idxLineToIdxList.add(line.index, line.index)
        .lines.add(line)
        .toFreeLines.add(line)
        return line
    }

    // Add one line, and set first column
    #[Swag.Discardable, Swag.Overload]
    mtd createLine(name: string, icon: Icon = {}, parentLine: *ListLine = null)->*ListLine
    {
        let line = .createLine(parentLine)
        line.setItem(0, name, icon)
        return line
    }

    // Clear all lines
    mtd clear()
    {
        .lines.clear()
        .virtualCount = 0
        .idxLineToIdxList.clear()
        .focusIndex = Swag.U32.Max
        .selModel.unselectAll()
        .checkModel.unselectAll()
    }

    // Add one column in a multi columns list
    mtd addColumn(name: string, width: f32 = 100, icon: Icon = {})
    {
        Debug.assert(.header != null)
        .header.addItem(name, width, icon)
    }

    // Set the column width
    mtd setColumnWidth(idx: u32, width: f32)
    {
        Debug.assert(.header != null)
        .header.setItemWidth(idx, width)
    }

    // Set the number of lines in a virtual list
    mtd setVirtualCount(count: u32)
    {
        .clear()
        .isVirtual    = true
        .virtualCount = count
        .computeLayout()
    }

    // Returns the first visible line index
    mtd getFirstVisibleLineIndex()->u32
    {
        let lc  = .getLineCount()
        let idx = cast(u32) (.listView.scrollPos.y / .getLineHeight())
        if idx >= lc do
            return Swag.U32.Max
        return idx
    }

    // Returns the last visible line index
    mtd getLastVisibleLineIndex()->u32
    {
        let lc = .getLineCount()
        if lc == 0 do
            return Swag.U32.Max

        var y = .listView.scrollPos.y + .scrollWnd.view.position.height
        y -= .scrollWnd.view.padding.y
        y -= .scrollWnd.view.padding.w
        if y < 0 do
            return cast(u32) (.listView.scrollPos.y / .getLineHeight())
        let idx = cast(u32) (y / .getLineHeight())
        if idx >= lc do
            return cast(u32) lc - 1
        return idx
    }

    // Set the corresponding line visible
    mtd ensureVisibleLine(idx: u32)
    {
        let first = .getFirstVisibleLineIndex()
        let last  = .getLastVisibleLineIndex()
        if idx > first and idx < last do
            return
        let sc = .scrollWnd.getScrollPos()
        .scrollWnd.setScrollPos(sc.x, idx * .getLineHeight())
    }

    // Returns true if the line at the given index is selected
    mtd const isSelectedLine(idx: u32)->bool
    {
        let idxLine = .isVirtual ? idx : .lines[idx].index
        return .selModel.isSelected(idxLine)
    }

    // Get the line with the keyboard focus
    mtd const getFocusLine()->#null *ListLine
    {
        if .focusIndex >= .lines.count do
            return null
        return .lines[.focusIndex]
    }

    // Get all the selected lines
    mtd const getSelectedLines()->Array'(*ListLine)
    {
        var result: retval
        if .isVirtual or .selModel.isEmpty() do
            return result

        foreach i in .selModel
        {
            let res = .idxLineToIdxList.tryFind(i)
            if !res do
                continue
            result.add(.lines[res.value])
        }

        return result
    }

    // Get all the checked lines
    mtd const getCheckedLines()->Array'(*ListLine)
    {
        var result: retval
        if .isVirtual or .checkModel.isEmpty() do
            return result

        foreach i in .checkModel
        {
            let res = .idxLineToIdxList.tryFind(i)
            if !res do
                continue
            result.add(.lines[res.value])
        }

        return result
    }

    // Returns a selected line
    // Mostly for singlesel mode
    mtd const getSelectedLine()->#null *ListLine
    {
        if .isVirtual or .selModel.isEmpty() do
            return null
        let range = .selModel.selection.front()
        let res   = .idxLineToIdxList.tryFind(range.start)
        if !res do
            return null
        let idx = res.value
        return .lines[idx]
    }

    internal mtd mapIndexes()
    {
        .idxLineToIdxList.clear()
        foreach l, idx in .lines do
            .idxLineToIdxList.add(l.index, cast(u32) idx)
    }

    #[Swag.Overload]
    internal mtd sort(column: u32, descentOrder: bool, toSort: *Array'*ListLine)
    {
        func sortFunc(column: u32, descentOrder: bool, a, b: *ListLine)->s32
        {
            let it0   = a.getItem(column)
            let it1   = b.getItem(column)
            let sort0 = it0.sortKey.isEmpty() ? &it0.name.raw : &it0.sortKey
            let sort1 = it1.sortKey.isEmpty() ? &it1.name.raw : &it1.sortKey
            var res   = Latin1.compareNatural(dref sort0, dref sort1)
            if descentOrder do
                res *= -1
            return res
        }

        let cb = mtd|column, descentOrder|(a, b: *ListLine)->s32
        {
            if .sigSort do
                return .sigSort(a, b, column, descentOrder)

            if a.parent != b.parent
            {
                if a.isParentOf(b) do
                    return -1
                if b.isParentOf(a) do
                    return 1

                var a1 = a
                var b1 = b
                while a1.level > b1.level do
                    a1 = a1.parent
                while b1.level > a1.level do
                    b1 = b1.parent
                while a1.parent != b1.parent
                {
                    a1 = a1.parent
                    b1 = b1.parent
                }

                return sortFunc(column, descentOrder, a1, b1)
            }

            return sortFunc(column, descentOrder, a, b)
        }

        toSort.sort(cb)
    }

    // Sort the list by a given column
    // If 'persistent' is true, then the sorted column will be displayed in the
    // header (if it exists)
    #[Swag.Overload]
    mtd sort(column: u32, descentOrder, persistent = false)
    {
        if .isVirtual do
            return

        .sort(column, descentOrder, &.lines)
        .mapIndexes()

        if .header and persistent
        {
            .header.sortColumn = column
            .header.sortMark   = descentOrder ? -1 : 1
            .header.invalidate()
        }

        .invalidate()
    }

    // Sort list as before
    #[Swag.Overload]
    mtd sort()
    {
        if .header and .header.sortMark != 0 do
            .sort(.header.sortColumn, .header.sortMark == -1 ? true : false)
        else do
            .sort(0)
    }

    // Expand the given line
    mtd expand(line: *ListLine)
    {
        if !line.canExpand() or line.expanded do
            return
        line.expanded = true
        .sigExpand.call(me, line)

        var tmp: Array'(*ListLine)
        tmp.add(line)

        // Auto sort
        if .header and .header.sortMark do
            .sort(.header.sortColumn, .header.sortMark == -1 ? true : false, &line.lines)

        var cptIdxLine = cast(u32) .lines.count
        var cptIdx     = 0'u32
        while tmp.count
        {
            let toI = tmp.popBack()
            let res = .idxLineToIdxList.tryFind(toI.index)
            if !res do
                continue
            let idxInsert = res.value
            foreach l, i in toI.lines
            {
                l.index = cptIdxLine
                cptIdxLine += 1
                .idxLineToIdxList.add(l.index, idxInsert + cptIdx + 1)
                cptIdx += 1
                if l.expanded do
                    tmp.add(l)
            }

            // Update indexes after the insertion
            for var idx = idxInsert + 1; idx < cast(u32) .lines.count; idx += 1 do
                .idxLineToIdxList.add(.lines[idx].index, cast(u32) (idx + toI.lines.count))

            // Insert at the right position
            .lines.insertAt(cast(u64) (idxInsert + 1), toI.lines.toSlice())
        }

        .computeLayout()
    }

    // Collapse the given line
    mtd collapse(line: *ListLine)
    {
        if !line.canExpand() or !line.expanded do
            return
        line.expanded = false
        .sigCollapse.call(me, line)

        var tmp: Array'(*ListLine)
        tmp.add(line)

        var removeCount = 0'u32
        while tmp.count
        {
            let toI = tmp.popBack()
            foreach l in toI.lines
            {
                removeCount += 1
                if l.expanded do
                    tmp.add(l)
            }
        }

        // Insert at the right position
        let res = .idxLineToIdxList.tryFind(line.index)
        if !res do
            return
        let idxInsert = res.value
        .lines.removeAtOrdered(cast(u64) (idxInsert + 1), cast(u64) removeCount)

        // Update indexes after the insertion
        for var idx = idxInsert + 1; idx < cast(u32) .lines.count; idx += 1 do
            .idxLineToIdxList.add(.lines[idx].index, idx)

        // Update selection
        if .focusIndex >= idxInsert + 1 and .focusIndex <= idxInsert + 1 + removeCount
        {
            .unselectAll()
            .focusIndex = idxInsert
            .selectLine(.focusIndex)
        }

        .computeLayout()
    }

    // Collapse all lines
    mtd collapseAll()
    {
        func collapseRec(sl: [..] *ListLine)
        {
            foreach l in sl
            {
                l.expanded = false
                collapseRec(l.lines.toSlice())
            }
        }

        foreach l in .lines
        {
            if l.expanded do
                .collapse(l)
        }

        collapseRec(.lines.toSlice())
        .computeLayout()
    }

    // Collapse the given line
    mtd toggleExpand(line: *ListLine)
    {
        if line.expanded do
            .collapse(line)
        else do
            .expand(line)
    }

    // Set focus to the list
    mtd setFocus()
    {
        .listView.setFocus()
    }
}
