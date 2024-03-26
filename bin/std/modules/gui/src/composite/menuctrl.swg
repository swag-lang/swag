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
    separator:        bool
    disabled:         bool
    checked:          bool
    hidden:           bool

    aniSel:           BlendColor{speed: 10}
    aniText:          BlendColor{speed: 10}
    aniRightText:     BlendColor{speed: 10}
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
        let theme   = getTheme()
        let colors  = getThemeColors()
        let rects   = getThemeRects()
        let metrics = getThemeMetrics()

        if dirtyLayout
        {
            dirtyLayout = false
            computeLayoutBar()
        }

        // Back
        var clientRect = getClientRect()
        if backgroundStyle != .Transparent
        {
            theme.drawSubRect(painter, clientRect, &rects.menuBar_Bk, colors.menuBar_Bk)
            theme.drawSubRect(painter, clientRect, &rects.menuBar_Border, colors.menuBar_Border)
        }

        var labelPos:       Math.Rectangle = undefined
        var colorText:      Color
        var colorRightText: Color

        visit i, idx: items
        {
            if i.hidden do
                continue
            if i.separator do
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
            if !i.disabled and (hotIdx == idx or selectedIdx == idx)
            {
                colorSel       = colors.menuBar_SelectedBk
                colorText      = colors.popupItem_HotText
                colorRightText = colors.popupItem_HotRightText
            }

            i.aniSel.apply(self, colorSel)
            i.aniText.apply(self, colorText)

            var backPos = labelPos
            backPos.x -= metrics.menuBar_SpaceBetween * 0.5
            backPos.width += metrics.menuBar_SpaceBetween
            theme.drawSubRect(painter, backPos, &rects.menuBar_SelectedBk, i.aniSel.cur())

            // Icon
            if i.icon.isValid()
            {
                i.icon.sizeX = i.size
                i.icon.sizeY = i.size
                i.icon.paint(painter, labelPos.x, labelPos.vertCenter() - i.icon.sizeY * 0.5, i.aniText.cur())
            }

            // Text
            theme.setTextColors(&format, i.aniText.cur())
            format.horzAlignment = .Left
            format.vertAlignment = .Center
            format.flags         = .Clip
            format.font          = getFont()
            painter.drawRichString(labelPos, &i.name, format)
        }
    }

    internal mtd paintPopup(painter: *Painter)
    {
        let theme   = getTheme()
        let colors  = getThemeColors()
        let rects   = getThemeRects()
        let metrics = getThemeMetrics()

        // Shadow
        if !paintByFrame
        {
            let subRect = &theme.rects.menuPopup_Shadow
            theme.drawSubRect(painter, {0, 0, position.width, position.height}, subRect, colors.menuPopup_Shadow)
        }

        // Back
        var clientRect = getClientRect()
        if !paintByFrame
        {
            clientRect.inflate(-metrics.menuPopup_ShadowSize)
            theme.drawSubRect(painter, clientRect, &rects.menuPopup_Bk, colors.menuPopup_Bk)
            theme.drawSubRect(painter, clientRect, &rects.menuPopup_Border, colors.menuPopup_Border, interpol: .Pixel)
        }

        clientRect.applyPadding(padding)

        var labelPos:       Math.Rectangle = undefined
        var colorText:      Color
        var colorRightText: Color

        var lastIsSeparator = true
        visit i, idx: items
        {
            if i.hidden do
                continue
            if i.separator and lastIsSeparator do
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
            if hotIdx == idx and !i.disabled
            {
                colorSel       = colors.popupItem_HotBk
                colorText      = colors.popupItem_HotText
                colorRightText = colors.popupItem_HotRightText
            }

            i.aniSel.apply(self, colorSel)
            i.aniText.apply(self, colorText)
            i.aniRightText.apply(self, colorRightText)

            var backPos = labelPos
            backPos.y -= metrics.menuPopup_SpaceLines * 0.5
            backPos.height += metrics.menuPopup_SpaceLines
            backPos.inflate(paddingSel, 0)
            theme.drawSubRect(painter, backPos, &rects.menuPopup_HotBk, i.aniSel.cur())

            // Icon
            if i.checked
            {
                var checkPos = labelPos
                checkPos.y = labelPos.vertCenter() - i.size * 0.5
                checkPos.width, checkPos.height = i.size
                theme.drawSubRect(painter, checkPos, &rects.btnCheck_CheckedMark, i.aniText.cur())
            }
            elif i.icon.isValid()
            {
                i.icon.sizeX = i.size
                i.icon.sizeY = i.size
                i.icon.paint(painter, labelPos.x, labelPos.vertCenter() - i.icon.sizeY * 0.5, i.aniText.cur())
            }

            labelPos.moveLeft(metrics.menuPopup_IconTextMargin + i.size)
            labelPos.width -= i.size

            // Text
            var format: RichStringFormat
            theme.setTextColors(&format, i.aniText.cur())
            format.horzAlignment = .Left
            format.vertAlignment = .Center
            format.flags         = .Clip
            format.font          = getFont()
            painter.drawRichString(labelPos, &i.name, format)

            // Text right
            if !menuCtrlFlags.has(.NoKeyShortcuts) and !i.rightName.isEmpty()
            {
                format.palette[0]    = i.aniRightText.cur()
                format.horzAlignment = .Right
                format.vertAlignment = .Center
                format.flags         = .Clip
                format.font          = getFont()
                painter.drawRichString(labelPos, &i.rightName, format)
            }

            // Right icon
            if i.popup
            {
                var icon = theme.getIcon24(ThemeIcons24.RightPopupArrow)
                icon.paint(painter, labelPos.right(), labelPos.vertCenter() - icon.sizeY * 0.5, colors.popupItem_Text)
            }
        }
    }

    mtd impl onPaintEvent(evt: *PaintEvent)
    {
        frameWnd.IWnd.onPaintEvent(evt)
        if bar do
            paintBar(evt.bc.painter)
        else do
            paintPopup(evt.bc.painter)
    }

    mtd impl onTimerEvent(evt: *TimerEvent)
    {
        hideSubVisible()
        deleteTimer()
        showHotPopup()
    }

    mtd impl onMouseEvent(evt: *MouseEvent)
    {
        let hit = hitTest(evt.surfacePos)
        if hit != hotIdx
        {
            // Timer to auto open/hide sub menus
            if !bar
            {
                if !timer do
                    timer = surface.app.addTimer(300'ms, self)
                timer.restart()
            }

            hotIdx = hit
            invalidate()

            if hit != Swag.U32.Max
            {
                var p = ownerMenu
                while p
                {
                    p.deleteTimer()
                    p = p.ownerMenu
                }
            }
        }

        evt.accepted = true
        if hotIdx == Swag.U32.Max and ownerMenu
        {
            ownerMenu.onMouseEvent(evt)
            return
        }

        if hotIdx == Swag.U32.Max and bar and !subVisible
        {
            evt.accepted = false
            return
        }

        // If this is a bar, then track sub popup if one is currently open
        if bar and subVisible and hotIdx != Swag.U32.Max
        {
            hideSubVisible()
            deleteTimer()
            showHotPopup()
        }

        // On a popup menu, just react to press button
        elif evt.kind == .MouseReleased and evt.button == .Left
        {
            if hotIdx != Swag.U32.Max
            {
                let hotItem = items[hotIdx]
                if hotItem.popup
                {
                    hideSubVisible()
                    deleteTimer()
                    showHotPopup()
                }
                elif !hotItem.separator and !hotItem.disabled
                {
                    setEndModal(hotItem.id)
                }
            }
            else
            {
                setEndModal(null)
            }
        }
    }
}

impl MenuCtrl
{
    mtd setEndModal(result: WndId)
    {
        var p = self
        while p.ownerMenu
        {
            p.hide()
            p = p.ownerMenu
        }

        p.endModal       = true
        p.endModalResult = result

        if onValidateResult and result != null do
            onValidateResult(self, result)

        if bar
        {
            hideSubVisible()
            postCommandEvent(result)
            setFocus()
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
        while subVisible
        {
            subVisible.hide()
            subVisible = subVisible.subVisible
        }

        selectedIdx = Swag.U32.Max
        if !bar do
            captureMouse()
        else do
            releaseMouse()
    }

    mtd deleteTimer()
    {
        if !timer do
            return
        surface.app.deleteTimer(timer)
        timer = null
    }

    public mtd updateState()
    {
        visit i: items
        {
            var evt = sendComputeCommandStateEvent(i.id)
            if evt.setFlags & .Disabled do
                i.disabled = evt.disabled
            if evt.setFlags & .Hidden do
                i.hidden = evt.hidden
            if evt.setFlags & .Icon do
                i.icon = evt.icon
            if evt.setFlags & .LongName do
                i.name = evt.longName
            elif evt.setFlags & .Name do
                i.name = evt.name
            if evt.setFlags & .Shortcut do
                i.rightName.set(evt.shortcut)
            if evt.setFlags & .Checked do
                i.checked = evt.checked
        }
    }

    mtd showSubVisible(ptSurface: Math.Point)
    {
        with subVisible
        {
            var pt = .parent.surfaceToLocal(ptSurface)
            .setPosition({pt.x, pt.y, .position.width, .position.height})
            .bringToTop()
            .show()
            .captureMouse()
            .hotIdx      = Swag.U32.Max
            .selectedIdx = Swag.U32.Max
            .subVisible  = null
            .timer       = null
        }
    }

    mtd showHotPopupBar()
    {
        selectedIdx = hotIdx
        let hotItem = items[hotIdx]
        let metrics = getThemeMetrics()

        var pos = getSurfaceRect()
        pos.x += hotItem.pos
        pos.x -= metrics.menuPopup_ShadowSize
        pos.x -= metrics.menuBar_SpaceBetween * 0.5
        pos.y += pos.height
        pos.y -= metrics.menuPopup_ShadowSize
        subVisible = hotItem.popup

        // Be sure popup menu is in the client area of the top view
        subVisible.updateState()
        subVisible.computeLayoutPopup()
        let topView = getTopView()
        var pt      = topView.surfaceToLocal({pos.x, pos.y})
        var subPos  = subVisible.position
        if pt.x + subPos.width > topView.position.width do
            pt.x = topView.position.width - subPos.width
        if pt.y + subPos.height > topView.position.height do
            pt.y = topView.position.height - subPos.height

        pt = topView.localToSurface(pt)
        showSubVisible(pt)
    }

    mtd showHotPopupPopup()
    {
        selectedIdx = hotIdx
        let hotItem = items[hotIdx]

        var pos         = getSurfaceRect()
        let sdw         = getThemeMetrics().menuPopup_ShadowSize
        var surfaceRect = pos
        pos.x = pos.right() - sdw
        pos.y += hotItem.pos
        pos.y -= padding.y
        subVisible = hotItem.popup

        // Be sure popup menu is in the client area of the top view
        subVisible.updateState()
        subVisible.computeLayoutPopup()
        let topView = getTopView()
        var pt      = topView.surfaceToLocal({pos.x, pos.y})
        pt -= sdw
        var subPos = subVisible.position
        if pt.x + subPos.width > topView.position.width do
            pt.x = surfaceRect.x - subPos.width
        if pt.y + subPos.height > topView.position.height do
            pt.y = surfaceRect.y - subPos.height

        pt = topView.localToSurface(pt)
        showSubVisible(pt)
    }

    mtd showHotPopup()
    {
        if hotIdx == Swag.U32.Max or !items[hotIdx].popup
        {
            selectedIdx = Swag.U32.Max
            return
        }

        if bar do
            showHotPopupBar()
        else do
            showHotPopupPopup()
    }

    mtd hitTestBar(surfacePos: Math.Point)->u32
    {
        var pos        = surfaceToLocal(surfacePos)
        let metrics    = getThemeMetrics()
        var clientRect = getClientRect()
        let sl         = metrics.menuBar_SpaceBetween
        visit i, idx: items
        {
            var rect = Math.Rectangle{i.pos - sl * 0.5, clientRect.y, i.size + sl, clientRect.height}
            if rect.contains(pos) do
                return cast(u32) idx
        }

        return Swag.U32.Max
    }

    mtd hitTestPopup(surfacePos: Math.Point)->u32
    {
        var pos        = surfaceToLocal(surfacePos)
        let metrics    = getThemeMetrics()
        var clientRect = getClientRect()
        let sl         = metrics.menuPopup_SpaceLines
        visit i, idx: items
        {
            var rect = Math.Rectangle{clientRect.x, i.pos - sl * 0.5, clientRect.width, i.size + sl}
            if rect.contains(pos) do
                return cast(u32) idx
        }

        return Swag.U32.Max
    }

    mtd hitTest(surfacePos: Math.Point)->u32
    {
        return bar ? hitTestBar(surfacePos) : hitTestPopup(surfacePos)
    }

    mtd computeLayoutBar()
    {
        let metrics    = getThemeMetrics()
        var clientRect = getClientRectPadding()

        var x = clientRect.x

        var fake: Pixel.Painter{fake: true}
        var rsf:  Pixel.RichStringFormat
        rsf.font = getFont()

        visit i, idx: items
        {
            if i.hidden do
                continue
            if onComputeItem do
                onComputeItem(self, i)
            i.pos = x + metrics.menuBar_SpaceBetween * 0.5
            fake.drawRichString({0, 0, 512, 0}, &i.name, rsf)
            i.size = i.name.boundRect.width
            x      = i.pos + i.size + metrics.menuBar_SpaceBetween * 0.5
        }
    }

    public mtd computeLayoutPopup()
    {
        let metrics = getThemeMetrics()

        var clientRect = getClientRect()
        if !paintByFrame do
            clientRect.inflate(-metrics.menuPopup_ShadowSize)
        clientRect.applyPadding(padding)

        position.width  = 0
        position.height = clientRect.y

        var fake: Pixel.Painter{fake: true}
        var rsf:  Pixel.RichStringFormat
        rsf.font = getFont()

        var lastIsSeparator = true
        visit i, idx: items
        {
            if i.hidden do
                continue
            if i.separator and lastIsSeparator do
                continue

            if onComputeItem do
                onComputeItem(self, i)
            if idx do
                position.height += metrics.menuPopup_SpaceLines
            i.pos = position.height

            lastIsSeparator = i.separator
            if i.separator
            {
                i.size = metrics.menuPopup_SeparatorHeight
                position.height += i.size
                continue
            }

            i.size = rsf.font.regular.height

            fake.drawRichString({0, 0, 512, 0}, &i.name, rsf)
            var w = i.name.boundRect.width

            if !menuCtrlFlags.has(.NoKeyShortcuts) and !i.rightName.isEmpty()
            {
                fake.drawRichString({0, 0, 512, 0}, &i.rightName, rsf)
                w += i.rightName.boundRect.width + rsf.font.regular.height
            }

            position.width = Math.max(position.width, w)
            position.height += i.size
        }

        // Add shadow size
        if !paintByFrame do
            position.width += 2 * metrics.menuPopup_ShadowSize

        position.width += (2 * rsf.font.regular.height) + (1 * metrics.menuPopup_IconTextMargin)

        if !paintByFrame do
            position.height += metrics.menuPopup_ShadowSize

        // Add padding
        position.width += padding.x + padding.z
        position.height += padding.w
    }
}

public impl MenuCtrl
{
    #[Swag.Discardable]
    func createPopup(parent: *Wnd, id: WndId = null, owner: *Wnd = null)->*MenuCtrl
    {
        with let res = Wnd.create'MenuCtrl(parent.getTopView(), null, {}, id)
        {
            .owner = owner orelse #self.parent
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
            if .position.height == 0 do
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
        self.addItem(null, #self.id)
    }

    // Add a new item
    #[Swag.Overload]
    mtd addItem(name: string, id: WndId, icon: Icon = {}, rightName: string = null, disabled = false, checked = false)
    {
        with let mitem = items.newPtr()
        {
            .name      = #self.name
            .icon      = #self.icon
            .id        = #self.id
            .rightName = rightName
            .disabled  = disabled
            .checked   = checked
        }

        items.add(mitem)
        dirtyLayout = true
    }

    // Add a new popup item
    #[Swag.Overload]
    mtd addPopup(popup: *MenuCtrl, id: WndId)
    {
        self.addPopup(null, popup, id: #self.id)
    }

    // Add a new popup item
    #[Swag.Overload]
    mtd addPopup(name: string, popup: *MenuCtrl, icon: Icon = {}, id: WndId = null)
    {
        with let mitem = items.newPtr()
        {
            .name  = #self.name
            .icon  = #self.icon
            .popup = #self.popup
            .id    = #self.id
        }

        popup.ownerMenu = self
        items.add(mitem)
        dirtyLayout = true
    }

    // Add a separator
    mtd addSeparator()
    {
        let item = items.newPtr()
        item.separator = true
        items.add(item)
        dirtyLayout = true
    }

    // Make the popup menu modal
    // Will return the selected id
    mtd doModal(surfacePos: Math.Point, pos = PopupPos.TopLeft, autoDestroy: bool = false)->WndId
    {
        Debug.assert(!bar)

        updateState()

        switch pos
        {
        case AnchorTopCenter:
            paintByFrame = true
            padding.y += 4
        case AnchorBottomCenter:
            paintByFrame = true
            padding.w += 4
        default:
            paintByFrame = false
        }

        computeLayoutPopup()

        // Be sure popup menu is in the client area of the top view
        var pt = parent.surfaceToLocal(surfacePos)

        switch pos
        {
        case TopLeft:
            pt -= getThemeMetrics().menuPopup_ShadowSize
            setPosition({pt.x, pt.y, position.width, position.height})

        case AnchorTopCenter:
            pt.y += getThemeMetrics().menuPopup_ShadowSize
            setPosition({pt.x - position.width * 0.5, pt.y, position.width, position.height})
            anchor    = .Top
            anchorPos = position.width * 0.5
            setFrameFlags(.Borders | .SmallShadow)
            setFrameForm(.Round)
            padding.y += 14
            paddingSel = 0

        case AnchorBottomCenter:
            pt.y -= getThemeMetrics().menuPopup_ShadowSize
            setPosition({pt.x - position.width * 0.5, pt.y - position.height, position.width, position.height})
            anchor    = .Bottom
            anchorPos = position.width * 0.5
            setFrameFlags(.Borders | .SmallShadow)
            setFrameForm(.Round)
            paddingSel = 0
        }

        fitPosInParent(simX: true, simY: true)

        bringToTop()
        show()
        captureMouse()

        endModal       = false
        endModalResult = null
        while !endModal do
            surface.app.runFrame()

        hideSubVisible()
        hide()

        if endModalResult do
            postCommandEvent(endModalResult)

        if autoDestroy do
            destroy()

        return endModalResult
    }
}
