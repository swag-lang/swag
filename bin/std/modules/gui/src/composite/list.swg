#global public
using Core, Pixel

struct ListItem
{
    name:       RichString
    icon:       Icon
    colorBk:    Color
    colorFg:    Color
    column:     u32
}

struct ListLine
{
    items:      Array'(*ListItem)
    userData:   *void
    colorBk:    Color
    colorFg:    Color
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

    lines:          Array'(*ListLine)
    selection:      Array'{start: u32, end: u32}
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
        visit item: line.items
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
                labelPos.x = list.header.getItemPosition(item.column)
                labelPos.width = list.header.getItemWidth(item.column)
            }

            // Background color ?
            if item.colorBk.a
                painter.fillRect(labelPos, item.colorBk)

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
        list.hookOnEvent = @(wnd: *Wnd, evt: *Event)->bool
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
            header.userData = list
            header.sigLayoutChanged = @(header: *Header) {
                list := cast(*List) header.userData
                list.computeLayout()
            }

            // Scroll that contains the list view
            scrollWnd = Wnd.create'ScrollWnd(fwnd, @{})
            scrollWnd.dockStyle = .Center
            scrollWnd.focusStrategy = .None
            scrollWnd.backgroundStyle = .Transparent
            scrollWnd.setAllPaddings(getThemeMetrics().list_Padding)
            scrollWnd.userData = list
            scrollWnd.sigScrollPosChanged = @(scrollWnd: *ScrollWnd, sp: Math.Point) {
                list := cast(*List) scrollWnd.userData
                header := list.header
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
        if selection.isEmpty()
            return
        unselectAllPrivate()
        invalidate()
        if sigSelChanged
            sigSelChanged(self)
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
            sigSelChanged(self)
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
            sigSelChanged(self)
    }

    // Compute global layout, once all lines have been added
    mtd computeLayout()
    {
        clientRect   := scrollWnd.getClientRectPadding()

        var scrollWidth: f32
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

    // Add one line, and set first column
    mtd createLine(name: string, icon: Icon = @{})->*ListLine
    {
        line := createLine()
        line.setItem(0, #self.name, #self.icon)
        return line
    }

    // Add one column in a multi columns list
    mtd addColumn(name: string, width: s32 = 100, icon: Icon = @{})
    {
        Debug.assert(header != null)
        header.addItem(#self.name, width, #self.icon)
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

    // Get a line by index
    mtd getLine(idx: u32)->*ListLine
    {
        return lines[idx]
    }
}