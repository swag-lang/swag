#global public
using Core, Pixel

struct PopupListCtrlItem
{
    using base:      Item
    isSeparator:     bool

    aniSel:          BlendColor{speed: 10}
    aniText:         BlendColor{speed: 10}
    aniCheck:        BlendColor{speed: 10}
}

struct PopupListCtrl
{
    using frameWnd:       FrameWnd
    heightItem:           f32 = 30
    heightSeparator:      f32 = 15
    minWidthPopup:        f32 = 100
    maxHeightPopup:       f32 = 300
    simFitX, simFitY:     bool

    sigSelected:          func||(*PopupListCtrl, u32)

    items:                ArrayPtr'PopupListCtrlItem
    selectedIdx           = Swag.U32.Max
    scrollWnd:            *ScrollWnd
    maxIconSize:          s32
}

struct PopupListView
{
    using wnd:     Wnd
    list:          *PopupListCtrl
    hotIdx:        u32 = Swag.U32.Max
}

impl IWnd for PopupListView
{
    mtd impl onFocusEvent(evt: *FocusEvent)
    {
        switch evt.kind
        {
        case KillFocus:
            if evt.other != me.parent
            {
                if me.list.sigSelected:
                    me.list.sigSelected(me.list, Swag.U32.Max)
                else:
                    me.list.destroy()
            }
        }
    }

    mtd impl onMouseEvent(evt: *MouseEvent)
    {
        var hit: u32

        var surfaceRect = me.getSurfaceRect()
        var mouseInRect = surfaceRect.contains(evt.surfacePos)

        var pos = me.surfaceToLocal(evt.surfacePos)
        if evt.kind == .MouseLeave or !mouseInRect:
            hit = Swag.U32.Max
        else
        {
            var y = pos.y + me.scrollPos.y
            foreach v, i in me.list.items
            {
                if v.isSeparator
                {
                    if y <= me.list.heightSeparator
                    {
                        hit = cast(u32) i
                        break
                    }
                    y -= me.list.heightSeparator
                }
                else
                {
                    if y <= me.list.heightItem
                    {
                        hit = cast(u32) i
                        break
                    }
                    y -= me.list.heightItem
                }
            }

            if hit >= me.list.items.count or me.list.items[hit].isSeparator:
                hit = Swag.U32.Max
        }

        if hit != me.hotIdx
        {
            me.hotIdx = hit
            me.invalidate()
        }

        if evt.kind == .MousePressed and evt.button == .Left and mouseInRect
        {
            if me.list.sigSelected:
                me.list.sigSelected(me.list, me.hotIdx)
            me.releaseMouse()
            me.list.destroy()
            return
        }

        surfaceRect = me.parent.getSurfaceRect()
        mouseInRect = surfaceRect.contains(evt.surfacePos)
        if evt.kind == .MousePressed and evt.button == .Left and !mouseInRect
        {
            if me.list.sigSelected:
                me.list.sigSelected(me.list, Swag.U32.Max)
            me.releaseMouse()
            me.list.destroy()
            return
        }

        if evt.kind == .MouseLeave:
            evt.accepted = true
        else:
            evt.accepted = false
    }

    mtd impl onPaintEvent(evt: *PaintEvent)
    {
        let painter = evt.bc.painter

        let theme   = me.getTheme()
        let colors  = me.getThemeColors()
        let metrics = me.getThemeMetrics()
        let rects   = me.getThemeRects()

        var clientRect = me.getClientRect()
        clientRect.offset(me.scrollPos)

        var labelPos:  Math.Rectangle = undefined
        var colorText: Color

        var y = 0'f32
        foreach i, idx in me.list.items
        {
            labelPos.x      = clientRect.x
            labelPos.y      = y
            labelPos.width  = clientRect.width
            labelPos.height = me.list.heightItem

            var format: RichStringFormat
            colorText = colors.popupList_Text

            // Separator
            if i.isSeparator
            {
                labelPos.height = me.list.heightSeparator
                painter.drawLine(labelPos.x + metrics.popupList_SeparatorPadding, labelPos.vertCenter(), labelPos.right() - metrics.popupList_SeparatorPadding, labelPos.vertCenter(), colors.popupList_Separator)
                y += me.list.heightSeparator
                continue
            }

            // Back selection
            var colorSel:  Color
            var colorCheck = colors.popupList_CheckMark
            if me.hotIdx == idx
            {
                colorSel   = colors.popupList_HotBk
                colorText  = colors.popupList_HotText
                colorCheck = colors.popupList_HotCheckMark
            }

            i.aniSel.apply(me, colorSel)
            i.aniText.apply(me, colorText)
            i.aniCheck.apply(me, colorCheck)

            theme.drawSubRect(painter, labelPos, &rects.selectionBox_Bk, i.aniSel.cur())
            labelPos.moveLeft(metrics.popupList_Padding)

            // Icon
            if i.icon.isValid():
                i.icon.paint(painter, labelPos.x, labelPos.vertCenter() - i.icon.sizeY * 0.5, i.aniText.cur())
            if me.list.maxIconSize:
                labelPos.moveLeft(me.list.maxIconSize + metrics.popupList_IconTextMargin)

            let fntHeight = me.getFont().regular.height
            if idx == me.list.selectedIdx:
                labelPos.setRight(labelPos.right() - fntHeight - metrics.popupList_IconTextMargin)

            // Text
            theme.setTextColors(&format, i.aniText.cur())
            format.horzAlignment = .Left
            format.vertAlignment = .Center
            format.flags         = .Clip
            format.font          = me.getFont()
            painter.drawRichString(labelPos, &i.name, format)

            // Check mark
            if idx == me.list.selectedIdx
            {
                labelPos.x      = labelPos.right()
                labelPos.y      = labelPos.vertCenter() - (fntHeight * 0.5)
                labelPos.width  = fntHeight
                labelPos.height = fntHeight
                theme.drawSubRect(painter, labelPos, &rects.btnCheck_CheckedMark, i.aniCheck.cur())
            }

            y += me.list.heightItem
        }
    }
}

impl PopupListCtrl
{
    // Create the popup list, but: not display it
    func create(owner: *Wnd)->*PopupListCtrl
    {
        let popup = Wnd.create'PopupListCtrl(owner.getTopView(), owner.position, owner.id)

        with popup
        {
            .setFrameFlags(.SmallShadow)
            .setFrameForm(.Square)
            .scrollWnd               = .createView'ScrollWnd()
            .scrollWnd.dockStyle     = .Center
            .scrollWnd.focusStrategy = .None
            .scrollWnd.scrollWndFlags |= .SmallBar
            let popupView = .scrollWnd.createView'PopupListView()
            popupView.list = popup
        }

        return popup
    }

    // Show popup
    mtd show(x, y: f32, width: f32 = 10)
    {
        me.scrollWnd.setScrollSize(0, cast(f32) me.items.count * me.heightItem)

        var pt  = me.parent.surfaceToLocal({x, y})
        var pos = Math.Rectangle{pt.x, pt.y, 0, 0}
        pos.width  = Math.max(width, me.minWidthPopup)
        pos.height = Math.min(me.scrollWnd.scrollSize.y, me.maxHeightPopup)
        pos.height += me.padding.y + me.padding.w
        me.setPosition(pos)
        var sdw = me.getThemeMetrics().frameWnd_SmallShadowSize
        me.fitPosInParent(sdw.z, sdw.w, me.simFitX, me.simFitY)
        me.scrollWnd.view.captureMouse()
    }

    // Add a new item in the list
    mtd addItem(name: string, id: WndId = null, icon: Icon = {}, userData0: *void = null, userData1: *void = null)
    {
        let item = me.items.newPtr()
        with item
        {
            .name      = name
            .icon      = icon
            .id        = id
            .userData0 = userData0
            .userData1 = userData1
        }

        if item.icon.isValid():
            me.maxIconSize = Math.max(me.maxIconSize, cast(s32) item.icon.sizeX)
        me.items.add(item)
    }

    // Add a separator
    mtd addSeparator()
    {
        let item = me.items.newPtr()
        item.isSeparator = true
        me.items.add(item)
    }
}
