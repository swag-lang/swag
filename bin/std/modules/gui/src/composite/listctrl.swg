#global marked
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
        while l and l != me:
            l = l.parent
        return l ? true : false
    }

    #[Swag.Inline]
    mtd canExpand()->bool
    {
        return me.forceExpandMark or me.lines.count > 0
    }

    // Get an item by index
    mtd getItem(column: u32)->*ListItem
    {
        Debug.assert(column < me.items.count)
        foreach item in me.items
        {
            if item.column == column:
                return item
        }

        unreachable
    }

    // Initialize a given column item
    #[Swag.Discardable]
    mtd setItem(column: u32, name: string, icon: Icon = {})->*ListItem
    {
        foreach item in me.items
        {
            if item.column == column
            {
                item.name = name
                item.icon = icon
                return item
            }
        }

        let item = me.items.newPtr()
        item.column = column
        item.name   = name
        item.icon   = icon
        me.items.add(item)
        return item
    }
}

impl IWnd for ListView
{
    mtd impl onResizeEvent(evt: *ResizeEvent)
    {
        if me.list:
            me.list.computeLayout()
    }

    mtd impl onKeyEvent(evt: *KeyEvent)
    {
        evt.accepted = true
        if evt.kind != .KeyPressed:
            return

        me.list.sigKeyPressed.call(me.list, evt)

        let lineCount = me.list.getLineCount()
        if !lineCount:
            return

        var focusLine: *ListLine
        if me.list.focusIndex < me.list.lines.count:
            focusLine = me.list.lines[me.list.focusIndex]

        var shift = cast(bool) (evt.modifiers & .Shift)
        let ctrl  = cast(bool) (evt.modifiers & .Control)

        if me.list.focusIndex == Swag.U32.Max
        {
            if evt.key != .Down:
                return
        }

        var selChanged = false
        var startSelIdx, endSelIdx: u32

        switch evt.key
        {
        case Space where focusLine:
            if focusLine.canCheck
            {
                foreach l in me.list.selModel
                {
                    let idx = me.list.idxLineToIdxList.find(l).value
                    if !me.list.lines[idx].canCheck:
                        continue
                    let checked = !me.list.checkModel.isSelected(l)
                    me.list.checkModel.select(l, checked)
                }

                me.list.sigCheckChanged.call(me.list)
                me.invalidate()
            }
            else
            {
                me.list.toggleExpand(focusLine)
            }

        case Enter where focusLine:
            if focusLine.canCheck
            {
                foreach l in me.list.selModel
                {
                    let idx = me.list.idxLineToIdxList.find(l).value
                    if !me.list.lines[idx].canCheck:
                        continue
                    me.list.checkModel.select(l, true)
                }

                me.list.sigCheckChanged.call(me.list)
                me.invalidate()
            }
            else
            {
                me.list.toggleExpand(focusLine)
            }

        case A:
            if !ctrl or me.list.selectionMode == .Single:
                break
            shift = true
            me.list.selModel.unselectAll()
            endSelIdx   = 0
            startSelIdx = me.list.getLineCount() - 1
            selChanged  = true

        case Home:
            startSelIdx = me.list.focusIndex
            endSelIdx   = 0
            selChanged  = true

        case End:
            startSelIdx = me.list.focusIndex
            endSelIdx   = lineCount - 1
            selChanged  = true

        case Up:
            if me.list.focusIndex == 0:
                startSelIdx, endSelIdx = 0
            else:
                startSelIdx, endSelIdx = me.list.focusIndex - 1
            selChanged = true

        case Down:
            if me.list.focusIndex == lineCount - 1:
                startSelIdx, endSelIdx = lineCount - 1
            elif me.list.focusIndex == Swag.U32.Max:
                me.list.focusIndex = 0
            else:
                startSelIdx, endSelIdx = me.list.focusIndex + 1
            selChanged = true

        case Left where focusLine:
            me.list.collapse(focusLine)
        case Right where focusLine:
            me.list.expand(focusLine)

        case PageDown:
            let first = me.list.getFirstVisibleLineIndex()
            let last  = me.list.getLastVisibleLineIndex()
            startSelIdx, endSelIdx = me.list.focusIndex + (last - first)
            startSelIdx = Math.min(startSelIdx, lineCount - 1)
            endSelIdx   = Math.min(endSelIdx, lineCount - 1)
            selChanged  = true

        case PageUp:
            let first  = me.list.getFirstVisibleLineIndex()
            let last   = me.list.getLastVisibleLineIndex()
            let offset = last - first
            if offset > me.list.focusIndex:
                startSelIdx, endSelIdx = 0
            else:
                startSelIdx, endSelIdx = me.list.focusIndex - offset
            selChanged = true
        }

        if selChanged
        {
            if !shift or me.list.selectionMode == .Single
            {
                me.list.selModel.unselectAll()
                startSelIdx = endSelIdx
            }

            for i in startSelIdx to endSelIdx:
                me.list.selectLine(i, true)
            me.list.focusIndex = endSelIdx
            me.list.ensureVisibleLine(me.list.focusIndex)
        }
    }

    mtd impl onMouseEvent(evt: *MouseEvent)
    {
        if evt.kind == .MouseLeave
        {
            me.list.hotIndex = Swag.U32.Max
            me.invalidate()
            evt.accepted = false
            return
        }

        let idx = me.hitTest(evt.surfacePos)
        if idx != me.list.hotIndex and me.list.listFlags.has(.HotTrack)
        {
            me.list.hotIndex = idx
            me.invalidate()
        }

        if evt.kind != .MousePressed and evt.kind != .MouseReleased and evt.kind != .MouseDoubleClick
        {
            evt.accepted = false
            return
        }

        let metrics = me.getThemeMetrics()

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
                    let idxLine = me.list.isVirtual ? idx : me.list.lines[idx].index
                    if !me.list.selModel.isSelected(idxLine)
                    {
                        me.list.selModel.unselectAll()
                        me.list.selModel.select(idxLine, true)
                        me.list.focusIndex = idx
                        me.invalidate()
                    }
                }
                elif evt.kind == .MouseReleased
                {
                    var pt = me.surfaceToLocal(evt.surfacePos)
                    me.list.sigRightClick.call(me.list, pt)
                }
            }

            return
        }

        if evt.button != .Left:
            return

        if idx != Swag.U32.Max and !me.list.isVirtual
        {
            let hitLine  = me.list.lines[idx]
            let idxLine  = hitLine.index
            var localPos = me.surfaceToLocal(evt.surfacePos)
            localPos.x += me.scrollPos.x
            var widthLeft = hitLine.level * me.list.indentWidth
            if localPos.x >= widthLeft
            {
                // Expand
                // ============================

                widthLeft += Math.min(metrics.btnCheck_Size, me.list.getLineHeight())
                if me.list.lines[idx].canExpand()
                {
                    var eatCheck = false
                    if localPos.x < widthLeft:
                        eatCheck = true
                    widthLeft += Math.min(metrics.btnCheck_Size, me.list.getLineHeight())

                    if eatCheck
                    {
                        if evt.kind == .MouseReleased:
                            return
                        me.list.toggleExpand(me.list.lines[idx])
                        return
                    }
                }

                // Check boxes
                // ============================

                // On checkbox
                if hitLine.canCheck
                {
                    var eatCheck = false
                    if evt.kind == .MouseDoubleClick:
                        eatCheck = true
                    elif localPos.x < widthLeft:
                        eatCheck = true

                    if eatCheck
                    {
                        if evt.kind == .MouseReleased:
                            return

                        let checked = !me.list.checkModel.isSelected(idxLine)
                        if me.list.selModel.isSelected(idxLine)
                        {
                            foreach l in me.list.selModel:
                                me.list.checkModel.select(l, checked)
                        }
                        else:
                            me.list.checkModel.select(idxLine, checked)
                        me.list.sigCheckChanged.call(me.list)
                        me.invalidate()
                        return
                    }
                }
            }
        }

        // User double click
        // ============================

        if evt.kind == .MouseDoubleClick
        {
            if idx != Swag.U32.Max:
                me.list.toggleExpand(me.list.lines[idx])
            var pt = me.surfaceToLocal(evt.surfacePos)
            me.list.sigLeftDoubleClick.call(me.list, pt)
            return
        }

        // Selection by left button
        // ============================

        if me.list.selectionMode == .None:
            return

        // We select by mouse release when the hittest line is selected
        if evt.kind == .MousePressed and idx != Swag.U32.Max and me.list.isLineSelected(idx)
        {
            me.discardMouseRelease = false
            return
        }

        if evt.kind == .MouseReleased and me.discardMouseRelease
        {
            return
        }

        me.discardMouseRelease = evt.kind == .MousePressed

        let canUnsel = idx != Swag.U32.Max or !me.list.listFlags.has(.NoMouseEmptySel)
        if canUnsel and (me.list.selectionMode == .Single or (!ctrl and !shift)):
            me.list.selModel.unselectAll()

        if idx != Swag.U32.Max
        {
            // Range selection
            if shift and me.list.selectionMode == .Multi
            {
                let startIdx = me.list.focusIndex == Swag.U32.Max ? 0 : me.list.focusIndex
                let idx0     = Math.min(startIdx, idx)
                let idx1     = Math.max(startIdx, idx)
                for i in idx0 to idx1
                {
                    let idxLine = me.list.isVirtual ? i : me.list.lines[i].index
                    me.list.selModel.select(idxLine, true)
                }
            }

            // Invert selection
            else
            {
                let idxLine = me.list.isVirtual ? idx : me.list.lines[idx].index
                let toSel   = !me.list.selModel.isSelected(idxLine)
                me.list.selModel.select(idxLine, toSel)
            }
        }

        if canUnsel:
            me.list.focusIndex = idx
        me.invalidate()
        if canUnsel or idx != Swag.U32.Max:
            me.list.sigSelChanged.call(me.list)
    }

    mtd impl onPaintEvent(evt: *PaintEvent)
    {
        if me.list.getLineCount() == 0:
            return

        let bc      = evt.bc
        let painter = bc.painter
        let colors  = me.getThemeColors()
        let metrics = me.getThemeMetrics()

        var rect: Math.Rectangle
        rect.width  = me.getDisplayWidth()
        rect.height = me.list.getLineHeight()

        // Clipping, take care of padding
        var clipRect = me.getSurfaceRect()
        clipRect.applyPadding(me.padding)

        let firstIdx = me.list.getFirstVisibleLineIndex()
        let lastIdx  = me.list.getLastVisibleLineIndex()

        // Do not display with bigs coordinates to avoid precision issues
        // So we: the scroll by hand, and draw will small coordinates
        var y = cast(f64) firstIdx * me.list.getLineHeight()
        y -= me.scrollPos.y
        rect.y = cast(f32) y

        if me.list.isVirtual
        {
            Debug.assert(me.list.sigVirtualFill != null)
            for i in firstIdx to lastIdx
            {
                var line: ListLine
                me.list.sigVirtualFill(&line, i)
                me.paintLine(bc, i, &line, rect)
                rect.y = rect.bottom()

                if me.list.listFlags.has(.HorzLines):
                    painter.drawLine(rect.x, rect.y, rect.right(), rect.y, colors.listItem_Line, 1)
            }
        }
        else
        {
            for i in firstIdx to lastIdx
            {
                let line = me.list.lines[i]
                me.paintLine(bc, i, line, rect)
                rect.y = rect.bottom()

                if me.list.listFlags.has(.HorzLines):
                    painter.drawLine(rect.x, rect.y, rect.right(), rect.y, colors.listItem_Line, 1)
            }
        }

        if me.list.header and me.list.listFlags.has(.VertLines)
        {
            rect = me.getClientRectPadding()
            for idx in 1 until me.list.header.items.count
            {
                var x = me.getXPos(me.list.header.items[idx].column)
                x -= me.scrollPos.x
                x -= metrics.header_Separator * 0.5
                painter.drawLine(x, rect.y, x, rect.bottom(), colors.listItem_Line, 1)
            }

            let last = me.list.header.items.back()
            var x    = me.getXPos(last.column)
            x -= me.scrollPos.x
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
        var x = me.list.header.getItemPositionByCol(column)
        x += me.list.header.getSurfaceRect().x
        x -= me.getSurfaceRect().x
        return x
    }

    mtd paintLine(bc: PaintContext, idx: u32, line: *ListLine, rect: Math.Rectangle)
    {
        let painter = bc.painter
        let theme   = me.getTheme()
        let colors  = me.getThemeColors()
        let metrics = me.getThemeMetrics()
        let rects   = me.getThemeRects()

        // Selection box
        let isSelected  = me.list.isLineSelected(idx)
        let canHotTrack = me.list.isVirtual or !line.separator
        let hideSel     = me.list.listFlags.has(.HideSelection)
        if isSelected and (!hideSel or me.hasFocus()) and !bc.isDisabled
        {
            let colorSel = me.hasFocus() ? colors.listItem_FocusSelectedBk : colors.listItem_SelectedBk
            painter.fillRect(rect, colorSel)
        }

        // Hot box
        elif idx == me.list.hotIndex and me.list.listFlags.has(.HotTrack) and canHotTrack
        {
            painter.fillRect(rect, colors.listItem_HotBk)
        }

        // User background
        elif line.colorBk.a
        {
            painter.fillRect(rect, line.colorBk)
        }

        // Alternate lines
        elif me.list.listFlags.has(.AlternateLines) and idx % 2 == 1
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
            if line.separator:
                colorText, colorExpand = colors.listItem_Separator
            else:
                colorText, colorExpand = colors.listItem_Text
        }

        var format: RichStringFormat
        foreach item, idxItem in line.items
        {
            var colorFg: Color = undefined
            if item.colorFg.a and !isSelected:
                colorFg = item.colorFg
            else:
                colorFg = colorText
            theme.setTextColors(&format, colorFg)
            format.horzAlignment = item.horzAlign
            format.vertAlignment = .Center
            format.flags         = .Clip
            format.font          = me.list.getFont()

            var labelPos = rect

            // Right
            if me.list.header
            {
                labelPos.x     = me.getXPos(item.column)
                labelPos.width = me.list.header.getItemWidthByCol(item.column)
            }

            labelPos.x -= me.scrollPos.x
            labelPos.x += me.list.leftTextMargin + line.leftTextMargin

            if idxItem == 0
            {
                // Ident level
                labelPos.moveLeft(line.level * me.list.indentWidth)

                // Expand/collapse mark
                let canExpand = line.canExpand()
                if canExpand or me.list.listFlags.has(.ForceMarginExpandMark)
                {
                    if canExpand
                    {
                        var expand = theme.getIcon24(line.expanded ? .Expanded : .Collapsed)
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

                let idxLine = me.list.isVirtual ? idx : line.index
                let checked = me.list.checkModel.isSelected(idxLine)

                var colorBorder = colorFg
                var colorMark   = colorFg

                if checked
                {
                    subRectBorder = &rects.btnCheck_CheckedBorder
                    subRectMark   = &rects.btnCheck_CheckedMark
                }
                else
                {
                    subRectBorder = &rects.btnCheck_NormalBorder
                }

                let widthCheck = Math.min(metrics.btnCheck_Size, me.list.getLineHeight())
                var posBox     = labelPos
                posBox.height = widthCheck
                posBox.y      = labelPos.vertCenter() - (posBox.height * 0.5)
                posBox.width  = posBox.height
                theme.drawSubRect(painter, posBox, subRectMark, colorMark)
                theme.drawSubRect(painter, posBox, subRectBorder, colorBorder)

                labelPos.moveLeft(widthCheck + 4)
            }

            // Take care of header item margin
            if me.list.header:
                labelPos.moveLeft(me.list.header.marginItems)

            // Left margin, or icon
            var icon = item.icon
            if me.list.iconSize > 0
            {
                if icon.isValid():
                    icon.sizeX, icon.sizeY = me.list.iconSize
                else
                {
                    var iconMargin = me.list.iconMarginFirstCol
                    if !idxItem and line.parent:
                        iconMargin = line.parent.iconMargin
                    elif idxItem:
                        iconMargin = me.list.header.getItem(cast(u32) idxItem).iconMargin
                    if iconMargin:
                        labelPos.moveLeft(me.list.iconSize + metrics.iconText_Margin)
                }
            }

            Theme.drawIconText(painter, labelPos, icon, &item.name, format, metrics.iconText_Margin)
        }
    }

    mtd hitTest(surfacePos: Math.Point)->u32
    {
        let lc  = me.list.getLineCount()
        var res = me.surfaceToLocal(surfacePos)
        if res.x < 0 or res.y < 0:
            return Swag.U32.Max
        res.y += me.scrollPos.y
        let idx = cast(u32) (res.y / me.list.getLineHeight())
        if idx >= lc:
            return Swag.U32.Max
        if !me.list.isVirtual and me.list.lines[idx].separator:
            return Swag.U32.Max
        return idx
    }

    mtd getDisplayWidth()->f32
    {
        if me.list.horizontalExtent > 0:
            return me.list.horizontalExtent
        return me.position.width
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
            if !evt is ResizeEvent:
                return false

            var list   = cast(*ListCtrl) wnd
            var header = list.header
            if header.form == .Transparent:
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
        if me.selModel.isEmpty():
            return
        me.selModel.unselectAll()
        me.invalidate()
        me.sigSelChanged.call(me)
    }

    // Returns
    mtd isLineSelected(idx: u32)->bool
    {
        Debug.assert(idx < me.getLineCount())
        if me.selectionMode == .None:
            return false
        if me.isVirtual:
            return me.selModel.isSelected(idx)
        return !me.lines[idx].separator and me.selModel.isSelected(me.lines[idx].index)
    }

    // Select of unselect the given line
    mtd selectLine(idx: u32, select: bool = true)
    {
        let lineCount = me.getLineCount()
        Debug.assert(idx < lineCount)

        if me.selectionMode == .None or lineCount == 0:
            return
        if !me.isVirtual and me.lines[idx].separator:
            return

        if me.selectionMode == .Single:
            me.selModel.unselectAll()

        let idxLine = me.isVirtual ? idx : me.lines[idx].index
        me.selModel.select(idxLine, select)
        me.invalidate()

        me.sigSelChanged.call(me)
    }

    // Select a range of lines
    mtd selectLines(start, end: u32, select: bool = true)
    {
        let lineCount = me.getLineCount()
        Debug.assert(start < lineCount)
        Debug.assert(end < lineCount)

        if me.selectionMode == .None or lineCount == 0:
            return

        for i in start to end
        {
            let idxLine = me.isVirtual ? i : me.lines[i].index
            me.selModel.select(idxLine, select)
        }

        me.invalidate()

        me.sigSelChanged.call(me)
    }

    // Compute global layout, once all lines have been added
    mtd computeLayout()
    {
        var clientRect = me.scrollWnd.getClientRectPadding()

        var scrollWidth: f32 = undefined
        if me.horizontalExtent > 0:
            scrollWidth = me.horizontalExtent
        elif me.header:
            scrollWidth = me.header.getTotalItemsWidth()
        else:
            scrollWidth = clientRect.width
        let scrollHeight = me.getLineCount() * me.getLineHeight()
        me.scrollWnd.setScrollSize(scrollWidth, scrollHeight)
        me.invalidate()
    }

    // Get the height of one line
    mtd getLineHeight()->f32
    {
        return me.lineHeight orelse me.getFont().regular.height + 2
    }

    // Get a line by index
    mtd getLine(idx: u32)->*ListLine
    {
        return me.lines[idx]
    }

    // Returns the number of lines in the list
    mtd getLineCount()->u32
    {
        if me.isVirtual:
            return me.virtualCount
        return cast(u32) me.lines.count
    }

    // Add one new line
    #[Swag.Overload]
    mtd createLine(parentLine: #null *ListLine)->*ListLine
    {
        Debug.assert(!me.isVirtual, "createLine cannot be called on a virtual list")

        if parentLine
        {
            let line = parentLine.lines.newPtr()
            line.parent = parentLine
            line.level  = parentLine.level + 1
            parentLine.lines.add(line)
            return line
        }

        let line = me.toFreeLines.newPtr()
        line.index = cast(u32) me.lines.count
        me.idxLineToIdxList.add(line.index, line.index)
        me.lines.add(line)
        me.toFreeLines.add(line)
        return line
    }

    // Add one line, and set first column
    #[Swag.Discardable, Swag.Overload]
    mtd createLine(name: string, icon: Icon = {}, parentLine: *ListLine = null)->*ListLine
    {
        let line = me.createLine(parentLine)
        line.setItem(0, name, icon)
        return line
    }

    // Clear all lines
    mtd clear()
    {
        me.lines.clear()
        me.virtualCount = 0
        me.idxLineToIdxList.clear()
        me.focusIndex = Swag.U32.Max
        me.selModel.unselectAll()
        me.checkModel.unselectAll()
    }

    // Add one column in a multi columns list
    mtd addColumn(name: string, width: f32 = 100, icon: Icon = {})
    {
        Debug.assert(me.header != null)
        me.header.addItem(name, width, icon)
    }

    // Set the column width
    mtd setColumnWidth(idx: u32, width: f32)
    {
        Debug.assert(me.header != null)
        me.header.setItemWidth(idx, width)
    }

    // Set the number of lines in a virtual list
    mtd setVirtualCount(count: u32)
    {
        me.clear()
        me.isVirtual    = true
        me.virtualCount = count
        me.computeLayout()
    }

    // Returns the first visible line index
    mtd getFirstVisibleLineIndex()->u32
    {
        let lc  = me.getLineCount()
        let idx = cast(u32) (me.listView.scrollPos.y / me.getLineHeight())
        if idx >= lc:
            return Swag.U32.Max
        return idx
    }

    // Returns the last visible line index
    mtd getLastVisibleLineIndex()->u32
    {
        let lc = me.getLineCount()
        if lc == 0:
            return Swag.U32.Max

        var y = me.listView.scrollPos.y + me.scrollWnd.view.position.height
        y -= me.scrollWnd.view.padding.y
        y -= me.scrollWnd.view.padding.w
        if y < 0:
            return cast(u32) (me.listView.scrollPos.y / me.getLineHeight())
        let idx = cast(u32) (y / me.getLineHeight())
        if idx >= lc:
            return cast(u32) lc - 1
        return idx
    }

    // Set the corresponding line visible
    mtd ensureVisibleLine(idx: u32)
    {
        let first = me.getFirstVisibleLineIndex()
        let last  = me.getLastVisibleLineIndex()
        if idx > first and idx < last:
            return
        let sc = me.scrollWnd.getScrollPos()
        me.scrollWnd.setScrollPos(sc.x, idx * me.getLineHeight())
    }

    // Returns true if the line at the given index is selected
    mtd const isSelectedLine(idx: u32)->bool
    {
        let idxLine = me.isVirtual ? idx : me.lines[idx].index
        return me.selModel.isSelected(idxLine)
    }

    // Get the line with the keyboard focus
    mtd const getFocusLine()->#null *ListLine
    {
        if me.focusIndex >= me.lines.count:
            return null
        return me.lines[me.focusIndex]
    }

    // Get all the selected lines
    mtd const getSelectedLines()->Array'(*ListLine)
    {
        var result: retval
        if me.isVirtual or me.selModel.isEmpty():
            return result

        foreach i in me.selModel
        {
            let res = me.idxLineToIdxList.tryFind(i)
            if !res:
                continue
            result.add(me.lines[res.value])
        }

        return result
    }

    // Get all the checked lines
    mtd const getCheckedLines()->Array'(*ListLine)
    {
        var result: retval
        if me.isVirtual or me.checkModel.isEmpty():
            return result

        foreach i in me.checkModel
        {
            let res = me.idxLineToIdxList.tryFind(i)
            if !res:
                continue
            result.add(me.lines[res.value])
        }

        return result
    }

    // Returns a selected line
    // Mostly for singlesel mode
    mtd const getSelectedLine()->#null *ListLine
    {
        if me.isVirtual or me.selModel.isEmpty():
            return null
        let range = me.selModel.selection.front()
        let res   = me.idxLineToIdxList.tryFind(range.start)
        if !res:
            return null
        let idx = res.value
        return me.lines[idx]
    }

    internal mtd mapIndexes()
    {
        me.idxLineToIdxList.clear()
        foreach l, idx in me.lines:
            me.idxLineToIdxList.add(l.index, cast(u32) idx)
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
            if descentOrder:
                res *= -1
            return res
        }

        let cb = mtd|column, descentOrder|(a, b: *ListLine)->s32
        {
            if me.sigSort:
                return me.sigSort(a, b, column, descentOrder)

            if a.parent != b.parent
            {
                if a.isParentOf(b):
                    return -1
                if b.isParentOf(a):
                    return 1

                var a1 = a
                var b1 = b
                while a1.level > b1.level:
                    a1 = a1.parent
                while b1.level > a1.level:
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
        if me.isVirtual:
            return

        me.sort(column, descentOrder, &me.lines)
        me.mapIndexes()

        if me.header and persistent
        {
            me.header.sortColumn = column
            me.header.sortMark   = descentOrder ? -1 : 1
            me.header.invalidate()
        }

        me.invalidate()
    }

    // Sort list as before
    #[Swag.Overload]
    mtd sort()
    {
        if me.header and me.header.sortMark != 0:
            me.sort(me.header.sortColumn, me.header.sortMark == -1 ? true : false)
        else:
            me.sort(0)
    }

    // Expand the given line
    mtd expand(line: *ListLine)
    {
        if !line.canExpand() or line.expanded:
            return
        line.expanded = true
        me.sigExpand.call(me, line)

        var tmp: Array'(*ListLine)
        tmp.add(line)

        // Auto sort
        if me.header and me.header.sortMark:
            me.sort(me.header.sortColumn, me.header.sortMark == -1 ? true : false, &line.lines)

        var cptIdxLine = cast(u32) me.lines.count
        var cptIdx     = 0'u32
        while tmp.count
        {
            let toI = tmp.popBack()
            let res = me.idxLineToIdxList.tryFind(toI.index)
            if !res:
                continue
            let idxInsert = res.value
            foreach l, i in toI.lines
            {
                l.index = cptIdxLine
                cptIdxLine += 1
                me.idxLineToIdxList.add(l.index, idxInsert + cptIdx + 1)
                cptIdx += 1
                if l.expanded:
                    tmp.add(l)
            }

            // Update indexes after the insertion
            for var idx = idxInsert + 1; idx < cast(u32) me.lines.count; idx += 1:
                me.idxLineToIdxList.add(me.lines[idx].index, cast(u32) (idx + toI.lines.count))

            // Insert at the right position
            me.lines.insertAt(cast(u64) (idxInsert + 1), toI.lines.toSlice())
        }

        me.computeLayout()
    }

    // Collapse the given line
    mtd collapse(line: *ListLine)
    {
        if !line.canExpand() or !line.expanded:
            return
        line.expanded = false
        me.sigCollapse.call(me, line)

        var tmp: Array'(*ListLine)
        tmp.add(line)

        var removeCount = 0'u32
        while tmp.count
        {
            let toI = tmp.popBack()
            foreach l in toI.lines
            {
                removeCount += 1
                if l.expanded:
                    tmp.add(l)
            }
        }

        // Insert at the right position
        let res = me.idxLineToIdxList.tryFind(line.index)
        if !res:
            return
        let idxInsert = res.value
        me.lines.removeAtOrdered(cast(u64) (idxInsert + 1), cast(u64) removeCount)

        // Update indexes after the insertion
        for var idx = idxInsert + 1; idx < cast(u32) me.lines.count; idx += 1:
            me.idxLineToIdxList.add(me.lines[idx].index, idx)

        // Update selection
        if me.focusIndex >= idxInsert + 1 and me.focusIndex <= idxInsert + 1 + removeCount
        {
            me.unselectAll()
            me.focusIndex = idxInsert
            me.selectLine(me.focusIndex)
        }

        me.computeLayout()
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

        foreach l in me.lines
        {
            if l.expanded:
                me.collapse(l)
        }

        collapseRec(me.lines.toSlice())
        me.computeLayout()
    }

    // Collapse the given line
    mtd toggleExpand(line: *ListLine)
    {
        if line.expanded:
            me.collapse(line)
        else:
            me.expand(line)
    }

    // Set focus to the list
    mtd setFocus()
    {
        me.listView.setFocus()
    }
}
