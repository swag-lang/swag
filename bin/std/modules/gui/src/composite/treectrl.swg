#global public
using Core, Pixel

struct TreeItem
{
    using item: Item
    sortKey:    String
    colorBk:    Color
    colorFg:    Color
    column:     u32
}

struct TreeLine
{
    items:      ArrayPtr'TreeItem
    colorBk:    Color
    colorFg:    Color
    userData:   *void
    index:      u32
}

impl TreeLine
{
    // Get an item by index
    mtd getItem(column: u32)->*TreeItem
    {
        visit item: items
        {
            if item.column == column
                return item
        }

        return null
    }

    // Initialize a given column item
    mtd setItem(column: u32, name: string, icon: Icon = @{})
    {
        visit item: items
        {
            if item.column == column
            {
                item.name = name
                item.icon = icon
                return
            }
        }

        item := items.newPtr()
        item.column = column
        item.name = name
        item.icon = icon
        items.add(item)
    }
}

enum TreeSelectionMode
{
    None
    Single
    Multi
}

#[Swag.EnumFlags]
enum TreeFlags
{
    None
    HideSelection
    AlternateLines
    Checkable
    HorzLines
    VertLines
}

struct TreeCtrl
{
    using frameWnd: FrameWnd

    horizontalExtent:   f32
    lineHeight:         f32 = 22
    iconSize:           f32
    treeFlags           = TreeFlags.None
    selectionMode       = TreeSelectionMode.Single
    sigVirtualFill:     func(*Self, *TreeLine, u32)
    sigSelChanged:      func(*Self)
    sigCheckChanged:    func(*Self)
    sigRightClick:      func(*Self, Math.Point)
    sigLeftDoubleClick: func(*Self, Math.Point)
    sigSort:            func(*TreeLine, *TreeLine, u32, bool)->s32

    lines:          ArrayPtr'TreeLine
    idxLineToIdx:   HashTable'(u32, u32)
    selModel:       SelModel
    checkModel:     SelModel
    focusIndex:     u32 = Swag.U32.Max
    scrollWnd:      *ScrollWnd
    listView:       *TreeView
    header:         *Header
    virtualCount:   u32
    isVirtual:      bool
    nextTreeIndex:  u32
}

struct TreeView
{
    using wnd:              Wnd
    list:                   *TreeCtrl
    discardMouseRelease:    bool
}

impl IWnd for TreeView
{
    mtd onKeyEvent(evt: *KeyEvent)
    {
        evt.accepted = true
        if evt.kind != .KeyPressed
            return

        lineCount := list.getLineCount()
        if !lineCount
            return
        if list.focusIndex == Swag.U32.Max
            return

        shift := cast(bool) (evt.modifiers & .Shift)
        ctrl  := cast(bool) (evt.modifiers & .Control)

        selChanged := false
        var startSelIdx, endSelIdx: u32

        switch evt.key
        {
        case Space:
            if !(list.treeFlags & .Checkable)
                break

            visit l: list.selModel
            {
                checked := !list.checkModel.isSelected(l)
                list.checkModel.select(l, checked)
            }

            if list.sigCheckChanged
                list.sigCheckChanged(list)
            invalidate()

        case Enter:
            if !(list.treeFlags & .Checkable)
                break
            visit l: list.selModel
                list.checkModel.select(l, true)
            if list.sigCheckChanged
                list.sigCheckChanged(list)
            invalidate()

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
            else
                startSelIdx, endSelIdx = list.focusIndex + 1
            selChanged = true

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

    mtd onMouseEvent(evt: *MouseEvent)
    {
        if evt.kind != .MousePressed and evt.kind != .MouseReleased and evt.kind != .MouseDoubleClick
        {
            evt.accepted = false
            return
        }

        idx := hitTest(evt.surfacePos)

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
                    if list.sigRightClick
                    {
                        pt := surfaceToLocal(evt.surfacePos)
                        list.sigRightClick(list, pt)
                    }
                }
            }

            return
        }

        // Check boxes
        // ============================

        if evt.button != .Left
            return

        // On checkbox
        if list.treeFlags & .Checkable and idx != Swag.U32.Max
        {
            eatCheck := false
            if evt.kind == .MouseDoubleClick
                eatCheck = true
            else
            {
                localPos := surfaceToLocal(evt.surfacePos)
                localPos.x += scrollPos.x
                if localPos.x < Math.min(getThemeMetrics().btnCheck_Size, list.lineHeight)
                    eatCheck = true
            }

            if eatCheck
            {
                if evt.kind == .MouseReleased
                    return
                idxLine := list.isVirtual ? idx : list.lines[idx].index
                checked := !list.checkModel.isSelected(idxLine)
                if list.selModel.isSelected(idxLine)
                {
                    visit l: list.selModel
                        list.checkModel.select(l, checked)
                }
                else
                    list.checkModel.select(idxLine, checked)
                if list.sigCheckChanged
                    list.sigCheckChanged(list)
                invalidate()
                return
            }
        }

        // User double click
        // ============================

        if evt.kind == .MouseDoubleClick
        {
            if list.sigLeftDoubleClick
            {
                pt := surfaceToLocal(evt.surfacePos)
                list.sigLeftDoubleClick(list, pt)
            }

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

        if list.selectionMode == .Single or (!ctrl and !shift)
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

        list.focusIndex = idx
        invalidate()
        if list.sigSelChanged
            list.sigSelChanged()
    }

    mtd onPaintEvent(evt: *PaintEvent)
    {
        if list.getLineCount() == 0
            return

        bc      := evt.bc
        painter := bc.painter
        colors  := getThemeColors()
        metrics := getThemeMetrics()

        var rect: Math.Rectangle
        rect.width = getDisplayWidth()
        rect.height = list.lineHeight

        // Clipping, take care of padding
        clipRect := getSurfaceRect()
        clipRect.applyPadding(padding)

        firstIdx := list.getFirstVisibleLineIndex()
        lastIdx  := list.getLastVisibleLineIndex()

        // Do not display with bigs coordinates to avoid precision issues
        // So we do the scroll by hand, and draw will small coordinates
        y := cast(f64) firstIdx * list.lineHeight
        y -= scrollPos.y
        rect.y = cast(f32) y

        if list.isVirtual
        {
            Debug.assert(list.sigVirtualFill != null)
            loop i: firstIdx..lastIdx
            {
                var line: TreeLine
                list.sigVirtualFill(&line, i)
                paintLine(bc, i, &line, rect)
                rect.y = rect.bottom()

                if list.treeFlags & .HorzLines
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

                if list.treeFlags & .HorzLines
                    painter.drawLine(rect.x, rect.y, rect.right(), rect.y, colors.listItem_Line, 1)
            }
        }

        if list.header and list.treeFlags & .VertLines
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

private impl TreeView
{
    mtd getXPos(column: u32)->f32
    {
        x := list.header.getItemPositionByCol(column)
        x += list.header.getSurfaceRect().x
        x -= getSurfaceRect().x
        return x
    }

    mtd paintLine(bc: PaintContext, idx: u32, line: *TreeLine, rect: Math.Rectangle)
    {
        painter := bc.painter
        theme   := getTheme()
        colors  := getThemeColors()
        metrics := getThemeMetrics()
        rects   := getThemeRects()
        font    := theme.res.fontDefault.regular

        // Selection box
        isSelected := list.isLineSelected(idx)
        hideSel := cast(bool) (list.treeFlags & .HideSelection)
        if isSelected and (!hideSel or hasFocus()) and !bc.isDisabled
        {
            colorSel := hasFocus() ? colors.listItem_FocusSelectedBk : colors.listItem_SelectedBk
            painter.fillRect(rect, colorSel)
        }

        // User background
        elif line.colorBk.a
        {
            painter.fillRect(rect, line.colorBk)
        }

        // Alternate lines
        elif (list.treeFlags & .AlternateLines) and idx % 2 == 1
        {
            painter.fillRect(rect, colors.listItem_AlternateBk)
        }

        var colorText: Color = ?
        switch
        {
            case bc.isDisabled:
                colorText = colors.listItem_DisabledText
            case isSelected:
                colorText = colors.listItem_SelectedText
            case line.colorFg.a:
                colorText = line.colorFg
            default:
                colorText = colors.listItem_Text
        }

        var format: RichStringFormat
        visit item, idxItem: line.items
        {
            var colorFg: Color = ?
            if item.colorFg.a and !isSelected
                colorFg = item.colorFg
            else
                colorFg = colorText
            theme.setTextColors(&format, colorFg)
            format.horzAlignment = .Left
            format.vertAlignment = .Center
            format.flags = .Clip
            format.font = theme.res.fontDefault

            labelPos := rect

            // Right
            if list.header
            {
                labelPos.x = getXPos(item.column)
                labelPos.width = list.header.getItemWidthByCol(item.column)
            }

            labelPos.x -= scrollPos.x

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
            if idxItem == 0 and list.treeFlags & .Checkable
            {
                var subRectBorder:  *ThemeImageRect
                var subRectMark:    *ThemeImageRect

                idxLine := list.isVirtual ? idx : list.lines[idx].index
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

                widthCheck := Math.min(metrics.btnCheck_Size, list.lineHeight)
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
                if !icon.isValid()
                    labelPos.moveLeft(list.iconSize + metrics.iconText_Margin)
                else
                {
                    icon.sizeX = list.iconSize
                    icon.sizeY = list.iconSize
                }
            }

            Theme.drawIconText(painter, labelPos, icon, &item.name, format, metrics.iconText_Margin)
        }
    }

    mtd hitTest(surfacePos: Math.Point)->u32
    {
        lc := list.getLineCount()
        res := surfaceToLocal(surfacePos)
        res.y += scrollPos.y
        idx := cast(u32) (res.y / list.lineHeight)
        if idx >= lc
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

impl TreeCtrl
{
    #[Swag.Discardable]
    func createSimple(parent: *Wnd, position: Math.Rectangle, id: WndId = null)->*TreeCtrl
    {
        list := Wnd.create'TreeCtrl(parent, "", position, id)

        with list
        {
            scrollWnd = createView'ScrollWnd()
            scrollWnd.dockStyle = .Center
            scrollWnd.focusStrategy = .None
            scrollWnd.backgroundStyle = .Transparent
            scrollWnd.setAllPaddings(getThemeMetrics().list_Padding)

            listView = scrollWnd.createView'TreeView()
            listView.backgroundStyle = .Transparent
            listView.list = list
            listView.wndFlags |= .NoScroll
        }

        return list
    }

    #[Swag.Discardable]
    func createMultiColumns(parent: *Wnd, position: Math.Rectangle, id: WndId = null)->*TreeCtrl
    {
        list := Wnd.create'TreeCtrl(parent, "", position, id)

        fwnd := list.createView'Wnd()
        fwnd.dockStyle = .Center
        fwnd.backgroundStyle = .Transparent
        fwnd.wndFlags ^= .ClipChildren

        // Adapt header to the frame container
        list.hookOnEvent = @(wnd, evt)
        {
            if evt.kind != .Resize
                return false

            list   := cast(*TreeCtrl) wnd
            header := list.header
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
            header = Header.create(fwnd, @{})
            header.dockStyle = .Top
            header.form = .Round
            header.sigLayoutChanged = @|list|(header) {
                list.computeLayout()
            }

            header.sigClicked = @|list|(header, col) {
                list.sort(col, header.sortMark <= 0 ? false : true, true)
            }

            // Scroll that contains the list view
            scrollWnd = Wnd.create'ScrollWnd(fwnd, @{})
            scrollWnd.dockStyle = .Center
            scrollWnd.focusStrategy = .None
            scrollWnd.backgroundStyle = .Transparent
            scrollWnd.setAllPaddings(getThemeMetrics().list_Padding)
            scrollWnd.sigScrollPosChanged += @|header|(scrollWnd, op, np) {
                header.scrollPos.x = np.x
                header.invalidate()
            }

            // TreeCtrl view
            listView = scrollWnd.createView'TreeView()
            listView.backgroundStyle = .Transparent
            listView.list = list
            listView.wndFlags |= .NoScroll
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
        if sigSelChanged
            sigSelChanged(self)
    }

    // Returns
    mtd isLineSelected(idx: u32)->bool
    {
        Debug.assert(idx < getLineCount())
        if selectionMode == .None
            return false
        if isVirtual
            return selModel.isSelected(idx)
        return selModel.isSelected(lines[idx].index)
    }

    // Select of unselect the given line
    mtd selectLine(idx: u32, select: bool = true)
    {
        lineCount := getLineCount()
        Debug.assert(idx < lineCount)

        if selectionMode == .None or lineCount == 0
            return

        idxLine := isVirtual ? idx : lines[idx].index
        selModel.select(idxLine, select)
        invalidate()

        if sigSelChanged
            sigSelChanged(self)
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

        if sigSelChanged
            sigSelChanged(self)
    }

    // Compute global layout, once all lines have been added
    mtd computeLayout()
    {
        clientRect := scrollWnd.getClientRectPadding()

        var scrollWidth: f32 = ?
        if horizontalExtent > 0
            scrollWidth = horizontalExtent
        elif header
            scrollWidth = header.getTotalItemsWidth()
        else
            scrollWidth = clientRect.width
        scrollHeight := getLineCount() * lineHeight
        scrollWnd.setScrollSize(scrollWidth, scrollHeight)
        invalidate()
    }

    // Get a line by index
    mtd getLine(idx: u32)->*TreeLine
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
    mtd createLine()->*TreeLine
    {
        Debug.assert(!isVirtual, "createLine cannot be called on a virtual list")

        line := lines.newPtr()
        line.index = nextTreeIndex
        idxLineToIdx.add(line.index, line.index)
        nextTreeIndex += 1
        lines.add(line)
        return line
    }

    // Add one line, and set first column
    mtd createLine(name: string, icon: Icon = @{})->*TreeLine
    {
        line := createLine()
        line.setItem(0, #self.name, #self.icon)
        return line
    }

    // Clear all lines
    mtd clear()
    {
        lines.clear()
        virtualCount = 0
        idxLineToIdx.clear()
    }

    // Add one column in a multi columns list
    mtd addColumn(name: string, width: f32 = 100, icon: Icon = @{})
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
        idx := cast(u32) (listView.scrollPos.y / lineHeight)
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
            return cast(u32) (listView.scrollPos.y / lineHeight)
        idx := cast(u32) (y / lineHeight)
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
        scrollWnd.setScrollPos(sc.x, idx * lineHeight)
    }

    // Returns true if the line at the given index is selected
    mtdc isSelectedLine(idx: u32)->bool
    {
        idxLine := isVirtual ? idx : lines[idx].index
        return selModel.isSelected(idxLine)
    }

    // Get all the selected lines
    mtdc getSelectedLines()->Array'(*TreeLine)
    {
        var result: retval
        if isVirtual or selModel.isEmpty()
            return result

        visit i: selModel
        {
            idx := idxLineToIdx.find(i).value
            result.add(lines[idx])
        }

        return result
    }

    // Get all the checked lines
    mtdc getCheckedLines()->Array'(*TreeLine)
    {
        var result: retval
        if isVirtual or checkModel.isEmpty()
            return result

        visit i: checkModel
        {
            idx := idxLineToIdx.find(i).value
            result.add(lines[idx])
        }

        return result
    }

    // Returns a selected line
    // Mostly for singlesel mode
    mtdc getSelectedLine()->*TreeLine
    {
        if isVirtual or selModel.isEmpty()
            return null
        range := selModel.selection.front()
        idx   := idxLineToIdx.find(range.start).value
        return lines[idx]
    }

    private mtd mapIndexes()
    {
        idxLineToIdx.clear()
        visit l, idx: lines
            idxLineToIdx.add(l.index, cast(u32) idx)
    }

    // Sort the list by a given column
    // If 'persistent' is true, then the sorted column will be displayed in the
    // header (if it exists)
    mtd sort(column: u32, descentOrder, persistent = false)
    {
        if isVirtual
            return

        cb := @|self, column, descentOrder|(a, b: *TreeLine)->s32
        {
            if self.sigSort
                return self.sigSort(a, b, column, descentOrder)
            it0 := a.getItem(column)
            it1 := b.getItem(column)
            sort0 := it0.sortKey.isEmpty() ? &it0.name.raw : &it0.sortKey
            sort1 := it1.sortKey.isEmpty() ? &it1.name.raw : &it1.sortKey
            res := Latin1.compareNatural(dref sort0, dref sort1)
            if descentOrder res *= -1
            return res
        }

        lines.sort(cb)
        mapIndexes()

        if header and persistent
        {
            header.sortColumn = column
            header.sortMark = descentOrder ? -1 : 1
            header.invalidate()
        }

        invalidate()
    }
}