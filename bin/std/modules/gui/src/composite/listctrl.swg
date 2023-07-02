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
    Zero = 0
    HideSelection
    AlternateLines
    HorzLines
    VertLines
    ForceMarginExpandMark   // Force one margin even if no expand mark
    NoMouseEmptySel         // Click does not clear selection if not on an item
    HotTrack
}

struct ListItem
{
    using item: Item
    sortKey:    String
    colorBk:    Color
    colorFg:    Color
    column:     u32
    horzAlign   = StringHorzAlignment.Left
}

struct ListLine
{
    colorBk:            Color
    colorFg:            Color
    userData0:          *void
    userData1:          *void
    userData2:          *void
    userData3:          *void
    leftTextMargin:     f32
    forceExpandMark:    bool
    canCheck:           bool
    separator:          bool
    iconMargin:         bool

    lines:          ArrayPtr'ListLine
    items:          ArrayPtr'ListItem
    parent:         *ListLine
    index:          u32
    level:          u32
    expanded:       bool
}

struct ListCtrl
{
    using frameWnd: FrameWnd

    horizontalExtent:   f32
    lineHeight:         f32
    indentWidth:        f32 = 22
    iconSize:           f32
    iconMarginFirstCol  = true
    listFlags           = ListFlags.Zero
    selectionMode       = ListSelectionMode.Single
    leftTextMargin:     f32

    sigSelChanged:      SigArray'closure(*Self)
    sigCheckChanged:    SigArray'closure(*Self)
    sigRightClick:      SigArray'closure(*Self, Math.Point)
    sigLeftDoubleClick: SigArray'closure(*Self, Math.Point)
    sigExpand:          SigArray'closure(*Self, *ListLine)
    sigCollapse:        SigArray'closure(*Self, *ListLine)
    sigKeyPressed:      SigArray'closure(*Self, *KeyEvent)
    sigVirtualFill:     func(*Self, *ListLine, u32)
    sigSort:            func(*ListLine, *ListLine, u32, bool)->s32

    toFreeLines:        ArrayPtr'ListLine
    lines:              Array'*ListLine
    idxLineToIdxList:   HashTable'(u32, u32)
    selModel:           SelModel
    checkModel:         SelModel
    focusIndex          = Swag.U32.Max
    hotIndex            = Swag.U32.Max
    scrollWnd:          *ScrollWnd
    listView:           *ListView
    header:             *Header
    virtualCount:       u32
    isVirtual:          bool
}

struct ListView
{
    using wnd:              Wnd
    list:                   *ListCtrl
    discardMouseRelease:    bool
}

impl ListLine
{
    mtd isParentOf(other: *ListLine)->bool
    {
        l := other.parent
        while l and l != self
            l = l.parent
        return l ? true : false
    }

    #[Swag.Inline]
    mtd canExpand()->bool
    {
        return forceExpandMark or lines.count > 0
    }

    // Get an item by index
    mtd getItem(column: u32)->*ListItem
    {
        visit item: items
        {
            if item.column == column
                return item
        }

        return null
    }

    // Initialize a given column item
    #[Swag.Discardable]
    mtd setItem(column: u32, name: string, icon: Icon = {})->*ListItem
    {
        visit item: items
        {
            if item.column == column
            {
                item.name = name
                item.icon = icon
                return item
            }
        }

        item := items.newPtr()
        item.column = column
        item.name = name
        item.icon = icon
        items.add(item)
        return item
    }
}

impl IWnd for ListView
{
    mtd impl onResizeEvent(evt: *ResizeEvent)
    {
        if list
            list.computeLayout()
    }

    mtd impl onKeyEvent(evt: *KeyEvent)
    {
        evt.accepted = true
        if evt.kind != .KeyPressed
            return

        list.sigKeyPressed.call(list, evt)

        lineCount := list.getLineCount()
        if !lineCount
            return

        var focusLine: *ListLine
        if list.focusIndex < list.lines.count
            focusLine = list.lines[list.focusIndex]

        shift := cast(bool) (evt.modifiers & .Shift)
        ctrl  := cast(bool) (evt.modifiers & .Control)

        if list.focusIndex == Swag.U32.Max
        {
            if evt.key != .Down
                return
        }

        selChanged := false
        var startSelIdx, endSelIdx: u32

        switch evt.key
        {
        case Space:
            if focusLine.canCheck
            {
                visit l: list.selModel
                {
                    idx := list.idxLineToIdxList.find(l).value
                    if !list.lines[idx].canCheck
                        continue
                    checked := !list.checkModel.isSelected(l)
                    list.checkModel.select(l, checked)
                }

                list.sigCheckChanged.call(list)
                invalidate()
            }
            else
            {
                list.toggleExpand(focusLine)
            }

        case Enter:
            if focusLine.canCheck
            {
                visit l: list.selModel
                {
                    idx := list.idxLineToIdxList.find(l).value
                    if !list.lines[idx].canCheck
                        continue
                    list.checkModel.select(l, true)
                }

                list.sigCheckChanged.call(list)
                invalidate()
            }
            else
            {
                list.toggleExpand(focusLine)
            }

        case A:
            if !ctrl or list.selectionMode == .Single
                break
            shift = true
            list.selModel.unselectAll()
            endSelIdx = 0
            startSelIdx = list.getLineCount() - 1
            selChanged = true

        case Home:
            startSelIdx = list.focusIndex
            endSelIdx = 0
            selChanged = true

        case End:
            startSelIdx = list.focusIndex
            endSelIdx = lineCount - 1
            selChanged = true

        case Up:
            if list.focusIndex == 0
                startSelIdx, endSelIdx = 0
            else
                startSelIdx, endSelIdx = list.focusIndex - 1
            selChanged = true

        case Down:
            if list.focusIndex == lineCount - 1
                startSelIdx, endSelIdx = lineCount - 1
            elif list.focusIndex == Swag.U32.Max
                list.focusIndex = 0
            else
                startSelIdx, endSelIdx = list.focusIndex + 1
            selChanged = true

        case Left:
            list.collapse(focusLine)
        case Right:
            list.expand(focusLine)

        case PageDown:
            first := list.getFirstVisibleLineIndex()
            last  := list.getLastVisibleLineIndex()
            startSelIdx, endSelIdx = list.focusIndex + (last - first)
            startSelIdx = Math.min(startSelIdx, lineCount - 1)
            endSelIdx = Math.min(endSelIdx, lineCount - 1)
            selChanged = true
            break

        case PageUp:
            first := list.getFirstVisibleLineIndex()
            last  := list.getLastVisibleLineIndex()
            offset := last - first
            if offset > list.focusIndex
                startSelIdx, endSelIdx = 0
            else
                startSelIdx, endSelIdx = list.focusIndex - offset
            selChanged = true
            break
        }

        if selChanged
        {
            if !shift or list.selectionMode == .Single
            {
                list.selModel.unselectAll()
                startSelIdx = endSelIdx
            }

            loop i: startSelIdx..endSelIdx
                list.selectLine(i, true)
            list.focusIndex = endSelIdx
            list.ensureVisibleLine(list.focusIndex)
        }
    }

    mtd impl onMouseEvent(evt: *MouseEvent)
    {
        if evt.kind == .MouseLeave
        {
            list.hotIndex = Swag.U32.Max
            invalidate()
            evt.accepted = false
            return
        }

        idx := hitTest(evt.surfacePos)
        if idx != list.hotIndex and list.listFlags.has(.HotTrack)
        {
            list.hotIndex = idx
            invalidate()
        }

        if evt.kind != .MousePressed and evt.kind != .MouseReleased and evt.kind != .MouseDoubleClick
        {
            evt.accepted = false
            return
        }

        metrics := getThemeMetrics()

        ctrl  := cast(bool) (evt.modifiers & .Control)
        shift := cast(bool) (evt.modifiers & .Shift)

        // Right mouse button
        // ============================

        if evt.button == .Right
        {
            if idx != Swag.U32.Max and !shift and !ctrl
            {
                if evt.kind == .MousePressed
                {
                    idxLine := list.isVirtual ? idx : list.lines[idx].index
                    if !list.selModel.isSelected(idxLine)
                    {
                        list.selModel.unselectAll()
                        list.selModel.select(idxLine, true)
                        list.focusIndex = idx
                        invalidate()
                    }
                }
                elif evt.kind == .MouseReleased
                {
                    pt := surfaceToLocal(evt.surfacePos)
                    list.sigRightClick.call(list, pt)
                }
            }

            return
        }

        if evt.button != .Left
            return

        if idx != Swag.U32.Max and !list.isVirtual
        {
            hitLine := list.lines[idx]
            idxLine := hitLine.index
            localPos := surfaceToLocal(evt.surfacePos)
            localPos.x += scrollPos.x
            widthLeft := hitLine.level * list.indentWidth
            if localPos.x >= widthLeft
            {
                // Expand
                // ============================

                widthLeft += Math.min(metrics.btnCheck_Size, list.getLineHeight())
                if list.lines[idx].canExpand()
                {
                    eatCheck := false
                    if localPos.x < widthLeft
                        eatCheck = true
                    widthLeft += Math.min(metrics.btnCheck_Size, list.getLineHeight())

                    if eatCheck
                    {
                        if evt.kind == .MouseReleased
                            return
                        list.toggleExpand(list.lines[idx])
                        return
                    }
                }

                // Check boxes
                // ============================

                // On checkbox
                if hitLine.canCheck
                {
                    eatCheck := false
                    if evt.kind == .MouseDoubleClick
                        eatCheck = true
                    elif localPos.x < widthLeft
                        eatCheck = true

                    if eatCheck
                    {
                        if evt.kind == .MouseReleased
                            return

                        checked := !list.checkModel.isSelected(idxLine)
                        if list.selModel.isSelected(idxLine)
                        {
                            visit l: list.selModel
                                list.checkModel.select(l, checked)
                        }
                        else
                            list.checkModel.select(idxLine, checked)
                        list.sigCheckChanged.call(list)
                        invalidate()
                        return
                    }
                }
            }
        }

        // User double click
        // ============================

        if evt.kind == .MouseDoubleClick
        {
            if idx != Swag.U32.Max
                list.toggleExpand(list.lines[idx])
            pt := surfaceToLocal(evt.surfacePos)
            list.sigLeftDoubleClick.call(list, pt)
            return
        }

        // Selection by left button
        // ============================

        if list.selectionMode == .None
            return

        // We select by mouse release when the hittest line is selected
        if evt.kind == .MousePressed and idx != Swag.U32.Max and list.isLineSelected(idx)
        {
            discardMouseRelease = false
            return
        }

        if evt.kind == .MouseReleased and discardMouseRelease
        {
            return
        }

        discardMouseRelease = evt.kind == .MousePressed

        canUnsel := idx != Swag.U32.Max or !list.listFlags.has(.NoMouseEmptySel)
        if canUnsel and (list.selectionMode == .Single or (!ctrl and !shift))
            list.selModel.unselectAll()

        if idx != Swag.U32.Max
        {
            // Range selection
            if shift and list.selectionMode == .Multi
            {
                startIdx := list.focusIndex == Swag.U32.Max ? 0 : list.focusIndex
                loop i: startIdx..idx
                {
                    idxLine := list.isVirtual ? i : list.lines[i].index
                    list.selModel.select(idxLine, true)
                }
            }

            // Invert selection
            else
            {
                idxLine := list.isVirtual ? idx : list.lines[idx].index
                toSel   := !list.selModel.isSelected(idxLine)
                list.selModel.select(idxLine, toSel)
            }
        }

        if canUnsel
            list.focusIndex = idx
        invalidate()
        if canUnsel or idx != Swag.U32.Max
            list.sigSelChanged.call(list)
    }

    mtd impl onPaintEvent(evt: *PaintEvent)
    {
        if list.getLineCount() == 0
            return

        bc      := evt.bc
        painter := bc.painter
        colors  := getThemeColors()
        metrics := getThemeMetrics()

        var rect: Math.Rectangle
        rect.width = getDisplayWidth()
        rect.height = list.getLineHeight()

        // Clipping, take care of padding
        clipRect := getSurfaceRect()
        clipRect.applyPadding(padding)

        firstIdx := list.getFirstVisibleLineIndex()
        lastIdx  := list.getLastVisibleLineIndex()

        // Do not display with bigs coordinates to avoid precision issues
        // So we do the scroll by hand, and draw will small coordinates
        y := cast(f64) firstIdx * list.getLineHeight()
        y -= scrollPos.y
        rect.y = cast(f32) y

        if list.isVirtual
        {
            Debug.assert(list.sigVirtualFill != null)
            loop i: firstIdx..lastIdx
            {
                var line: ListLine
                list.sigVirtualFill(&line, i)
                paintLine(bc, i, &line, rect)
                rect.y = rect.bottom()

                if list.listFlags.has(.HorzLines)
                    painter.drawLine(rect.x, rect.y, rect.right(), rect.y, colors.listItem_Line, 1)
            }
        }
        else
        {
            loop i: firstIdx..lastIdx
            {
                line := list.lines[i]
                paintLine(bc, i, line, rect)
                rect.y = rect.bottom()

                if list.listFlags.has(.HorzLines)
                    painter.drawLine(rect.x, rect.y, rect.right(), rect.y, colors.listItem_Line, 1)
            }
        }

        if list.header and list.listFlags.has(.VertLines)
        {
            rect = getClientRectPadding()
            loop idx: 1..list.header.items.count-1
            {
                x := getXPos(list.header.items[idx].column)
                x -= scrollPos.x
                x -= metrics.header_Separator * 0.5
                painter.drawLine(x, rect.y, x, rect.bottom(), colors.listItem_Line, 1)
            }

            last := list.header.items.back()
            x := getXPos(last.column)
            x -= scrollPos.x
            x += last.width
            x += metrics.header_Separator * 0.5
            painter.drawLine(x, rect.y, x, rect.bottom(), colors.listItem_Line, 1)
        }
    }
}

private impl ListView
{
    mtd getXPos(column: u32)->f32
    {
        x := list.header.getItemPositionByCol(column)
        x += list.header.getSurfaceRect().x
        x -= getSurfaceRect().x
        return x
    }

    mtd paintLine(bc: PaintContext, idx: u32, line: *ListLine, rect: Math.Rectangle)
    {
        painter := bc.painter
        theme   := getTheme()
        colors  := getThemeColors()
        metrics := getThemeMetrics()
        rects   := getThemeRects()

        // Selection box
        isSelected  := list.isLineSelected(idx)
        canHotTrack := list.isVirtual or !line.separator
        hideSel := list.listFlags.has(.HideSelection)
        if isSelected and (!hideSel or hasFocus()) and !bc.isDisabled
        {
            colorSel := hasFocus() ? colors.listItem_FocusSelectedBk : colors.listItem_SelectedBk
            painter.fillRect(rect, colorSel)
        }

        // Hot box
        elif idx == list.hotIndex and list.listFlags.has(.HotTrack) and canHotTrack
        {
            painter.fillRect(rect, colors.listItem_HotBk)
        }

        // User background
        elif line.colorBk.a
        {
            painter.fillRect(rect, line.colorBk)
        }

        // Alternate lines
        elif list.listFlags.has(.AlternateLines) and idx % 2 == 1
        {
            painter.fillRect(rect, colors.listItem_AlternateBk)
        }

        var colorText, colorExpand: Color = undefined
        switch
        {
            case bc.isDisabled:
                colorText, colorExpand = colors.listItem_DisabledText
            case isSelected:
                colorText, colorExpand = colors.listItem_SelectedText
            case line.colorFg.a:
                colorText   = line.colorFg
                colorExpand = colors.listItem_Text
            default:
                if line.separator
                    colorText, colorExpand = colors.listItem_Separator
                else
                    colorText, colorExpand = colors.listItem_Text
        }

        var format: RichStringFormat
        visit item, idxItem: line.items
        {
            var colorFg: Color = undefined
            if item.colorFg.a and !isSelected
                colorFg = item.colorFg
            else
                colorFg = colorText
            theme.setTextColors(&format, colorFg)
            format.horzAlignment = item.horzAlign
            format.vertAlignment = .Center
            format.flags = .Clip
            format.font = list.getFont()

            labelPos := rect

            // Right
            if list.header
            {
                labelPos.x = getXPos(item.column)
                labelPos.width = list.header.getItemWidthByCol(item.column)
            }

            labelPos.x -= scrollPos.x
            labelPos.x += list.leftTextMargin + line.leftTextMargin

            if idxItem == 0
            {
                // Ident level
                labelPos.moveLeft(line.level * list.indentWidth)

                // Expand/collapse mark
                canExpand := line.canExpand()
                if canExpand or list.listFlags.has(.ForceMarginExpandMark)
                {
                    if canExpand
                    {
                        expand := theme.getIcon24(line.expanded ? .SortArrowDown : .SortArrowRight)
                        expand.paint(painter, labelPos.x, labelPos.vertCenter() - expand.sizeY * 0.5, colorExpand)
                    }

                    labelPos.moveLeft(metrics.list_ExpandSize)
                }
            }

            // Background color ?
            if item.colorBk.a and !isSelected
            {
                rectPos := labelPos
                rectPos.width += metrics.header_Separator * 0.5
                painter.fillRect(rectPos, item.colorBk)
            }

            painter.pushClippingRect(labelPos)
            defer painter.popClippingRect()

            // Checkbox
            if idxItem == 0 and line.canCheck
            {
                var subRectBorder:  *ThemeImageRect
                var subRectMark:    *ThemeImageRect

                idxLine := list.isVirtual ? idx : line.index
                checked := list.checkModel.isSelected(idxLine)

                colorBorder := colorFg
                colorMark   := colorFg

                if checked
                {
                    subRectBorder = &rects.btnCheck_CheckedBorder
                    subRectMark   = &rects.btnCheck_CheckedMark
                }
                else
                {
                    subRectBorder = &rects.btnCheck_NormalBorder
                }

                widthCheck := Math.min(metrics.btnCheck_Size, list.getLineHeight())
                posBox := labelPos
                posBox.height = widthCheck
                posBox.y = labelPos.vertCenter() - (posBox.height * 0.5)
                posBox.width = posBox.height
                theme.drawSubRect(painter, posBox, subRectMark, colorMark)
                theme.drawSubRect(painter, posBox, subRectBorder, colorBorder)

                labelPos.moveLeft(widthCheck + 4)
            }

            // Take care of header item margin
            if list.header
                labelPos.moveLeft(list.header.marginItems)

            // Left margin, or icon
            icon := item.icon
            if list.iconSize > 0
            {
                if icon.isValid()
                    icon.sizeX, icon.sizeY = list.iconSize
                else
                {
                    iconMargin := list.iconMarginFirstCol
                    if !idxItem and line.parent
                        iconMargin = line.parent.iconMargin
                    else if idxItem
                        iconMargin = list.header.getItem(cast(u32) idxItem).iconMargin
                    if iconMargin
                        labelPos.moveLeft(list.iconSize + metrics.iconText_Margin)
                }
            }

            Theme.drawIconText(painter, labelPos, icon, &item.name, format, metrics.iconText_Margin)
        }
    }

    mtd hitTest(surfacePos: Math.Point)->u32
    {
        lc := list.getLineCount()
        res := surfaceToLocal(surfacePos)
        if res.x < 0 or res.y < 0
            return Swag.U32.Max
        res.y += scrollPos.y
        idx := cast(u32) (res.y / list.getLineHeight())
        if idx >= lc
            return Swag.U32.Max
        if !list.isVirtual and list.lines[idx].separator
            return Swag.U32.Max
        return idx
    }

    mtd getDisplayWidth()->f32
    {
        if list.horizontalExtent > 0
            return list.horizontalExtent
        return position.width
    }
}

impl ListCtrl
{
    #[Swag.Discardable]
    func createSimple(parent: *Wnd, position: Math.Rectangle = {}, id: WndId = null)->*ListCtrl
    {
        with list := Wnd.create'ListCtrl(parent, "", position, id)
        {
            .scrollWnd = .createView'ScrollWnd()
            .scrollWnd.dockStyle = .Center
            .scrollWnd.focusStrategy = .None
            .scrollWnd.setAllPaddings(.getThemeMetrics().list_Padding)

            .listView = .scrollWnd.createView'ListView()
            .listView.list = list
            .listView.wndFlags |= .NoScroll
        }

        return list
    }

    #[Swag.Discardable]
    func createMultiColumns(parent: *Wnd, position: Math.Rectangle = {}, id: WndId = null)->*ListCtrl
    {
        list := Wnd.create'ListCtrl(parent, "", position, id)

        fwnd := list.createView'Wnd()
        fwnd.dockStyle = .Center
        fwnd.wndFlags ^= .ClipChildren

        // Adapt header to the frame container
        list.hookOnEvent = func(wnd, evt) {
            if evt.kind != .Resize
                return false

            list   := cast(*ListCtrl) wnd
            header := list.header
            if header.form == .Transparent
                return false
            switch list.frameForm
            {
            case Round:
                header.form = .Round
                header.margin = -wnd.getThemeMetrics().frameWnd_RoundPadding
            case Square:
                header.form = .Flat
                header.margin = -wnd.getThemeMetrics().frameWnd_SquarePadding
            case Transparent:
                header.form = .Flat
                header.margin = -wnd.getThemeMetrics().frameWnd_FlatPadding
            }

            header.setMargin(bottom: 0)
            return false
        }

        with list
        {
            // Header
            // Need to recompute layout when header size has changed
            .header = Header.create(fwnd, {})
            .header.dockStyle = .Top
            .header.form = .Round
            .header.sigLayoutChanged = closure|list|(header) {
                list.computeLayout()
            }

            .header.sigClicked = closure|list|(header, col) {
                list.sort(col, header.sortMark <= 0 ? false : true, true)
            }

            // Scroll that contains the list view
            .scrollWnd = Wnd.create'ScrollWnd(fwnd, {})
            .scrollWnd.dockStyle = .Center
            .scrollWnd.focusStrategy = .None
            .scrollWnd.backgroundStyle = .Transparent
            .scrollWnd.setAllPaddings(.getThemeMetrics().list_Padding)
            .scrollWnd.sigScrollPosChanged += closure|list.header|(scrollWnd, op, np) {
                header.scrollPos.x = np.x
                header.invalidate()
            }

            // ListCtrl view
            .listView = .scrollWnd.createView'ListView()
            .listView.list = list
            .listView.wndFlags |= .NoScroll
        }

        return list
    }

    // Unselect all lines
    mtd unselectAll()
    {
        if selModel.isEmpty()
            return
        selModel.unselectAll()
        invalidate()
        sigSelChanged.call(self)
    }

    // Returns
    mtd isLineSelected(idx: u32)->bool
    {
        Debug.assert(idx < getLineCount())
        if selectionMode == .None
            return false
        if isVirtual
            return selModel.isSelected(idx)
        return !lines[idx].separator and selModel.isSelected(lines[idx].index)
    }

    // Select of unselect the given line
    mtd selectLine(idx: u32, select: bool = true)
    {
        lineCount := getLineCount()
        Debug.assert(idx < lineCount)

        if selectionMode == .None or lineCount == 0
            return
        if !isVirtual and lines[idx].separator
            return

        if selectionMode == .Single
            selModel.unselectAll()

        idxLine := isVirtual ? idx : lines[idx].index
        selModel.select(idxLine, select)
        invalidate()

        sigSelChanged.call(self)
    }

    // Select a range of lines
    mtd selectLines(start, end: u32, select: bool = true)
    {
        lineCount := getLineCount()
        Debug.assert(start < lineCount)
        Debug.assert(end < lineCount)

        if selectionMode == .None or lineCount == 0
            return

        loop i: start..end
        {
            idxLine := isVirtual ? i : lines[i].index
            selModel.select(idxLine, select)
        }

        invalidate()

        sigSelChanged.call(self)
    }

    // Compute global layout, once all lines have been added
    mtd computeLayout()
    {
        clientRect := scrollWnd.getClientRectPadding()

        var scrollWidth: f32 = undefined
        if horizontalExtent > 0
            scrollWidth = horizontalExtent
        elif header
            scrollWidth = header.getTotalItemsWidth()
        else
            scrollWidth = clientRect.width
        scrollHeight := getLineCount() * getLineHeight()
        scrollWnd.setScrollSize(scrollWidth, scrollHeight)
        invalidate()
    }

    // Get the height of one line
    mtd getLineHeight()->f32
    {
        return lineHeight orelse getFont().regular.height + 2
    }

    // Get a line by index
    mtd getLine(idx: u32)->*ListLine
    {
        return lines[idx]
    }

    // Returns the number of lines in the list
    mtd getLineCount()->u32
    {
        if isVirtual
            return virtualCount
        return cast(u32) lines.count
    }

    // Add one new line
    mtd createLine(parentLine: *ListLine)->*ListLine
    {
        Debug.assert(!isVirtual, "createLine cannot be called on a virtual list")

        if parentLine
        {
            line := parentLine.lines.newPtr()
            line.parent = parentLine
            line.level = parentLine.level + 1
            parentLine.lines.add(line)
            return line
        }

        line := toFreeLines.newPtr()
        line.index  = cast(u32) lines.count
        idxLineToIdxList.add(line.index, line.index)
        lines.add(line)
        toFreeLines.add(line)
        return line
    }

    // Add one line, and set first column
    #[Swag.Discardable]
    mtd createLine(name: string, icon: Icon = {}, parentLine: *ListLine = null)->*ListLine
    {
        line := createLine(parentLine)
        line.setItem(0, #self.name, #self.icon)
        return line
    }

    // Clear all lines
    mtd clear()
    {
        lines.clear()
        virtualCount = 0
        idxLineToIdxList.clear()
        focusIndex = Swag.U32.Max
        selModel.unselectAll()
        checkModel.unselectAll()
    }

    // Add one column in a multi columns list
    mtd addColumn(name: string, width: f32 = 100, icon: Icon = {})
    {
        Debug.assert(header != null)
        header.addItem(#self.name, width, #self.icon)
    }

    // Set the column width
    mtd setColumnWidth(idx: u32, width: f32)
    {
        Debug.assert(header != null)
        header.setItemWidth(idx, width)
    }

    // Set the number of lines in a virtual list
    mtd setVirtualCount(count: u32)
    {
        clear()
        isVirtual = true
        virtualCount = count
        computeLayout()
    }

    // Returns the first visible line index
    mtd getFirstVisibleLineIndex()->u32
    {
        lc := getLineCount()
        idx := cast(u32) (listView.scrollPos.y / getLineHeight())
        if idx >= lc
            return Swag.U32.Max
        return idx
    }

    // Returns the last visible line index
    mtd getLastVisibleLineIndex()->u32
    {
        lc := getLineCount()
        if lc == 0
            return Swag.U32.Max

        y := listView.scrollPos.y + scrollWnd.view.position.height
        y -= scrollWnd.view.padding.y
        y -= scrollWnd.view.padding.w
        if y < 0
            return cast(u32) (listView.scrollPos.y / getLineHeight())
        idx := cast(u32) (y / getLineHeight())
        if idx >= lc
            return cast(u32) lc - 1
        return idx
    }

    // Set the corresponding line visible
    mtd ensureVisibleLine(idx: u32)
    {
        first := getFirstVisibleLineIndex()
        last  := getLastVisibleLineIndex()
        if idx > first and idx < last
            return
        sc := scrollWnd.getScrollPos()
        scrollWnd.setScrollPos(sc.x, idx * getLineHeight())
    }

    // Returns true if the line at the given index is selected
    mtd const isSelectedLine(idx: u32)->bool
    {
        idxLine := isVirtual ? idx : lines[idx].index
        return selModel.isSelected(idxLine)
    }

    // Get the line with the keyboard focus
    mtd const getFocusLine()->*ListLine
    {
        if focusIndex >= lines.count
            return null
        return lines[focusIndex]
    }

    // Get all the selected lines
    mtd const getSelectedLines()->Array'(*ListLine)
    {
        var result: retval
        if isVirtual or selModel.isEmpty()
            return result

        visit i: selModel
        {
            idx := idxLineToIdxList.find(i).value
            result.add(lines[idx])
        }

        return result
    }

    // Get all the checked lines
    mtd const getCheckedLines()->Array'(*ListLine)
    {
        var result: retval
        if isVirtual or checkModel.isEmpty()
            return result

        visit i: checkModel
        {
            idx := idxLineToIdxList.find(i).value
            result.add(lines[idx])
        }

        return result
    }

    // Returns a selected line
    // Mostly for singlesel mode
    mtd const getSelectedLine()->*ListLine
    {
        if isVirtual or selModel.isEmpty()
            return null
        range := selModel.selection.front()
        idx   := idxLineToIdxList.find(range.start).value
        return lines[idx]
    }

    private mtd mapIndexes()
    {
        idxLineToIdxList.clear()
        visit l, idx: lines
            idxLineToIdxList.add(l.index, cast(u32) idx)
    }

    private mtd sort(column: u32, descentOrder: bool, toSort: *Array'*ListLine)
    {
        func sortFunc(column: u32, descentOrder: bool, a, b: *ListLine)->s32
        {
            it0 := a.getItem(column)
            it1 := b.getItem(column)
            sort0 := it0.sortKey.isEmpty() ? &it0.name.raw : &it0.sortKey
            sort1 := it1.sortKey.isEmpty() ? &it1.name.raw : &it1.sortKey
            res := Latin1.compareNatural(dref sort0, dref sort1)
            if descentOrder res *= -1
            return res
        }

        cb := closure|self, column, descentOrder|(a, b: *ListLine)->s32
        {
            if self.sigSort
                return self.sigSort(a, b, column, descentOrder)

            if a.parent != b.parent
            {
                if a.isParentOf(b) return -1
                if b.isParentOf(a) return 1

                a1 := a
                b1 := b
                while a1.level > b1.level a1 = a1.parent
                while b1.level > a1.level b1 = b1.parent
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
    mtd sort(column: u32, descentOrder, persistent = false)
    {
        if isVirtual
            return

        sort(column, descentOrder, &lines)
        mapIndexes()

        if header and persistent
        {
            header.sortColumn = column
            header.sortMark = descentOrder ? -1 : 1
            header.invalidate()
        }

        invalidate()
    }

    // Sort list as before
    mtd sort()
    {
        if header and header.sortMark != 0
            sort(header.sortColumn, header.sortMark == -1 ? true : false)
        else
            sort(0)
    }

    // Expand the given line
    mtd expand(line: *ListLine)
    {
        if !line.canExpand() or line.expanded
            return
        line.expanded = true
        sigExpand.call(line)

        var tmp: Array'(*ListLine)
        tmp.add(line)

        // Auto sort
        if header and header.sortMark
            sort(header.sortColumn, header.sortMark == -1 ? true : false, &line.lines)

        cptIdxLine := cast(u32) lines.count
        cptIdx     := 0'u32
        while tmp.count
        {
            to := tmp.popBack()
            idxInsert := idxLineToIdxList.find(to.index).value
            visit l, i: to.lines
            {
                l.index = cptIdxLine
                cptIdxLine += 1
                idxLineToIdxList.add(l.index, idxInsert + cptIdx + 1)
                cptIdx += 1
                if l.expanded
                    tmp.add(l)
            }

            // Update indexes after the insertion
            for idx := idxInsert + 1; idx < cast(u32) lines.count; idx += 1
                idxLineToIdxList.add(lines[idx].index, cast(u32) (idx + to.lines.count))

            // Insert at the right position
            lines.insertAt(cast(u64) (idxInsert + 1), to.lines.toSlice())
        }

        computeLayout()
    }

    // Collapse the given line
    mtd collapse(line: *ListLine)
    {
        if !line.canExpand() or !line.expanded
            return
        line.expanded = false
        sigCollapse.call(line)

        var tmp: Array'(*ListLine)
        tmp.add(line)

        removeCount := 0'u32
        while tmp.count
        {
            to := tmp.popBack()
            visit l: to.lines
            {
                removeCount += 1
                if l.expanded
                    tmp.add(l)
            }
        }

        // Insert at the right position
        idxInsert := idxLineToIdxList.find(line.index).value
        lines.removeAtOrdered(cast(u64) (idxInsert + 1), cast(u64) removeCount)

        // Update indexes after the insertion
        for idx := idxInsert + 1; idx < cast(u32) lines.count; idx += 1
            idxLineToIdxList.add(lines[idx].index, idx)

        // Update selection
        if focusIndex >= idxInsert + 1 and focusIndex <= idxInsert + 1 + removeCount
        {
            unselectAll()
            focusIndex = idxInsert
            selectLine(focusIndex)
        }

        computeLayout()
    }

    // Collapse all lines
    mtd collapseAll()
    {
        func collapseRec(sl: [..] *ListLine)
        {
            visit l: sl
            {
                l.expanded = false
                collapseRec(l.lines.toSlice())
            }
        }

        visit l: lines
        {
            if l.expanded
                collapse(l)
        }

        collapseRec(lines.toSlice())
        computeLayout()
    }

    // Collapse the given line
    mtd toggleExpand(line: *ListLine)
    {
        if line.expanded
            collapse(line)
        else
            expand(line)
    }

    // Set focus to the list
    mtd setFocus()
    {
        listView.setFocus()
    }
}