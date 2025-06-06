using Threading, Time

enum PreviewSize
{
    Small
    Medium
    Big
}

struct LibraryWnd
{
    using wnd:     Wnd

    main:          *MainWnd
    view:          *LibraryView
    listCtrl:      *ListCtrl
    editZoomBar:   *Wnd
    zoomBtn:       *IconButton
    previewSize:    PreviewSize
}

struct LibraryItem
{
    fi:          File.FileInfo
    capture:     *Capture
    texture:     Texture
    rect:        Math.Rectangle
    visible:     bool
}

impl LibraryItem
{
    mtd opDrop()
    {
        Memory.delete(capture)
    }
}

struct LibraryView
{
    using wnd:       Wnd
    scrollWnd:       *ScrollWnd
    libWnd:          *LibraryWnd
    mainWnd:         *MainWnd
    items:           Array'LibraryItem
    mutexLoaded:     Sync.Mutex
    thread:          Thread
    hotItem          = Swag.U32.Max
    focusItem        = Swag.U32.Max
    selModel:        SelModel
    filterYear:      u64
    filterMonth:     u64
    filterDay:       u64
}

impl IWnd for LibraryView
{
    mtd impl onMouseEvent(evt: *MouseEvent)
    {
        if evt.kind == .MouseLeave
        {
            hotItem = Swag.U32.Max
            invalidate()
            return
        }

        if evt.kind == .MouseWheel:
            evt.accepted = false

        var pos = surfaceToLocal(evt.surfacePos)
        pos += scrollWnd.getScrollPos()

        // Hit test
        var ht = Swag.U32.Max
        foreach it, idx in items
        {
            if it.rect.contains(pos)
            {
                ht = cast(u32) idx
                break
            }
        }

        if ht != hotItem
        {
            invalidate()
            hotItem = ht
        }

        let shiftPressed = Input.Keyboard.isKeyPressed(.Shift) or Input.Keyboard.isKeyPressed(.RShift)
        let ctrlPressed = Input.Keyboard.isKeyPressed(.Control) or Input.Keyboard.isKeyPressed(.RControl)

        if evt.kind == .MouseReleased and hotItem != Swag.U32.Max and evt.button == .Left
        {
            if !ctrlPressed and !shiftPressed:
                selModel.unselectAll()

            if shiftPressed and focusItem < items.count:
                selModel.select(hotItem, focusItem, true)
            elif ctrlPressed and selModel.isSelected(hotItem):
                selModel.select(hotItem, false)
            else:
                selModel.select(hotItem, true)

            focusItem = hotItem
            invalidate()
        }

        if evt.kind == .MouseDoubleClick and focusItem < items.count and evt.button == .Left
        {
            let item = &items[focusItem]
            catch File.touch(item.fi.fullname)
            mainWnd.recentWnd.addRecent(item.fi.fullname, item.capture.preview, front: true, sel: true, linked: item.capture.linkedImage)
            mainWnd.recentWnd.setScrollPos(0, 0)
            mainWnd.swapLibraryMode()
            mainWnd.updateState()
        }

        if evt.kind == .MousePressed and evt.button == .Right and focusItem < items.count
        {
            with let menu = MenuCtrl.createPopup(self)
            {
                .addItem(CmdIdDeleteCapture)
                .addSeparator()
                .addItem(CmdIdCaptureFileLocation)
                .addItem(CmdIdLinkedFileLocation)
                .addSeparator()
                .addItem(CmdIdFileDetails)
            }

            discard menu.doModal(evt.surfacePos, autoDestroy: true)
        }
    }

    mtd impl onPaintEvent(evt: *PaintEvent)
    {
        let painter    = evt.bc.painter
        var clientRect = getClientRectPadding()

        var x = clientRect.x
        var y = clientRect.y

        var H: f32 = 128.0
        var W: f32 = 0
        switch libWnd.previewSize
        {
        case Small:
            H = 128
            W = H * 4.0 / 3.0
        case Medium:
            H = 256
            W = H * 4.0 / 3.0
        case Big:
            H = 512
            W = H * 4.0 / 3.0
        }

        const Margin = 8.0

        let spos = scrollWnd.getScrollPos()
        foreach &it, i in items
        {
            it.visible = true
            if y + H < spos.y:
                it.visible = false
            if y > spos.y + position.height:
                it.visible = false

            // Retrieve the loaded capture if it exists
            mutexLoaded.lock()
            let capture = it.capture
            mutexLoaded.unlock()

            // We have a capture but not yet a texture. Create it.
            if capture and !it.texture.isValid():
                it.texture = evt.bc.renderer.addImage(capture.preview)

            it.rect = Math.Rectangle{x, y, W, H}
            let selected = selModel.isSelected(cast() i)

            if capture:
                RecentView.IWnd.paintRecent(self, painter, it.texture, it.rect, i == hotItem, selected, capture.linkedUpToDate, capture.linkedImage)
            else:
                RecentView.IWnd.paintRecent(self, painter, it.texture, it.rect, i == hotItem, selected, false, null)

            // Waiting for texture
            if !it.texture.isValid()
            {
                var icon  = getTheme().getIcon64(.HourGlass, 48)
                var color = Color.fromArgb(50, Argb.White)
                icon.paint(painter, it.rect.horzCenter() - icon.sizeX * 0.5, it.rect.vertCenter() - icon.sizeY * 0.5, color)
            }

            // Next item
            x += W + Margin
            if x + W + Margin >= clientRect.right()
            {
                x = clientRect.x
                y += H + Margin
            }
        }

        scrollWnd.setScrollSize(0, y + H + Margin)
    }
}

impl LibraryView
{
    mtd updateRecent(capture: *Capture)
    {
        foreach &it in items
        {
            if capture.file == it.fi.fullname
            {
                if it.capture and it.texture.isValid()
                {
                    getApp().getRenderer().removeTexture(&it.texture)
                    it.texture = getApp().getRenderer().addImage(capture.preview)
                }

                break
            }
        }
    }

    #[Swag.Overload]
    mtd loadOne(it: *LibraryItem)
    {
        let capture = catch Capture.load(it.fi.fullname)
        if !capture:
            return

        capture.backImg.release()
        mutexLoaded.lock()
        it.capture = capture
        mutexLoaded.unlock()
        postInvalidateEvent()
    }

    #[Swag.Overload]
    mtd loadOne(th: Thread)
    {
        var remain = true
        while remain
        {
            var notVis: *LibraryItem

            remain = false
            foreach &it in items
            {
                if th.requestEnd
                {
                    remain = false
                    break
                }

                if it.capture:
                    continue

                remain = true
                if !it.visible
                {
                    notVis = it
                    continue
                }

                loadOne(it)
                break
            }

            if notVis:
                loadOne(notVis)

        }
    }

    mtd fill() throw
    {
        if thread.isValid()
        {
            thread.safeForceEnd()
            thread.wait()
        }

        // Filter
        var res: Array'File.FileInfo
        foreach fi in g_Library.files
        {
            if (filterYear == 0 or fi.lastWriteTime.year == filterYear) and
               (filterMonth == 0 or fi.lastWriteTime.month == filterMonth) and
               (filterDay == 0 or fi.lastWriteTime.day == filterDay)
            {
                res.add(fi)
            }
        }

        res.sort(func(a, b)
        {
            return b.lastWriteTime <=> a.lastWriteTime
        })

        let renderer = getApp().getRenderer()
        foreach &it in items:
            renderer.removeTexture(&it.texture)

        // Add items
        items.clear()
        items.reserve(res.count)
        foreach fi in res
        {
            var item: LibraryItem
            item.fi = fi
            items.add(item)
        }

        hotItem = Swag.U32.Max
        selModel.unselectAll()

        // Launch thread to read all captures
        try
        {
            thread.init(func|self|(th: Thread) { self.loadOne(th); })
            thread.start()
        }

        invalidate()
    }
}

impl LibraryWnd
{
    mtd setPreviewSize(size: PreviewSize)
    {
        previewSize = size
        switch size
        {
        case Small:
            zoomBtn.name.set("small")
        case Medium:
            zoomBtn.name.set("medium")
        case Big:
            zoomBtn.name.set("big")
        }

        invalidate()
    }

    mtd getCapture()->#null *Capture
    {
        if view.focusItem < view.items.count:
            return view.items[view.focusItem].capture
        return null
    }

    mtd requestClose()
    {
        if view.thread.isValid()
        {
            view.thread.safeForceEnd()
            view.thread.wait()
        }
    }

    mtd const hasSelected()->bool
    {
        return !view.selModel.isEmpty()
    }

    mtd removeSelected()
    {
        foreach it, i in view.items
        {
            if view.selModel.isSelected(cast() i)
            {
                g_Library.removeFile(it.fi.fullname)
            }
        }
    }

    mtd fill()
    {
        if listCtrl.getLineCount():
            return
        g_Library.onDeleteFile += func|self|(file)
        {
            foreach it, i in self.view.items
            {
                if it.fi.fullname == file
                {
                    self.view.selModel.select(cast() i, false)
                    self.view.items.removeAtOrdered(i)
                    break
                }
            }
        }

        let line = listCtrl.createLine("All Captures", Icon.from(&main.icons24, 4))
        var item = line.setItem(1, Format.toString("% ", g_Library.files.count))
        item.horzAlign = .Right

        g_Library.files.sort(func(a, b)
        {
            return b.lastWriteTime <=> a.lastWriteTime
        })

        var lastDateTime:  const *DateTime
        var lastLine:      *ListLine
        var lastLineYear:  *ListLine
        var lastLineMonth: *ListLine
        var lastLineDay:   *ListLine

        var icon1 = Icon.from(&main.icons24, 3)

        var cptYear, cptMonth, cptDay = 0

        #[Swag.Mixin]
        func fillLastYear()
        {
            if lastLineYear
            {
                item           = lastLineYear.setItem(1, Format.toString("% ", cptYear))
                item.horzAlign = .Right
                cptYear        = 0
            }
        }

        #[Swag.Mixin]
        func fillLastMonth()
        {
            if lastLineMonth
            {
                item           = lastLineMonth.setItem(1, Format.toString("% ", cptMonth))
                item.horzAlign = .Right
                cptMonth       = 0
            }
        }

        #[Swag.Mixin]
        func fillLastDay()
        {
            if lastLineDay
            {
                item           = lastLineDay.setItem(1, Format.toString("% ", cptDay))
                item.horzAlign = .Right
                cptDay         = 0
            }
        }

        foreach fi in g_Library.files
        {
            if !lastDateTime or (fi.lastWriteTime.year != lastDateTime.year)
            {
                fillLastYear()
                lastLine           = listCtrl.createLine(Format.toString("%", fi.lastWriteTime.year), icon1)
                lastLine.userData0 = cast() cast(u64) fi.lastWriteTime.year
                lastLineYear       = lastLine
            }

            if !lastDateTime or (fi.lastWriteTime.month != lastDateTime.month)
            {
                fillLastMonth()
                lastLine           = listCtrl.createLine(Format.toString("%", DateTime.monthName(fi.lastWriteTime.month)), icon1, lastLineYear)
                lastLine.userData0 = cast() cast(u64) fi.lastWriteTime.year
                lastLine.userData1 = cast() cast(u64) fi.lastWriteTime.month
                lastLineMonth      = lastLine
            }

            if !lastDateTime or (fi.lastWriteTime.day != lastDateTime.day)
            {
                fillLastDay()
                lastLine           = listCtrl.createLine(Format.toString("%, %", DateTime.dayOfWeekName(fi.lastWriteTime.dayOfWeek()), fi.lastWriteTime.day), icon1, lastLineMonth)
                lastLine.userData0 = cast() cast(u64) fi.lastWriteTime.year
                lastLine.userData1 = cast() cast(u64) fi.lastWriteTime.month
                lastLine.userData2 = cast() cast(u64) fi.lastWriteTime.day
                lastLineDay        = lastLine
            }

            lastDateTime = &fi.lastWriteTime
            cptYear, cptMonth, cptDay += 1
        }

        fillLastYear()
        fillLastMonth()
        fillLastDay()

        listCtrl.computeLayout()
        catch view.fill()
    }

    func create(parent: *MainWnd)->*LibraryWnd
    {
        with let res = Wnd.create'LibraryWnd(parent, "captme.librarywnd")
        {
            .main            = parent
            .dockStyle       = .Center
            .backgroundStyle = .View
        }

        with let rightBar = Wnd.create'Wnd(res, {0, 0, 300, 0})
        {
            .dockStyle = .Right
        }

        with let scrollWnd = Wnd.create'ScrollWnd(res)
        {
            .dockStyle = .Center
        }

        with let view = Wnd.create'LibraryView(scrollWnd)
        {
            .libWnd    = res
            .padding   = 8
            .scrollWnd = scrollWnd
            .mainWnd   = parent
        }

        scrollWnd.setView(view)
        res.view = view

        with let list = ListCtrl.createMultiColumns(rightBar)
        {
            .addColumn("name", 200)
            .addColumn("count", 70)
            .listView.style.addStyleSheetColors("listItem_SelectedBk $listItem_FocusSelectedBk")
            .lineHeight = 30
            .listFlags |= .NoMouseEmptySel
            .scrollWnd.scrollWndFlags |= .SmallBar | .ForceVertical | .TransparentBar
            .header.hide()
            .backgroundStyle = .Window
            .listFlags |= .ForceMarginExpandMark
            .dockStyle = .Center
            .sigSelChanged += func|view|(list)
            {
                var sel = list.getSelectedLine()
                if !sel:
                    return
                view.filterYear  = cast(u64) sel.userData0
                view.filterMonth = cast(u64) sel.userData1
                view.filterDay   = cast(u64) sel.userData2
                catch view.fill()
            }
        }
        res.listCtrl = list

        // Bottom bar
        with res.editZoomBar = StackLayoutCtrl.create(res, .HorzCenter, {0, 0, 300, 32})
        {
            .dockStyle       = .Bottom
            .backgroundStyle = .Window
        }

        var ic = res.getTheme().getIcon24(.Search, 20)
        with res.zoomBtn = IconButton.create(res.editZoomBar, ic, {0, 0, 120, 32})
        {
            .iconBtnFlags.add(.Center)
            .textPos  = .Right
            .arrowPos = .RightUp
            .sigPressed += func|res|(btn)
            {
                var pos  = btn.getSurfaceRect()
                with var menu = MenuCtrl.createPopup(res)
                {
                    .menuCtrlFlags |= .NoKeyShortcuts
                    .addItem(CmdIdLibSmall)
                    .addItem(CmdIdLibMedium)
                    .addItem(CmdIdLibBig)
                }
                discard menu.doModal({pos.horzCenter(), pos.y}, .AnchorBottomCenter, autoDestroy: true)
            }
        }

        res.setPreviewSize(.Small)
        return res
    }
}
