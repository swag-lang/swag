using Core, Pixel

public enum PopupPos
{
    TopLeft
    AnchorTopCenter
    AnchorBottomCenter
}

#[Swag.EnumFlags]
public enum MenuCtrlFlags
{
    Zero           = 0
    NoKeyShortcuts
}

public struct PopupMenuItem
{
    using item:       Item
    rightName:        RichString
    popup:            *MenuCtrl
    pos:              f32
    size:             f32
    rg:               CommandStateEvent.RenderingHint
    separator:        bool
    disabled:         bool
    checked:          bool
    hidden:           bool

    aniSel:           BlendColor{speed: 50}
    aniText:          BlendColor{speed: 50}
    aniRightText:     BlendColor{speed: 50}
}

public struct MenuCtrl
{
    using frameWnd:       FrameWnd
    onValidateResult:     func(*MenuCtrl, WndId)
    onComputeItem:        func(*MenuCtrl, *PopupMenuItem)
    menuCtrlFlags:        MenuCtrlFlags = Zero

    paddingSel:           f32 = 4
    items:                ArrayPtr'PopupMenuItem
    hotIdx:               u32 = Swag.U32.Max
    selectedIdx:          u32 = Swag.U32.Max
    bar:                  bool
    endModal:             bool
    endModalResult:       WndId
    ownerMenu:            *MenuCtrl
    subVisible:           *MenuCtrl
    timer:                *Timer
    dirtyLayout:          bool
    paintByFrame:         bool
}

public impl IWnd for MenuCtrl
{
    internal mtd paintBar(painter: *Painter)
    {
        let theme   = me.getTheme()
        let colors  = me.getThemeColors()
        let rects   = me.getThemeRects()
        let metrics = me.getThemeMetrics()

        if me.dirtyLayout
        {
            me.dirtyLayout = false
            me.computeLayoutBar()
        }

        // Back
        var clientRect = me.getClientRect()
        if me.backgroundStyle != .Transparent
        {
            theme.drawSubRect(painter, clientRect, &rects.menuBar_Bk, colors.menuBar_Bk)
            theme.drawSubRect(painter, clientRect, &rects.menuBar_Border, colors.menuBar_Border)
        }

        var labelPos:       Math.Rectangle = undefined
        var colorText:      Color
        var colorRightText: Color

        foreach i, idx in me.items
        {
            if i.hidden:
                continue
            if i.separator:
                continue

            labelPos.x      = i.pos
            labelPos.y      = clientRect.y
            labelPos.width  = i.size
            labelPos.height = clientRect.height

            var format: RichStringFormat
            colorText      = i.disabled ? colors.popupItem_DisabledText : colors.popupItem_Text
            colorRightText = i.disabled ? colors.popupItem_DisabledText : colors.popupItem_RightText

            // Back selection
            var colorSel: Color
            if !i.disabled and (me.hotIdx == idx or me.selectedIdx == idx)
            {
                colorSel       = colors.menuBar_SelectedBk
                colorText      = colors.popupItem_HotText
                colorRightText = colors.popupItem_HotRightText
            }

            i.aniSel.apply(me, colorSel)
            i.aniText.apply(me, colorText)

            var backPos = labelPos
            backPos.x -= metrics.menuBar_SpaceBetween * 0.5
            backPos.width += metrics.menuBar_SpaceBetween
            theme.drawSubRect(painter, backPos, &rects.menuBar_SelectedBk, i.aniSel.cur())

            // Icon
            if i.icon.isValid()
            {
                i.icon.sizeX, i.icon.sizeY = i.size - 4
                i.icon.paint(painter, labelPos.x, labelPos.vertCenter() - i.icon.sizeY * 0.5, i.aniText.cur())
            }

            // Text
            theme.setTextColors(&format, i.aniText.cur())
            format.horzAlignment = .Left
            format.vertAlignment = .Center
            format.flags         = .Clip
            format.font          = me.getFont()
            painter.drawRichString(labelPos, &i.name, format)
        }
    }

    internal mtd paintPopup(painter: *Painter)
    {
        let theme   = me.getTheme()
        let colors  = me.getThemeColors()
        let rects   = me.getThemeRects()
        let metrics = me.getThemeMetrics()

        // Shadow
        if !me.paintByFrame
        {
            let subRect = &theme.rects.menuPopup_Shadow
            theme.drawSubRect(painter, {0, 0, me.position.width, me.position.height}, subRect, colors.menuPopup_Shadow)
        }

        // Back
        var clientRect = me.getClientRect()
        if !me.paintByFrame
        {
            clientRect.inflate(-metrics.menuPopup_ShadowSize)
            theme.drawSubRect(painter, clientRect, &rects.menuPopup_Bk, colors.menuPopup_Bk)
            theme.drawSubRect(painter, clientRect, &rects.menuPopup_Border, colors.menuPopup_Border, interpol: .Pixel)
        }

        clientRect.applyPadding(me.padding)

        var labelPos:       Math.Rectangle = undefined
        var colorText:      Color
        var colorRightText: Color

        var lastIsSeparator = true
        foreach i, idx in me.items
        {
            if i.hidden:
                continue
            if i.separator and lastIsSeparator:
                continue

            labelPos.x      = clientRect.x
            labelPos.y      = i.pos
            labelPos.width  = clientRect.width
            labelPos.height = i.size

            lastIsSeparator = i.separator
            if i.separator
            {
                painter.drawLine(labelPos.x, labelPos.vertCenter(), labelPos.right(), labelPos.vertCenter(), Pen.createSolid(colors.menuPopup_Separator))
                continue
            }

            colorText      = i.disabled ? colors.popupItem_DisabledText : colors.popupItem_Text
            colorRightText = i.disabled ? colors.popupItem_DisabledText : colors.popupItem_RightText

            // Back selection
            var colorSel: Color
            if me.hotIdx == idx and !i.disabled
            {
                colorSel       = colors.popupItem_HotBk
                colorText      = colors.popupItem_HotText
                colorRightText = colors.popupItem_HotRightText
            }

            i.aniSel.apply(me, colorSel)
            i.aniText.apply(me, colorText)
            i.aniRightText.apply(me, colorRightText)

            var backPos = labelPos
            backPos.y -= metrics.menuPopup_SpaceLines * 0.5
            backPos.height += metrics.menuPopup_SpaceLines
            backPos.inflate(me.paddingSel, 0)
            theme.drawSubRect(painter, backPos, &rects.menuPopup_HotBk, i.aniSel.cur())

            // Icon
            var iconPos = labelPos
            iconPos.x += metrics.menuPopup_IconMargin
            if i.checked or i.rg == .ExclusiveGroup
            {
                var checkPos = iconPos
                checkPos.y = iconPos.vertCenter() - i.size * 0.5
                checkPos.width, checkPos.height = i.size

                if i.rg == .ExclusiveGroup
                {
                    theme.drawSubRect(painter, checkPos, &rects.btnRadio_CheckedBorder, i.aniText.cur())
                    if i.checked:
                        theme.drawSubRect(painter, checkPos, &rects.btnRadio_CheckedMark, i.aniText.cur())
                }
                else:
                    theme.drawSubRect(painter, checkPos, &rects.btnCheck_CheckedMark, i.aniText.cur())
            }
            elif i.icon.isValid()
            {
                i.icon.sizeX, i.icon.sizeY = i.size - 4
                i.icon.paint(painter, iconPos.x, iconPos.vertCenter() - i.icon.sizeY * 0.5, i.aniText.cur())
            }

            labelPos.moveLeft(metrics.menuPopup_IconTextMargin + i.size)
            labelPos.width -= i.size

            // Text
            var format: RichStringFormat
            theme.setTextColors(&format, i.aniText.cur())
            format.horzAlignment = .Left
            format.vertAlignment = .Center
            format.flags         = .Clip
            format.font          = me.getFont()
            painter.drawRichString(labelPos, &i.name, format)

            // Text right
            if !me.menuCtrlFlags.has(.NoKeyShortcuts) and !i.rightName.isEmpty()
            {
                format.palette[0]    = i.aniRightText.cur()
                format.horzAlignment = .Right
                format.vertAlignment = .Center
                format.flags         = .Clip
                format.font          = me.getFont()
                painter.drawRichString(labelPos, &i.rightName, format)
            }

            // Right icon
            if i.popup
            {
                var icon = theme.getIcon24(ThemeIcons24.RightPopupArrow)
                icon.paint(painter, labelPos.right(), labelPos.vertCenter() - icon.sizeY * 0.5, colorText)
            }
        }
    }

    mtd impl onPaintEvent(evt: *PaintEvent)
    {
        me.frameWnd.IWnd.onPaintEvent(evt)
        if me.bar:
            me.paintBar(evt.bc.painter)
        else:
            me.paintPopup(evt.bc.painter)
    }

    mtd impl onTimerEvent(evt: *TimerEvent)
    {
        me.hideSubVisible()
        me.deleteTimer()
        me.showHotPopup()
    }

    mtd impl onMouseEvent(evt: *MouseEvent)
    {
        let hit = me.hitTest(evt.surfacePos)
        if hit != me.hotIdx
        {
            // Timer to auto open/hide sub menus
            if !me.bar
            {
                if !me.timer:
                    me.timer = me.surface.app.addTimer(300'ms, me)
                me.timer.restart()
            }

            me.hotIdx = hit
            me.invalidate()

            if hit != Swag.U32.Max
            {
                var p = me.ownerMenu
                while p
                {
                    p.deleteTimer()
                    p = p.ownerMenu
                }
            }
        }

        evt.accepted = true
        if me.hotIdx == Swag.U32.Max and me.ownerMenu
        {
            me.ownerMenu.onMouseEvent(evt)
            return
        }

        if me.hotIdx == Swag.U32.Max and me.bar and !me.subVisible
        {
            evt.accepted = false
            return
        }

        // If this is a bar, then track sub popup if one is currently open
        if me.bar and me.subVisible and me.hotIdx != Swag.U32.Max
        {
            me.hideSubVisible()
            me.deleteTimer()
            me.showHotPopup()
        }

        // On a popup menu, just react to press button
        elif evt.kind == .MouseReleased and evt.button == .Left
        {
            if me.hotIdx != Swag.U32.Max
            {
                let hotItem = me.items[me.hotIdx]
                if hotItem.popup
                {
                    me.hideSubVisible()
                    me.deleteTimer()
                    me.showHotPopup()
                }
                elif !hotItem.separator and !hotItem.disabled
                {
                    me.setEndModal(hotItem.id)
                }
            }
            else
            {
                me.setEndModal(null)
            }
        }
    }
}

impl MenuCtrl
{
    mtd setEndModal(result: WndId)
    {
        var p = me
        while p.ownerMenu
        {
            p.hide()
            p = p.ownerMenu
        }

        p.endModal       = true
        p.endModalResult = result

        if me.onValidateResult and result != null:
            me.onValidateResult(me, result)

        if me.bar
        {
            me.hideSubVisible()
            me.postCommandEvent(result)
            me.setFocus()
        }

        if p.bar
        {
            p.selectedIdx = Swag.U32.Max
            p.hotIdx      = Swag.U32.Max
            p.subVisible  = null
            p.postCommandEvent(result)
        }
    }

    mtd hideSubVisible()
    {
        while me.subVisible
        {
            me.subVisible.hide()
            me.subVisible = me.subVisible.subVisible
        }

        me.selectedIdx = Swag.U32.Max
        if !me.bar:
            me.captureMouse()
        else:
            me.releaseMouse()
    }

    mtd deleteTimer()
    {
        if !me.timer:
            return
        me.surface.app.deleteTimer(me.timer)
        me.timer = null
    }

    mtd resetAnim()
    {
        foreach i in me.items
        {
            i.aniSel.reset()
            i.aniText.reset()
            i.aniRightText.reset()
        }
    }

    public mtd updateState()
    {
        foreach i in me.items
        {
            var evt = me.sendComputeCommandStateEvent(i.id)
            if evt.setFlags & .Disabled:
                i.disabled = evt.disabled
            if evt.setFlags & .Hidden:
                i.hidden = evt.hidden
            if evt.setFlags & .Icon:
                i.icon = evt.icon
            if evt.setFlags & .LongName:
                i.name = evt.longName
            elif evt.setFlags & .Name:
                i.name = evt.name
            if evt.setFlags & .Shortcut:
                i.rightName.set(evt.shortcut)
            if evt.setFlags & .Checked:
                i.checked = evt.checked
            if evt.setFlags & .RenderingHint:
                i.rg = evt.renderingHint
        }
    }

    mtd showSubVisible(ptSurface: Math.Point)
    {
        with me.subVisible
        {
            var pt = .parent.surfaceToLocal(ptSurface)
            .setPosition({pt.x, pt.y, .position.width, .position.height})
            .bringToTop()
            .show()
            .captureMouse()
            .resetAnim()
            .hotIdx      = Swag.U32.Max
            .selectedIdx = Swag.U32.Max
            .subVisible  = null
            .timer       = null
        }
    }

    mtd showHotPopupBar()
    {
        me.selectedIdx = me.hotIdx
        let hotItem = me.items[me.hotIdx]
        let metrics = me.getThemeMetrics()

        var pos = me.getSurfaceRect()
        pos.x += hotItem.pos
        pos.x -= metrics.menuPopup_ShadowSize
        pos.x -= metrics.menuBar_SpaceBetween * 0.5
        pos.y += pos.height
        pos.y -= metrics.menuPopup_ShadowSize
        me.subVisible = hotItem.popup

        // Be sure popup menu is in the client area of the top view
        me.subVisible.updateState()
        me.subVisible.computeLayoutPopup()
        let topView = me.getTopView()
        var pt      = topView.surfaceToLocal({pos.x, pos.y})
        var subPos  = me.subVisible.position
        if pt.x + subPos.width > topView.position.width:
            pt.x = topView.position.width - subPos.width
        if pt.y + subPos.height > topView.position.height:
            pt.y = topView.position.height - subPos.height

        pt = topView.localToSurface(pt)
        me.showSubVisible(pt)
    }

    mtd showHotPopupPopup()
    {
        me.selectedIdx = me.hotIdx
        let hotItem = me.items[me.hotIdx]

        var pos         = me.getSurfaceRect()
        let sdw         = me.getThemeMetrics().menuPopup_ShadowSize
        var surfaceRect = pos
        pos.x = pos.right() - sdw
        pos.y += hotItem.pos
        pos.y -= me.padding.y
        me.subVisible = hotItem.popup

        // Be sure popup menu is in the client area of the top view
        me.subVisible.updateState()
        me.subVisible.computeLayoutPopup()
        let topView = me.getTopView()
        var pt      = topView.surfaceToLocal({pos.x, pos.y})
        pt -= sdw
        var subPos = me.subVisible.position
        if pt.x + subPos.width > topView.position.width:
            pt.x = surfaceRect.x - subPos.width
        if pt.y + subPos.height > topView.position.height:
            pt.y = surfaceRect.y - subPos.height

        pt = topView.localToSurface(pt)
        me.showSubVisible(pt)
    }

    mtd showHotPopup()
    {
        if me.hotIdx == Swag.U32.Max or !me.items[me.hotIdx].popup
        {
            me.selectedIdx = Swag.U32.Max
            return
        }

        if me.bar:
            me.showHotPopupBar()
        else:
            me.showHotPopupPopup()
    }

    mtd hitTestBar(surfacePos: Math.Point)->u32
    {
        var pos        = me.surfaceToLocal(surfacePos)
        let metrics    = me.getThemeMetrics()
        var clientRect = me.getClientRect()
        let sl         = metrics.menuBar_SpaceBetween
        foreach i, idx in me.items
        {
            var rect = Math.Rectangle{i.pos - sl * 0.5, clientRect.y, i.size + sl, clientRect.height}
            if rect.contains(pos):
                return cast(u32) idx
        }

        return Swag.U32.Max
    }

    mtd hitTestPopup(surfacePos: Math.Point)->u32
    {
        var pos        = me.surfaceToLocal(surfacePos)
        let metrics    = me.getThemeMetrics()
        var clientRect = me.getClientRect()
        let sl         = metrics.menuPopup_SpaceLines
        foreach i, idx in me.items
        {
            var rect = Math.Rectangle{clientRect.x, i.pos - sl * 0.5, clientRect.width, i.size + sl}
            if rect.contains(pos):
                return cast(u32) idx
        }

        return Swag.U32.Max
    }

    mtd hitTest(surfacePos: Math.Point)->u32
    {
        return me.bar ? me.hitTestBar(surfacePos) : me.hitTestPopup(surfacePos)
    }

    mtd computeLayoutBar()
    {
        let metrics    = me.getThemeMetrics()
        var clientRect = me.getClientRectPadding()

        var x = clientRect.x

        var fake: Pixel.Painter{fake: true}
        var rsf:  Pixel.RichStringFormat
        rsf.font = me.getFont()

        foreach i, idx in me.items
        {
            if i.hidden:
                continue
            if me.onComputeItem:
                me.onComputeItem(i)
            i.pos = x + metrics.menuBar_SpaceBetween * 0.5
            fake.drawRichString({0, 0, 512, 0}, &i.name, rsf)
            i.size = i.name.boundRect.width
            x      = i.pos + i.size + metrics.menuBar_SpaceBetween * 0.5
        }
    }

    public mtd computeLayoutPopup()
    {
        let metrics = me.getThemeMetrics()

        var clientRect = me.getClientRect()
        if !me.paintByFrame:
            clientRect.inflate(-metrics.menuPopup_ShadowSize)
        clientRect.applyPadding(me.padding)

        me.position.width  = 0
        me.position.height = clientRect.y

        var fake: Pixel.Painter{fake: true}
        var rsf:  Pixel.RichStringFormat
        rsf.font = me.getFont()

        var lastIsSeparator = true
        foreach i, idx in me.items
        {
            if i.hidden:
                continue
            if i.separator and lastIsSeparator:
                continue

            if me.onComputeItem:
                me.onComputeItem(i)
            if idx:
                me.position.height += metrics.menuPopup_SpaceLines
            i.pos = me.position.height

            lastIsSeparator = i.separator
            if i.separator
            {
                i.size = metrics.menuPopup_SeparatorHeight
                me.position.height += i.size
                continue
            }

            i.size = rsf.font.regular.height

            fake.drawRichString({0, 0, 512, 0}, &i.name, rsf)
            var w = i.name.boundRect.width

            if !me.menuCtrlFlags.has(.NoKeyShortcuts) and !i.rightName.isEmpty()
            {
                fake.drawRichString({0, 0, 512, 0}, &i.rightName, rsf)
                w += i.rightName.boundRect.width + rsf.font.regular.height
            }

            me.position.width = Math.max(me.position.width, w)
            me.position.height += i.size
        }

        // Add shadow size
        if !me.paintByFrame:
            me.position.width += 2 * metrics.menuPopup_ShadowSize

        me.position.width += (2 * rsf.font.regular.height) + (1 * metrics.menuPopup_IconTextMargin)

        if !me.paintByFrame:
            me.position.height += metrics.menuPopup_ShadowSize

        // Add padding
        me.position.width += me.padding.x + me.padding.z
        me.position.height += me.padding.w
    }
}

public impl MenuCtrl
{
    #[Swag.Discardable]
    func createPopup(parent: *Wnd, id: WndId = null, owner: *Wnd = null)->*MenuCtrl
    {
        with let res = Wnd.create'MenuCtrl(parent.getTopView(), null, {}, id)
        {
            .owner = owner orelse parent
            .setAllPaddings(.getThemeMetrics().menuPopup_Padding)
            .wndFlags |= .Hidden
        }

        return res
    }

    #[Swag.Discardable]
    func createBar(parent: *Wnd, pos: Math.Rectangle = {}, id: WndId = null, owner: *Wnd = null)->*MenuCtrl
    {
        with let res = Wnd.create'MenuCtrl(parent, null, pos, id)
        {
            .owner = owner
            let metrics = .getThemeMetrics()
            if .position.height == 0:
                .position.height = metrics.menuBar_Height
            .setAllPaddings(metrics.menuPopup_Padding)
            .bar = true
        }

        return res
    }

    // Add a new item
    #[Swag.Overload]
    mtd addItem(id: WndId)
    {
        me.addItem(null, id)
    }

    // Add a new item
    #[Swag.Overload]
    mtd addItem(name: #null string, id: WndId, icon: Icon = {}, rightName: string = null, disabled = false, checked = false)
    {
        with let mitem = me.items.newPtr()
        {
            .name      = name
            .icon      = icon
            .id        = id
            .rightName = rightName
            .disabled  = disabled
            .checked   = checked
        }

        me.items.add(mitem)
        me.dirtyLayout = true
    }

    // Add a new popup item
    #[Swag.Overload]
    mtd addPopup(popup: *MenuCtrl, id: WndId)
    {
        me.addPopup(null, popup, id: id)
    }

    // Add a new popup item
    #[Swag.Overload]
    mtd addPopup(name: #null string, popup: *MenuCtrl, icon: Icon = {}, id: WndId = null)
    {
        with let mitem = me.items.newPtr()
        {
            .name  = name
            .icon  = icon
            .popup = popup
            .id    = id
        }

        popup.ownerMenu = me
        me.items.add(mitem)
        me.dirtyLayout = true
    }

    // Add a separator
    mtd addSeparator()
    {
        let item = me.items.newPtr()
        item.separator = true
        me.items.add(item)
        me.dirtyLayout = true
    }

    // Make the popup menu modal
    // Will return the selected id
    mtd doModal(surfacePos: Math.Point, pos = PopupPos.TopLeft, autoDestroy: bool = false)->WndId
    {
        Debug.assert(!me.bar)

        me.updateState()

        switch pos
        {
        case AnchorTopCenter:
            me.paintByFrame = true
            me.padding.y += 4
        case AnchorBottomCenter:
            me.paintByFrame = true
            me.padding.w += 4
        default:
            me.paintByFrame = false
        }

        me.computeLayoutPopup()

        // Be sure popup menu is in the client area of the top view
        var pt = me.parent.surfaceToLocal(surfacePos)

        switch pos
        {
        case TopLeft:
            pt -= me.getThemeMetrics().menuPopup_ShadowSize
            me.setPosition({pt.x, pt.y, me.position.width, me.position.height})

        case AnchorTopCenter:
            pt.y += me.getThemeMetrics().menuPopup_ShadowSize
            me.setPosition({pt.x - me.position.width * 0.5, pt.y, me.position.width, me.position.height})
            me.anchor    = .Top
            me.anchorPos = me.position.width * 0.5
            me.setFrameFlags(.Borders | .SmallShadow)
            me.setFrameForm(.Round)
            me.padding.y += 14
            me.paddingSel = 0

        case AnchorBottomCenter:
            pt.y -= me.getThemeMetrics().menuPopup_ShadowSize
            me.setPosition({pt.x - me.position.width * 0.5, pt.y - me.position.height, me.position.width, me.position.height})
            me.anchor    = .Bottom
            me.anchorPos = me.position.width * 0.5
            me.setFrameFlags(.Borders | .SmallShadow)
            me.setFrameForm(.Round)
            me.paddingSel = 0
        }

        me.fitPosInParent(simX: true, simY: true)

        me.bringToTop()
        me.show()
        me.captureMouse()

        me.endModal       = false
        me.endModalResult = null
        me.getApp().inModalLoop += 1
        while !me.endModal:
            me.surface.app.runFrame()
        me.getApp().inModalLoop -= 1

        me.hideSubVisible()
        me.hide()

        if me.endModalResult:
            me.postCommandEvent(me.endModalResult)

        if autoDestroy:
            me.destroy()

        return me.endModalResult
    }
}
