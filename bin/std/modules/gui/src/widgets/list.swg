#global public
using Core, Pixel

struct ListLine
{
    items:          Array'(*ListItem)
    selectedIdx:    u32 = Swag.U32.Max
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
    lines:          Array'(*ListLine)
    selection:      Array'(*ListLine)
    focusIndex:     u32 = Swag.U32.Max
    lineHeight:     f32 = 20
    isVirtual:      bool
}

struct ListView
{
    using wnd:              Wnd
    list:                   *List
    discardMouseRelease:    bool
}

public impl ListLine
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

        idx := hitTest(evt.surfacePos)
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

        if !ctrl and !shift
            list.unselectAll()

        if idx != Swag.U32.Max
        {
            // Range selection
            if shift
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
        bc      := evt.bc
        painter := bc.painter
        colors  := getThemeColors()

        var rect: Math.Rectangle
        rect.width = getDisplayWidth()
        rect.height = list.lineHeight

        font := getTheme().res.fontDefault.regular
        visit line: list.lines
        {
            // Selection
            if line.isSelected()
                painter.fillRect(rect, hasFocus() ? colors.listItem_FocusSelectedBk : colors.listItem_SelectedBk)

            painter.drawString(rect.x, rect.y, "Zaa", font, Argb.White)
            rect.y = rect.bottom()
        }
    }
}

private impl ListView
{
    mtd hitTest(surfacePos: Math.Point)->u32
    {
        res := surfaceToLocal(surfacePos)
        res.y += scrollPos.y
        idx := cast(u32) (res.y / list.lineHeight)
        if idx >= list.lines.count
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
        if selection.isEmpty()
            return

        visit line: selection
            line.selectedIdx = Swag.U32.Max
        selection.clear()
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


    // Compute global layout, once all lines have been added
    mtd computeLayout()
    {
        scrollWnd    := cast(*ScrollWnd) view
        clientRect   := scrollWnd.getClientRect()
        scrollWidth  := width orelse clientRect.width
        scrollHeight := cast(f32) lines.count * lineHeight
        scrollWnd.setScrollSize(scrollWidth, scrollHeight)
        invalidate()
    }

    // Add one new line
    mtd createLine()->*ListLine
    {
        Debug.assert(!isVirtual)
        line := Memory.new'ListLine()
        lines.add(line)
        return line
    }
}