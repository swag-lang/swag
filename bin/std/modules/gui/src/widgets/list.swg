#global public
using Core, Pixel

struct ListItem
{
    name:   RichString
    icon:   Icon
    column: u32
}

struct ListLine
{
    items:      Array'(*ListItem)
    userData:   *void
}

impl ListLine
{
    mtd opDrop()
    {
        visit item: items
            Memory.delete(item)
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

struct List
{
    using frameWnd:     FrameWnd
    horizontalExtent:   f32
    lineHeight:         f32 = 20
    iconSize:           f32
    hideSelection:      bool
    selectionMode:      ListSelectionMode = Single
    sigVirtualFill:     func(*ListLine, u32)
    sigSelChanged:      func()

    lines:          Array'(*ListLine)
    selection:      Array'{start: u32, end: u32}
    focusIndex:     u32 = Swag.U32.Max
    listView:       *ListView

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

        idx := hitTest(evt.surfacePos)

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
                list.unselectAllPrivate()
        }

        if idx != Swag.U32.Max
        {
            // Range selection
            if shift and list.selectionMode == .MultiExtended
            {
                startIdx := list.focusIndex == Swag.U32.Max ? 0 : list.focusIndex
                loop i: startIdx..idx
                    list.selectLinePrivate(i, true)
            }

            // Invert selection
            else
            {
                list.selectLinePrivate(idx, !list.isLineSelected(idx))
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
        font    := theme.res.fontDefault.regular

        isSelected := list.isLineSelected(idx)
        if isSelected and (!list.hideSelection or hasFocus()) and !bc.isDisabled
            painter.fillRect(rect, hasFocus() ? colors.listItem_FocusSelectedBk : colors.listItem_SelectedBk)

        var colorText: Color = ?
        switch
        {
            case bc.isDisabled:
                colorText = colors.listItem_DisabledText
            case isSelected:
                colorText = colors.listItem_SelectedText
            default:
                colorText = colors.listItem_Text
        }

        var format: RichStringFormat
        visit item: line.items
        {
            theme.setTextColors(&format, colorText)
            format.horzAlignment = .Left
            format.vertAlignment = .Center
            format.flags = .Clip
            format.font = theme.res.fontDefault

            labelPos := rect
            icon     := item.icon
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

private impl List
{
    mtd unselectAllPrivate()
    {
        selection.clear()
    }

    mtd isLineSelectedPrivate(idx: u32)->u32
    {
        if idx >= getLineCount()
            return Swag.U32.Max

        visit *r, i: selection
        {
            if idx >= r.start and idx <= r.end
                return cast(u32) i
        }

        return Swag.U32.Max
    }

    mtd selectLinePrivate(idx: u32, select: bool)
    {
        if idx >= getLineCount()
            return

        isSelectedRange := isLineSelectedPrivate(idx)
        isSelected := isSelectedRange != Swag.U32.Max
        if select == isSelected
            return

        line := lines[idx]

        if select
        {
            // Prepend/Append to an existing range
            visit *r, i: selection
            {
                if r.start and idx == r.start - 1
                {
                    r.start -= 1
                    return
                }

                if idx == r.end + 1
                {
                    r.end += 1
                    return
                }
            }

            // Need to make a new range
            selection.add(@{idx, idx})
        }
        else
        {
            range := selection[isSelectedRange]
            if range.start == range.end
                selection.removeAt(cast(uint) isSelectedRange)
            elif range.start == idx
                range.start += 1
            elif range.end == idx
                range.end -= 1
            else
            {
                end := range.end
                range.end = idx - 1
                selection.add(@{idx + 1, end})
            }
        }
    }
}

impl List
{
    #[Swag.Discardable]
    func createSimple(parent: *Wnd, position: Math.Rectangle, id: string = null)->*List
    {
        list := Wnd.create'List(parent, "", position, id)

        scrollWnd := list.createView'ScrollWnd()
        scrollWnd.dockStyle = .Center
        scrollWnd.focusStrategy = .None
        scrollWnd.backgroundStyle = .Transparent

        list.listView = scrollWnd.createView'ListView()
        list.listView.backgroundStyle = .Transparent
        list.listView.list = list

        return list
    }

    // Unselect all lines
    mtd unselectAll()
    {
        if selection.isEmpty()
            return
        unselectAllPrivate()
        invalidate()
        if sigSelChanged
            sigSelChanged()
    }

    // Returns
    mtd isLineSelected(idx: u32)->bool
    {
        if selectionMode == .None
            return false
        return isLineSelectedPrivate(idx) != Swag.U32.Max
    }

    // Select of unselect the given line
    mtd selectLine(idx: u32, select: bool = true)
    {
        if selectionMode == .None
            return
        selectLinePrivate(idx, select)
        invalidate()
        if sigSelChanged
            sigSelChanged()
    }

    // Select a range of lines
    mtd selectLines(start, end: u32, select: bool = true)
    {
        if selectionMode == .None
            return
        loop i: start..end
            selectLinePrivate(i, select)
        invalidate()
        if sigSelChanged
            sigSelChanged()
    }

    // Compute global layout, once all lines have been added
    mtd computeLayout()
    {
        scrollWnd    := cast(*ScrollWnd) view
        clientRect   := scrollWnd.getClientRect()
        scrollWidth  := horizontalExtent orelse clientRect.width
        scrollHeight := getLineCount() * lineHeight
        scrollWnd.setScrollSize(scrollWidth, scrollHeight)
        invalidate()
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
        lines.add(line)
        return line
    }

    mtd createLine(name: string, icon: Icon = @{})->*ListLine
    {
        line := createLine()
        line.setItem(0, #self.name, #self.icon)
        return line
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
}