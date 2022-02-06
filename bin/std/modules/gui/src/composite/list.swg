#global public
using Core, Pixel

struct ListItem
{
    userData:   *void
    name:       RichString
    sortKey:    String
    icon:       Icon
    colorBk:    Color
    colorFg:    Color
    column:     u32
}

struct ListLine
{
    items:      Array'(*ListItem)
    colorBk:    Color
    colorFg:    Color
    userData:   *void
    index:      u32
}

impl ListLine
{
    mtd opDrop()
    {
        visit item: items
            Memory.delete(item)
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

        item := Memory.new'ListItem()
        item.column = column
        item.name = name
        item.icon = icon
        items.add(item)
    }
}

enum ListSelectionMode
{
    None
    Single
    MultiSimple
    MultiExtended
}

#[Swag.EnumFlags]
enum ListFlags
{
    None
    HideSelection
    AlternateLines
    Checkable
}

struct List
{
    using frameWnd: FrameWnd

    horizontalExtent:   f32
    lineHeight:         f32 = 20
    iconSize:           f32
    listFlags           = ListFlags.None
    selectionMode       = ListSelectionMode.Single
    sigVirtualFill:     func(*Self, *ListLine, u32)
    sigSelChanged:      func(*Self)
    sigCheckChanged:    func(*Self)
    sigSort:            func(*ListLine, *ListLine, u32, bool)->s32

    lines:          Array'(*ListLine)
    idxLineToIdx:   HashTable'(u32, u32)
    selModel:       SelModel
    checkModel:     SelModel
    focusIndex:     u32 = Swag.U32.Max
    scrollWnd:      *ScrollWnd
    listView:       *ListView
    header:         *Header
    virtualCount:   u32
    isVirtual:      bool
}

struct ListView
{
    using wnd:              Wnd
    list:                   *List
    discardMouseRelease:    bool
}

impl IWnd for ListView
{
    mtd onMouseEvent(evt: *MouseEvent)
    {
        if evt.kind != .MousePressed and evt.kind != .MouseReleased
        {
            evt.accepted = false
            return
        }

        if evt.button != .Left
            return
        if list.selectionMode == .None
            return

        idx := hitTest(evt.surfacePos)

        // On checkbox
        if list.listFlags & .Checkable and idx != Swag.U32.Max
        {
            localPos := surfaceToLocal(evt.surfacePos)
            localPos.x += scrollPos.x
            if localPos.x < getThemeMetrics().btnCheck_Size
            {
                if evt.kind == .MouseReleased
                    return
                idxLine := list.isVirtual ? idx : list.lines[idx].index
                checked := !list.checkModel.isSelected(idxLine)
                if list.isLineSelected(idx)
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

        ctrl  := evt.modifiers & .Control
        shift := evt.modifiers & .Shift

        if list.selectionMode != .MultiSimple
        {
            if list.selectionMode == .Single or (!ctrl and !shift)
            {
                list.selModel.unselectAll()
            }
        }

        if idx != Swag.U32.Max
        {
            // Range selection
            if shift and list.selectionMode == .MultiExtended
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

        var rect: Math.Rectangle
        rect.width = getDisplayWidth()
        rect.height = list.lineHeight

        firstIdx := list.getFirstVisibleLineIndex()
        lastIdx  := list.getLastVisibleLineIndex()
        rect.y = firstIdx * list.lineHeight

        if list.isVirtual
        {
            Debug.assert(list.sigVirtualFill != null)
            loop i: firstIdx..lastIdx
            {
                var line: ListLine
                list.sigVirtualFill(&line, i)
                paintLine(bc, i, &line, rect)
                rect.y = rect.bottom()
            }
        }
        else
        {
            loop i: firstIdx..lastIdx
            {
                line := list.lines[i]
                paintLine(bc, i, line, rect)
                rect.y = rect.bottom()
            }
        }
    }
}

private impl ListView
{
    mtd paintLine(bc: PaintContext, idx: u32, line: *ListLine, rect: Math.Rectangle)
    {
        painter := bc.painter
        theme   := getTheme()
        colors  := getThemeColors()
        metrics := getThemeMetrics()
        rects   := getThemeRects()
        font    := theme.res.fontDefault.regular

        posSel := rect
        posSel.x += scrollPos.x

        // Selection box
        isSelected := list.isLineSelected(idx)
        hideSel := cast(bool) (list.listFlags & .HideSelection)
        if isSelected and (!hideSel or hasFocus()) and !bc.isDisabled
        {
            colorSel := hasFocus() ? colors.listItem_FocusSelectedBk : colors.listItem_SelectedBk
            painter.fillRect(posSel, colorSel)
        }

        // User background
        elif line.colorBk.a
        {
            painter.fillRect(posSel, line.colorBk)
        }

        // Alternate lines
        elif (list.listFlags & .AlternateLines) and idx % 2 == 1
        {
            painter.fillRect(posSel, colors.listItem_AlternateBk)
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
            colorFg := item.colorFg.a ? item.colorFg : colorText
            theme.setTextColors(&format, colorFg)
            format.horzAlignment = .Left
            format.vertAlignment = .Center
            format.flags = .Clip
            format.font = theme.res.fontDefault

            labelPos := rect

            // Right
            if list.header
            {
                labelPos.x = list.header.getItemPositionByCol(item.column)
                labelPos.width = list.header.getItemWidthByCol(item.column)
            }

            // Background color ?
            if item.colorBk.a
                painter.fillRect(labelPos, item.colorBk)

            // Checkbox
            if idxItem == 0 and list.listFlags & .Checkable
            {
                var subRectBorder:  *ThemeImageRect
                var subRectMark:    *ThemeImageRect

                idxLine := list.isVirtual ? idx : list.lines[idx].index
                checked := list.checkModel.isSelected(idxLine)

                if checked
                {
                    subRectBorder = &rects.btnCheck_CheckedBorder
                    subRectMark   = &rects.btnCheck_CheckedMark
                }
                else
                {
                    subRectBorder = &rects.btnCheck_NormalBorder
                }

                posBox := labelPos
                posBox.height = metrics.btnCheck_Size
                posBox.y = labelPos.vertCenter() - (posBox.height * 0.5)
                posBox.width = posBox.height
                theme.drawSubRect(painter, posBox, subRectMark, colorFg)
                theme.drawSubRect(painter, posBox, subRectBorder, colorFg)

                labelPos.moveLeft(metrics.btnCheck_Size + 4)
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
                    icon.size = list.iconSize
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

impl List
{
    #[Swag.Discardable]
    func createSimple(parent: *Wnd, position: Math.Rectangle, id: string = null)->*List
    {
        list := Wnd.create'List(parent, "", position, id)

        with list
        {
            scrollWnd = createView'ScrollWnd()
            scrollWnd.dockStyle = .Center
            scrollWnd.focusStrategy = .None
            scrollWnd.backgroundStyle = .Transparent
            scrollWnd.setAllPaddings(getThemeMetrics().list_Padding)

            listView = scrollWnd.createView'ListView()
            listView.backgroundStyle = .Transparent
            listView.list = list
        }

        return list
    }

    #[Swag.Discardable]
    func createMultiColumns(parent: *Wnd, position: Math.Rectangle, id: string = null)->*List
    {
        list := Wnd.create'List(parent, "", position, id)

        fwnd := list.createView'Wnd()
        fwnd.dockStyle = .Center
        fwnd.backgroundStyle = .Transparent
        fwnd.wndFlags ^= .ClipChildren

        // Adapt header to the frame container
        list.hookOnEvent = @(wnd, evt)->bool
        {
            if evt.kind != .Resize
                return false

            list   := cast(*List) wnd
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
            scrollWnd.sigScrollPosChanged = @|header|(scrollWnd, sp) {
                header.scrollPos.x = sp.x
                header.invalidate()
            }

            // List view
            listView = scrollWnd.createView'ListView()
            listView.backgroundStyle = .Transparent
            listView.list = list
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
        Debug.assert(idx < getLineCount())

        if selectionMode == .None or lines.isEmpty()
            return

        selModel.select(lines[idx].index, select)
        invalidate()

        if sigSelChanged
            sigSelChanged(self)
    }

    // Select a range of lines
    mtd selectLines(start, end: u32, select: bool = true)
    {
        Debug.assert(start < getLineCount())
        Debug.assert(end < getLineCount())

        if selectionMode == .None or lines.isEmpty()
            return

        loop i: start..end
            selModel.select(lines[i].index, select)
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
    mtd createLine()->*ListLine
    {
        Debug.assert(!isVirtual, "createLine cannot be called on a virtual list")
        line := Memory.new'ListLine()
        line.index = cast(u32) lines.count
        idxLineToIdx.add(line.index, line.index)
        lines.add(line)
        return line
    }

    // Add one line, and set first column
    mtd createLine(name: string, icon: Icon = @{})->*ListLine
    {
        line := createLine()
        line.setItem(0, #self.name, #self.icon)
        return line
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
        Debug.assert(lines.isEmpty(), "setVirtualCount must only be called on a virtual list")
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
        idx := cast(u32) ((listView.scrollPos.y + position.height) / lineHeight)
        if idx >= lc
            return cast(u32) lc - 1
        return idx
    }

    // Set the corresponding line visible
    mtd ensureVisibleLine(idx: u32)
    {
        first := getFirstVisibleLineIndex()
        last  := getLastVisibleLineIndex()
        if idx >= first and idx <= last
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
    mtdc getSelectedLines()->Array'(*ListLine)
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
    mtdc getCheckedLines()->Array'(*ListLine)
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
    mtdc getSelectedLine()->*ListLine
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

        cb := @|self, column, descentOrder|(a, b: *ListLine)->s32
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