#global public
using Core, Pixel

struct ListLine
{
    items:          Array'(*ListItem)
    selectedIdx:    u32 = Swag.U32.Max
    userData:       *void
}

struct ListItem
{
    name:   RichString
    icon:   Icon
}

struct List
{
    using frameWnd: FrameWnd
    width:          f32
    isVirtual:      bool
    isMultiSel:     bool
    virtualFill:    func(*ListLine, u32)

    lines:          Array'(*ListLine)
    selection:      Array'(*ListLine)
    focusIndex:     u32 = Swag.U32.Max
    lineHeight:     f32 = 20

    virtualCount:       u32
    virtualFirstSel:    u32 = Swag.U32.Max
    virtualLastSel:     u32 = Swag.U32.Max
}

struct ListView
{
    using wnd:              Wnd
    list:                   *List
    discardMouseRelease:    bool
}

impl ListLine
{
    #[Swag.Inline]
    mtd isSelected() => selectedIdx != Swag.U32.Max
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
        if evt.kind == .MousePressed and idx != Swag.U32.Max and list.lines[idx].isSelected()
        {
            discardMouseRelease = false
            return
        }
        if evt.kind == .MouseReleased and discardMouseRelease
            return
        discardMouseRelease = evt.kind == .MousePressed

        ctrl  := evt.modifiers & .Control
        shift := evt.modifiers & .Shift

        if !list.isMultiSel or (!ctrl and !shift)
            list.unselectAll()

        if idx != Swag.U32.Max
        {
            // Range selection
            if shift and list.isMultiSel
            {
                startIdx := list.focusIndex == Swag.U32.Max ? 0 : list.focusIndex
                loop i: startIdx..idx
                    list.selectLine(i, true)
            }

            // Invert selection
            else
            {
                list.selectLine(idx, !list.lines[idx].isSelected())
            }

            invalidate()
        }

        list.focusIndex = idx
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

        firstIdx := getFirstVisibleLineIndex()
        lastIdx  := getLastVisibleLineIndex()
        rect.y = firstIdx * list.lineHeight

        if list.isVirtual
        {
            loop i: firstIdx..lastIdx
            {
                var line: ListLine
                if list.virtualFill
                    list.virtualFill(&line, i)
                paintLine(bc, &line, rect)
                rect.y = rect.bottom()
            }
        }
        else
        {
            loop i: firstIdx..lastIdx
            {
                line := list.lines[i]
                paintLine(bc, line, rect)
                rect.y = rect.bottom()
            }
        }
    }
}

private impl ListView
{
    mtd paintLine(bc: PaintContext, line: *ListLine, rect: Math.Rectangle)
    {
        painter := bc.painter
        colors  := getThemeColors()
        font    := getTheme().res.fontDefault.regular

        if line.isSelected()
            painter.fillRect(rect, hasFocus() ? colors.listItem_FocusSelectedBk : colors.listItem_SelectedBk)

        str := Format.toString("Zaa %", line)
        painter.drawString(rect.x, rect.y, str, font, Argb.White)
    }

    mtd getFirstVisibleLineIndex()->u32
    {
        lc := list.getLineCount()
        idx := cast(u32) (scrollPos.y / list.lineHeight)
        if idx >= lc
            return Swag.U32.Max
        return idx
    }

    mtd getLastVisibleLineIndex()->u32
    {
        lc := list.getLineCount()
        if lc == 0
            return Swag.U32.Max
        idx := cast(u32) ((scrollPos.y + position.height) / list.lineHeight)
        if idx >= lc
            return cast(u32) lc - 1
        return idx
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
        if list.width > 0
            return list.width
        return position.width
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

        listView := scrollWnd.createView'ListView()
        listView.backgroundStyle = .Transparent
        listView.list = list

        return list
    }

    // Unselect all lines
    mtd unselectAll()
    {
        if isVirtual
        {
            if virtualFirstSel == Swag.U32.Max
                return
            virtualFirstSel = Swag.U32.Max
            virtualLastSel  = Swag.U32.Max
        }
        else
        {
            if selection.isEmpty()
                return
            visit line: selection
                line.selectedIdx = Swag.U32.Max
            selection.clear()
        }

        invalidate()
    }

    // Select of unselect the given line
    mtd selectLine(idx: u32, select: bool = true)
    {
        if idx >= lines.count
            return
        if select and lines[idx].selectedIdx != Swag.U32.Max
            return
        if !select and lines[idx].selectedIdx == Swag.U32.Max
            return

        line := lines[idx]

        if select
        {
            line.selectedIdx = cast(u32) selection.count
            selection.add(line)
        }
        else
        {
            selectedIdx := line.selectedIdx
            line.selectedIdx = Swag.U32.Max
            selection.removeAt(cast(uint) selectedIdx)
            if selectedIdx < selection.count
                selection[selectedIdx].selectedIdx = selectedIdx
        }

        invalidate()
    }

    // Retreive selected lines
    mtd getSelection()->Array'(*ListLine)
    {
        return selection
    }

    // Compute global layout, once all lines have been added
    mtd computeLayout()
    {
        scrollWnd    := cast(*ScrollWnd) view
        clientRect   := scrollWnd.getClientRect()
        scrollWidth  := width orelse clientRect.width
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

    // Set the number of lines in a virtual list
    mtd setVirtualCount(count: u32)
    {
        Debug.assert(lines.isEmpty(), "setVirtualCount must only be called on a virtual list")
        isVirtual = true
        virtualCount = count
        computeLayout()
    }
}