#global public
using Core, Pixel

struct PopupListCtrlItem
{
    using base: Item
    isSeparator: bool

    aniSel:     BlendColor{speed: 10}
    aniText:    BlendColor{speed: 10}
    aniCheck:   BlendColor{speed: 10}
}

struct PopupListCtrl
{
    using frameWnd: FrameWnd
    heightItem:         f32 = 30
    heightSeparator:    f32 = 15
    minWidthPopup:      f32 = 100
    maxHeightPopup:     f32 = 300
    simFitX, simFitY:   bool

    sigSelected:    closure(*Self, u32)

    items:          ArrayPtr'PopupListCtrlItem
    selectedIdx     = Swag.U32.Max
    scrollWnd:      *ScrollWnd
    maxIconSize:    s32
}

struct PopupListView
{
    using wnd:  Wnd
    list:       *PopupListCtrl
    hotIdx:     u32 = Swag.U32.Max
}

impl IWnd for PopupListView
{
    mtd onFocusEvent(evt: *FocusEvent)
    {
        switch evt.kind
        {
        case KillFocus:
            if evt.other != parent
            {
                if list.sigSelected
                    list.sigSelected(list, Swag.U32.Max)
                else
                    list.destroy()
            }
        }
    }

    mtd onMouseEvent(evt: *MouseEvent)
    {
        var hit: u32

        surfaceRect := getSurfaceRect()
        mouseInRect := surfaceRect.contains(evt.surfacePos)

        pos := surfaceToLocal(evt.surfacePos)
        if evt.kind == .MouseLeave or !mouseInRect
            hit = Swag.U32.Max
        else
        {
            y := pos.y + scrollPos.y
            visit v, i: list.items
            {
                if v.isSeparator
                {
                    if y <= list.heightSeparator
                    {
                        hit = cast(u32) i
                        break
                    }
                    y -= list.heightSeparator
                }
                else
                {
                    if y <= list.heightItem
                    {
                        hit = cast(u32) i
                        break
                    }
                    y -= list.heightItem
                }
            }

            if hit >= list.items.count or list.items[hit].isSeparator
                hit = Swag.U32.Max
        }

        if hit != hotIdx
        {
            hotIdx = hit
            invalidate()
        }

        if evt.kind == .MousePressed and evt.button == .Left and mouseInRect
        {
            if list.sigSelected
                list.sigSelected(list, hotIdx)
            releaseMouse()
            list.destroy()
            return
        }

        surfaceRect = parent.getSurfaceRect()
        mouseInRect = surfaceRect.contains(evt.surfacePos)
        if evt.kind == .MousePressed and evt.button == .Left and !mouseInRect
        {
            if list.sigSelected
                list.sigSelected(list, Swag.U32.Max)
            releaseMouse()
            list.destroy()
            return
        }

        if evt.kind == .MouseLeave
            evt.accepted = true
        else
            evt.accepted = false
    }

    mtd onPaintEvent(evt: *PaintEvent)
    {
        painter := evt.bc.painter

        theme   := getTheme()
        colors  := getThemeColors()
        metrics := getThemeMetrics()
        rects   := getThemeRects()

        clientRect := getClientRect()
        clientRect.offset(scrollPos)

        var labelPos: Math.Rectangle = undefined
        var colorText: Color

        y := 0'f32
        visit i, idx: list.items
        {
            labelPos.x = clientRect.x
            labelPos.y = y
            labelPos.width = clientRect.width
            labelPos.height = list.heightItem

            var format: RichStringFormat
            colorText = colors.popupList_Text

            // Separator
            if i.isSeparator
            {
                labelPos.height = list.heightSeparator
                painter.drawLine(labelPos.x + metrics.popupList_SeparatorPadding, labelPos.vertCenter(), labelPos.right() - metrics.popupList_SeparatorPadding, labelPos.vertCenter(), colors.popupList_Separator)
                y += list.heightSeparator
                continue
            }

            // Back selection
            var colorSel: Color
            var colorCheck = colors.popupList_CheckMark
            if hotIdx == idx
            {
                colorSel   = colors.popupList_HotBk
                colorText  = colors.popupList_HotText
                colorCheck = colors.popupList_HotCheckMark
            }

            i.aniSel.apply(self, colorSel)
            i.aniText.apply(self, colorText)
            i.aniCheck.apply(self, colorCheck)

            theme.drawSubRect(painter, labelPos, &rects.selectionBox_Bk, i.aniSel.cur())
            labelPos.moveLeft(metrics.popupList_Padding)

            // Icon
            if i.icon.isValid()
                i.icon.paint(painter, labelPos.x, labelPos.vertCenter() - i.icon.sizeY * 0.5, i.aniText.cur())
            if list.maxIconSize
                labelPos.moveLeft(list.maxIconSize + metrics.popupList_IconTextMargin)

            fntHeight := getFont().regular.height
            if idx == list.selectedIdx
                labelPos.setRight(labelPos.right() - fntHeight - metrics.popupList_IconTextMargin)

            // Text
            theme.setTextColors(&format, i.aniText.cur())
            format.horzAlignment = .Left
            format.vertAlignment = .Center
            format.flags = .Clip
            format.font = getFont()
            painter.drawRichString(labelPos, &i.name, format)

            // Check mark
            if idx == list.selectedIdx
            {
                labelPos.x = labelPos.right()
                labelPos.y = labelPos.vertCenter() - (fntHeight * 0.5)
                labelPos.width = fntHeight
                labelPos.height = fntHeight
                theme.drawSubRect(painter, labelPos, &rects.btnCheck_CheckedMark, i.aniCheck.cur())
            }

            y += list.heightItem
        }
    }
}

impl PopupListCtrl
{
    // Create the popup list, but do not display it
    func create(owner: *Wnd)->*PopupListCtrl
    {
        popup := Wnd.create'PopupListCtrl(owner.getTopView(), owner.position, owner.id)

        with popup
        {
            .setFrameFlags(FrameWndFlags.SmallShadow)
            .setFrameForm(FrameWndForm.Square)
            .scrollWnd = .createView'ScrollWnd()
            .scrollWnd.dockStyle = .Center
            .scrollWnd.focusStrategy = .None
            .scrollWnd.scrollWndFlags |= .SmallBar
            popupView := .scrollWnd.createView'PopupListView()
            popupView.list = popup
        }

        return popup
    }

    // Show popup
    mtd show(x, y: f32, width: f32 = 10)
    {
        scrollWnd.setScrollSize(0, cast(f32) items.count * heightItem)

        pt := parent.surfaceToLocal({x, y})
        pos := Math.Rectangle{pt.x, pt.y, 0, 0}
        pos.width = Math.max(width, minWidthPopup)
        pos.height = Math.min(scrollWnd.scrollSize.y, maxHeightPopup)
        pos.height += padding.y + padding.w
        setPosition(pos)
        sdw := getThemeMetrics().frameWnd_SmallShadowSize
        fitPosInParent(sdw.z, sdw.w, simFitX, simFitY)
        scrollWnd.view.captureMouse()
    }

    // Add a new item in the list
    mtd addItem(name: string, id: WndId = null, icon: Icon = {}, userData0: *void = null, userData1: *void = null)
    {
        item := items.newPtr()
        with item
        {
            .name      = #self.name
            .icon      = #self.icon
            .id        = #self.id
            .userData0 = #self.userData0
            .userData1 = #self.userData1
        }

        if item.icon.isValid()
            maxIconSize = Math.max(maxIconSize, cast(s32) item.icon.sizeX)
        items.add(item)
    }

    // Add a separator
    mtd addSeparator()
    {
        item := items.newPtr()
        item.isSeparator = true
        items.add(item)
    }
}
