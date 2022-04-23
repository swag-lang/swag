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
    lines:      Array'(*TreeLine)
    parent:     *TreeLine
    items:      ArrayPtr'TreeItem
    colorBk:    Color
    colorFg:    Color
    userData:   *void
    index:      u32
    level:      u32
    expanded:   bool
    visible:    bool
    forceExpandMark: bool
}

impl TreeLine
{
    mtd isVisible()->bool
    {
        if !parent return true
        return parent.expanded and parent.isVisible()
    }

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
    indentWidth:        f32 = 10
    iconSize:           f32
    treeFlags           = TreeFlags.None
    selectionMode       = TreeSelectionMode.Single
    sigSelChanged:      SigArray'closure(*Self)
    sigCheckChanged:    SigArray'closure(*Self)
    sigRightClick:      SigArray'closure(*Self, Math.Point)
    sigLeftDoubleClick: SigArray'closure(*Self, Math.Point)
    sigSort:            func(*TreeLine, *TreeLine, u32, bool)->s32

    lines:              ArrayPtr'TreeLine
    idxLineToIdx:       HashTable'(u32, u32)
    selModel:           SelModel
    checkModel:         SelModel
    focusIndex:         u32 = Swag.U32.Max
    scrollWnd:          *ScrollWnd
    treeView:           *TreeView
    header:             *Header
    nextTreeIndex:      u32

    recomputeIndexes:               bool = true
    visibleLineCount:       u32
    firstVisibleIndex:      u32
    firstVisibleLineIndex:  u32
    lastVisibleLineIndex:   u32
}

struct TreeView
{
    using wnd:              Wnd
    tree:                   *TreeCtrl
    discardMouseRelease:    bool
}

impl IWnd for TreeView
{
    mtd onKeyEvent(evt: *KeyEvent)
    {
        evt.accepted = true
        if evt.kind != .KeyPressed
            return

        lineCount := tree.getLineCount()
        if !lineCount
            return
        if tree.focusIndex == Swag.U32.Max
            return
        focusLine := tree.lines[tree.focusIndex]

        shift := cast(bool) (evt.modifiers & .Shift)
        ctrl  := cast(bool) (evt.modifiers & .Control)

        selChanged := false
        var startSelIdx, endSelIdx: u32

        switch evt.key
        {
        case Space:
            if tree.treeFlags & .Checkable
            {
                visit l: tree.selModel
                {
                    checked := !tree.checkModel.isSelected(l)
                    tree.checkModel.select(l, checked)
                }

                tree.sigCheckChanged.call(tree)
                invalidate()
            }
            else
            {
                tree.toggleExpand(focusLine)
            }

        case Enter:
            if tree.treeFlags & .Checkable
            {
                visit l: tree.selModel
                    tree.checkModel.select(l, true)
                tree.sigCheckChanged.call(tree)
                invalidate()
            }
            else
            {
                tree.toggleExpand(focusLine)
            }

        case A:
            if !ctrl or tree.selectionMode == .Single
                break
            shift = true
            tree.selModel.unselectAll()
            endSelIdx = 0
            startSelIdx = tree.getLineCount() - 1
            selChanged = true

        case Home:
            startSelIdx = tree.focusIndex
            endSelIdx = 0
            selChanged = true

        case End:
            startSelIdx = tree.focusIndex
            endSelIdx = lineCount - 1
            selChanged = true

        case Up:
            if tree.focusIndex == 0
            {
                startSelIdx, endSelIdx = 0
                selChanged = true
            }
            else
            {
                prev := tree.getPrevVisibleLineIndex(tree.focusIndex)
                if prev != Swag.U32.Max
                {
                    startSelIdx, endSelIdx = prev
                    selChanged = true
                }
            }

        case Down:
            if tree.focusIndex == lineCount - 1
            {
                startSelIdx, endSelIdx = lineCount - 1
                selChanged = true
            }
            else
            {
                next := tree.getNextVisibleLineIndex(tree.focusIndex)
                if next != Swag.U32.Max
                {
                    startSelIdx, endSelIdx = next
                    selChanged = true
                }
            }

        case Left:
            tree.collapse(focusLine)
        case Right:
            tree.expand(focusLine)


        case PageDown:
            first := tree.firstVisibleLineIndex
            last  := tree.lastVisibleLineIndex
            startSelIdx, endSelIdx = tree.focusIndex + (last - first)
            startSelIdx = Math.min(startSelIdx, lineCount - 1)
            endSelIdx = Math.min(endSelIdx, lineCount - 1)
            selChanged = true
            break

        case PageUp:
            first := tree.firstVisibleLineIndex
            last  := tree.lastVisibleLineIndex
            offset := last - first
            if offset > tree.focusIndex
                startSelIdx, endSelIdx = 0
            else
                startSelIdx, endSelIdx = tree.focusIndex - offset
            selChanged = true
            break
        }

        if selChanged
        {
            if !shift or tree.selectionMode == .Single
            {
                tree.selModel.unselectAll()
                startSelIdx = endSelIdx
            }

            loop i: startSelIdx..endSelIdx
                tree.selectLine(i, true)
            tree.focusIndex = endSelIdx
            tree.ensureVisibleLine(tree.focusIndex)
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
                    idxLine := tree.lines[idx].index
                    if !tree.selModel.isSelected(idxLine)
                    {
                        tree.selModel.unselectAll()
                        tree.selModel.select(idxLine, true)
                        tree.focusIndex = idx
                        invalidate()
                    }
                }
                elif evt.kind == .MouseReleased
                {
                    pt := surfaceToLocal(evt.surfacePos)
                    tree.sigRightClick.call(tree, pt)
                }
            }

            return
        }

        // Check boxes
        // ============================

        if evt.button != .Left
            return

        // On checkbox
        if tree.treeFlags & .Checkable and idx != Swag.U32.Max
        {
            eatCheck := false
            if evt.kind == .MouseDoubleClick
                eatCheck = true
            else
            {
                localPos := surfaceToLocal(evt.surfacePos)
                localPos.x += scrollPos.x
                if localPos.x < Math.min(getThemeMetrics().btnCheck_Size, tree.lineHeight)
                    eatCheck = true
            }

            if eatCheck
            {
                if evt.kind == .MouseReleased
                    return
                idxLine := tree.lines[idx].index
                checked := !tree.checkModel.isSelected(idxLine)
                if tree.selModel.isSelected(idxLine)
                {
                    visit l: tree.selModel
                        tree.checkModel.select(l, checked)
                }
                else
                    tree.checkModel.select(idxLine, checked)
                tree.sigCheckChanged.call(tree)
                invalidate()
                return
            }
        }

        // User double click
        // ============================

        if evt.kind == .MouseDoubleClick
        {
            if idx != Swag.U32.Max
                tree.toggleExpand(tree.lines[idx])
            pt := surfaceToLocal(evt.surfacePos)
            tree.sigLeftDoubleClick.call(tree, pt)
            return
        }

        // Selection by left button
        // ============================

        if tree.selectionMode == .None
            return

        // We select by mouse release when the hittest line is selected
        if evt.kind == .MousePressed and idx != Swag.U32.Max and tree.isLineSelected(idx)
        {
            discardMouseRelease = false
            return
        }

        if evt.kind == .MouseReleased and discardMouseRelease
        {
            return
        }

        discardMouseRelease = evt.kind == .MousePressed

        if tree.selectionMode == .Single or (!ctrl and !shift)
            tree.selModel.unselectAll()

        if idx != Swag.U32.Max
        {
            // Range selection
            if shift and tree.selectionMode == .Multi
            {
                startIdx := tree.focusIndex == Swag.U32.Max ? 0 : tree.focusIndex
                loop i: startIdx..idx
                {
                    idxLine := tree.lines[i].index
                    tree.selModel.select(idxLine, true)
                }
            }

            // Invert selection
            else
            {
                idxLine := tree.lines[idx].index
                toSel   := !tree.selModel.isSelected(idxLine)
                tree.selModel.select(idxLine, toSel)
            }
        }

        tree.focusIndex = idx
        invalidate()
        tree.sigSelChanged.call()
    }

    mtd onPaintEvent(evt: *PaintEvent)
    {
        if tree.getLineCount() == 0
            return

        bc      := evt.bc
        painter := bc.painter
        colors  := getThemeColors()
        metrics := getThemeMetrics()

        var rect: Math.Rectangle
        rect.width = getDisplayWidth()
        rect.height = tree.lineHeight

        // Clipping, take care of padding
        clipRect := getSurfaceRect()
        clipRect.applyPadding(padding)
        clientRect := getClientRectPadding()

        firstIdx := tree.firstVisibleLineIndex
        lastIdx  := tree.lastVisibleLineIndex

        // Do not display with bigs coordinates to avoid precision issues
        // So we do the scroll by hand, and draw will small coordinates
        y := cast(f64) tree.firstVisibleIndex * tree.lineHeight
        y -= scrollPos.y
        rect.y = cast(f32) y

        firstVisible := true
        cptVis       := 0'u32
        loop i: firstIdx..lastIdx
        {
            line := tree.lines[i]
            if !line.visible
                continue
            if rect.y > clientRect.bottom()
                break

            if rect.bottom() < 0
            {
                cptVis += 1
                rect.y = rect.bottom()
                continue
            }

            // Update visible indexes
            if tree.recomputeIndexes
            {
                if firstVisible
                {
                    firstVisible = false
                    tree.firstVisibleIndex = cptVis
                    tree.firstVisibleLineIndex, tree.lastVisibleLineIndex = i
                }
                else
                {
                    tree.lastVisibleLineIndex = i
                }
            }

            paintLine(bc, i, cptVis, line, rect)
            rect.y = rect.bottom()

            if tree.treeFlags & .HorzLines
                painter.drawLine(rect.x, rect.y, rect.right(), rect.y, colors.treeItem_Line, 1)

            cptVis += 1
        }

        if tree.header and tree.treeFlags & .VertLines
        {
            rect = getClientRectPadding()
            loop idx: 1..tree.header.items.count-1
            {
                x := getXPos(tree.header.items[idx].column)
                x -= scrollPos.x
                x -= metrics.header_Separator * 0.5
                painter.drawLine(x, rect.y, x, rect.bottom(), colors.treeItem_Line, 1)
            }

            last := tree.header.items.back()
            x := getXPos(last.column)
            x -= scrollPos.x
            x += last.width
            x += metrics.header_Separator * 0.5
            painter.drawLine(x, rect.y, x, rect.bottom(), colors.treeItem_Line, 1)
        }

        tree.recomputeIndexes = false
    }
}

private impl TreeView
{
    mtd getXPos(column: u32)->f32
    {
        x := tree.header.getItemPositionByCol(column)
        x += tree.header.getSurfaceRect().x
        x -= getSurfaceRect().x
        return x
    }

    mtd paintLine(bc: PaintContext, idx, idxVis: u32, line: *TreeLine, rect: Math.Rectangle)
    {
        painter := bc.painter
        theme   := getTheme()
        colors  := getThemeColors()
        metrics := getThemeMetrics()
        rects   := getThemeRects()
        font    := theme.res.fontDefault.regular

        // Selection box
        isSelected := tree.isLineSelected(idx)
        hideSel := cast(bool) (tree.treeFlags & .HideSelection)
        if isSelected and (!hideSel or hasFocus()) and !bc.isDisabled
        {
            colorSel := hasFocus() ? colors.treeItem_FocusSelectedBk : colors.treeItem_SelectedBk
            painter.fillRect(rect, colorSel)
        }

        // User background
        elif line.colorBk.a
        {
            painter.fillRect(rect, line.colorBk)
        }

        // Alternate lines
        elif (tree.treeFlags & .AlternateLines) and idxVis % 2 == 1
        {
            painter.fillRect(rect, colors.treeItem_AlternateBk)
        }

        var colorText: Color = ?
        switch
        {
            case bc.isDisabled:
                colorText = colors.treeItem_DisabledText
            case isSelected:
                colorText = colors.treeItem_SelectedText
            case line.colorFg.a:
                colorText = line.colorFg
            default:
                colorText = colors.treeItem_Text
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
            if tree.header
            {
                labelPos.x = getXPos(item.column)
                labelPos.width = tree.header.getItemWidthByCol(item.column)
            }

            labelPos.x -= scrollPos.x

            if idxItem == 0
            {
                // Ident level
                labelPos.moveLeft(line.level * tree.indentWidth)

                // Expand/collapse mark
                if line.lines.count or line.forceExpandMark
                {
                    expand := theme.getIcon24(line.expanded ? .SortArrowDown : .SortArrowRight, rect.height)
                    expand.paint(painter, labelPos.x, labelPos.vertCenter() - rect.height * 0.5, colorFg)
                    labelPos.moveLeft(rect.height)
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
            if idxItem == 0 and tree.treeFlags & .Checkable
            {
                var subRectBorder:  *ThemeImageRect
                var subRectMark:    *ThemeImageRect

                idxLine := tree.lines[idx].index
                checked := tree.checkModel.isSelected(idxLine)

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

                widthCheck := Math.min(metrics.btnCheck_Size, tree.lineHeight)
                posBox := labelPos
                posBox.height = widthCheck
                posBox.y = labelPos.vertCenter() - (posBox.height * 0.5)
                posBox.width = posBox.height
                theme.drawSubRect(painter, posBox, subRectMark, colorMark)
                theme.drawSubRect(painter, posBox, subRectBorder, colorBorder)

                labelPos.moveLeft(widthCheck + 4)
            }

            // Take care of header item margin
            if tree.header
                labelPos.moveLeft(tree.header.marginItems)

            // Left margin, or icon
            icon := item.icon
            if tree.iconSize > 0
            {
                if !icon.isValid()
                    labelPos.moveLeft(tree.iconSize + metrics.iconText_Margin)
                else
                    icon.sizeX, icon.sizeY = tree.iconSize
            }

            Theme.drawIconText(painter, labelPos, icon, &item.name, format, metrics.iconText_Margin)
        }
    }

    mtd hitTest(surfacePos: Math.Point)->u32
    {
        lc := tree.getLineCount()
        res := surfaceToLocal(surfacePos)
        idx := cast(u32) (res.y / tree.lineHeight)
        if idx >= lc
            return Swag.U32.Max

        idx1 := tree.firstVisibleLineIndex
        cpt  := 0
        while cpt != idx
        {
            idx1 += 1
            if idx1 >= lc
                return Swag.U32.Max
            if tree.lines[idx1].visible
                cpt += 1
        }

        return idx1
    }

    mtd getDisplayWidth()->f32
    {
        if tree.horizontalExtent > 0
            return tree.horizontalExtent
        return position.width
    }
}

impl TreeCtrl
{
    #[Swag.Discardable]
    func createSimple(parent: *Wnd, position: Math.Rectangle, id: WndId = null)->*TreeCtrl
    {
        tree := Wnd.create'TreeCtrl(parent, "", position, id)

        with tree
        {
            scrollWnd = createView'ScrollWnd()
            scrollWnd.dockStyle = .Center
            scrollWnd.focusStrategy = .None
            scrollWnd.backgroundStyle = .Transparent
            scrollWnd.setAllPaddings(getThemeMetrics().list_Padding)
            scrollWnd.sigScrollPosChanged += @|tree|(scrollWnd, op, np) {
                tree.computeLayout()
            }

            treeView = scrollWnd.createView'TreeView()
            treeView.backgroundStyle = .Transparent
            treeView.tree = tree
            treeView.wndFlags |= .NoScroll
        }

        return tree
    }

    #[Swag.Discardable]
    func createMultiColumns(parent: *Wnd, position: Math.Rectangle, id: WndId = null)->*TreeCtrl
    {
        tree := Wnd.create'TreeCtrl(parent, "", position, id)

        fwnd := tree.createView'Wnd()
        fwnd.dockStyle = .Center
        fwnd.backgroundStyle = .Transparent
        fwnd.wndFlags ^= .ClipChildren

        // Adapt header to the frame container
        tree.hookOnEvent = @(wnd, evt)
        {
            if evt.kind != .Resize
                return false

            tree   := cast(*TreeCtrl) wnd
            header := tree.header
            switch tree.frameForm
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

        with tree
        {
            // Header
            // Need to recompute layout when header size has changed
            header = Header.create(fwnd, @{})
            header.dockStyle = .Top
            header.form = .Round
            header.sigLayoutChanged = @|tree|(header) {
                tree.computeLayout()
            }

            header.sigClicked = @|tree|(header, col) {
                tree.sort(col, header.sortMark <= 0 ? false : true, true)
            }

            // Scroll that contains the tree view
            scrollWnd = Wnd.create'ScrollWnd(fwnd, @{})
            scrollWnd.dockStyle = .Center
            scrollWnd.focusStrategy = .None
            scrollWnd.backgroundStyle = .Transparent
            scrollWnd.setAllPaddings(getThemeMetrics().list_Padding)
            scrollWnd.sigScrollPosChanged += @|tree,header|(scrollWnd, op, np) {
                header.scrollPos.x = np.x
                header.invalidate()
                tree.computeLayout()
            }

            // TreeCtrl view
            treeView = scrollWnd.createView'TreeView()
            treeView.backgroundStyle = .Transparent
            treeView.tree = tree
            treeView.wndFlags |= .NoScroll
        }

        return tree
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
        return selModel.isSelected(lines[idx].index)
    }

    // Select of unselect the given line
    mtd selectLine(idx: u32, select: bool = true)
    {
        lineCount := getLineCount()
        Debug.assert(idx < lineCount)

        if selectionMode == .None or lineCount == 0
            return

        idxLine := lines[idx].index
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
            idxLine := lines[i].index
            selModel.select(idxLine, select)
        }

        invalidate()
        sigSelChanged.call(self)
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
        scrollWnd.setScrollSize(scrollWidth, visibleLineCount * lineHeight)

        firstVisibleIndex     = 0
        firstVisibleLineIndex = 0
        lastVisibleLineIndex  = visibleLineCount ? getLineCount() - 1 : 0
        recomputeIndexes = true

        invalidate()
    }

    // Get a line by index
    mtd getLine(idx: u32)->*TreeLine
    {
        return lines[idx]
    }

    // Returns the number of lines in the tree
    mtd getLineCount()->u32
    {
        return cast(u32) lines.count
    }

    // Add one new line
    mtd createLine(parentLine: *TreeLine)->*TreeLine
    {
        line := lines.newPtr()
        line.parent = parentLine
        line.index = nextTreeIndex
        idxLineToIdx.add(line.index, line.index)
        nextTreeIndex += 1

        line.visible = line.isVisible()
        if line.visible visibleLineCount += 1

        if parentLine
        {
            line.level = parentLine.level + 1
            parentLine.lines.add(line)
        }

        lines.add(line)
        return line
    }

    // Add one line, and set first column
    mtd createLine(parentLine: *TreeLine, name: string, icon: Icon = @{})->*TreeLine
    {
        line := createLine(parentLine)
        line.setItem(0, #self.name, #self.icon)
        return line
    }

    // Clear all lines
    mtd clear()
    {
        lines.clear()
        idxLineToIdx.clear()
        visibleLineCount = 0
    }

    // Add one column in a multi columns tree
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

    // Set the corresponding line visible
    mtd ensureVisibleLine(idx: u32)
    {
        first := firstVisibleLineIndex
        last  := lastVisibleLineIndex
        if idx > first and idx < last
            return
        sc := scrollWnd.getScrollPos()
        scrollWnd.setScrollPos(sc.x, idx * lineHeight)
    }

    // Returns true if the line at the given index is selected
    mtdc isSelectedLine(idx: u32)->bool
    {
        idxLine := lines[idx].index
        return selModel.isSelected(idxLine)
    }

    // Get all the selected lines
    mtdc getSelectedLines()->Array'(*TreeLine)
    {
        var result: retval
        if selModel.isEmpty()
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
        if checkModel.isEmpty()
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
        if selModel.isEmpty()
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

    // Sort the tree by a given column
    // If 'persistent' is true, then the sorted column will be displayed in the
    // header (if it exists)
    mtd sort(column: u32, descentOrder, persistent = false)
    {
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

    // Expand the given line
    mtd expand(line: *TreeLine)
    {
        if line.expanded
            return
        line.expanded = true

        var tmp: Array'(*TreeLine)
        tmp.add(line)

        while tmp.count
        {
            to := tmp.popBack()
            visit l: to.lines
            {
                l.visible = true
                visibleLineCount += 1
                if l.expanded and l.lines.count
                    tmp.add(l)
            }
        }

        computeLayout()
    }

    // Collapse all lines
    mtd collapseAll()
    {
        visibleLineCount = 0
        visit l: lines
        {
            l.visible  = l.parent ? false : true
            l.expanded = false
            visibleLineCount += l.visible ? 1 : 0
        }

        computeLayout()
    }

    // Collapse the given line
    mtd collapse(line: *TreeLine)
    {
        if !line.expanded
            return
        line.expanded = false

        var tmp: Array'(*TreeLine)
        tmp.add(line)
        while tmp.count
        {
            to := tmp.popBack()
            visit l: to.lines
            {
                l.visible = false
                visibleLineCount -= 1
                if l.expanded and l.lines.count
                    tmp.add(l)
            }
        }

        computeLayout()
    }

    // Collapse the given line
    mtd toggleExpand(line: *TreeLine)
    {
        if line.expanded
            collapse(line)
        else
            expand(line)
    }

    // Return the next visible line index, or Swag.U32.Max
    mtd getPrevVisibleLineIndex(idx: u32)->u32
    {
        if !idx
            return Swag.U32.Max
        i := idx - 1
        while !lines[i].visible
        {
            if i == 0
                return Swag.U32.Max
            i -= 1
        }

        return i
    }

    // Return the next visible line index, or Swag.U32.Max
    mtd getNextVisibleLineIndex(idx: u32)->u32
    {
        i := idx + 1
        while i < lines.count and !lines[i].visible
            i += 1
        if i >= lines.count
            return Swag.U32.Max
        return i
    }
}