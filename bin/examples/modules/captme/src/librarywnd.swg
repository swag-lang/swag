using Threading, Time

struct LibraryWnd
{
    using wnd: Wnd

    main:       *MainWnd
    view:       *LibraryView
    listCtrl:   *ListCtrl
}

struct LibraryItem
{
    fi:         File.FileInfo
    capture:    *Capture
    texture:    Texture
    rect:       Math.Rectangle
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
    using wnd: Wnd
    scrollWnd:      *ScrollWnd
    libWnd:         *LibraryWnd
    mainWnd:        *MainWnd
    items:          Array'LibraryItem
    mutexLoaded:    Sync.Mutex
    loaded:         Array'*Capture
    thread:         Thread
    hotItem         = Swag.U64.Max
    selectedItem    = Swag.U64.Max
    filterYear:     u64
    filterMonth:    u64
    filterDay:      u64
}

impl IWnd for LibraryView
{
    mtd onMouseEvent(evt: *MouseEvent)
    {
        if evt.kind == .MouseWheel
            evt.accepted = false

        pos := surfaceToLocal(evt.surfacePos)
        pos += scrollWnd.getScrollPos()

        // Hit test
        ht := Swag.U64.Max
        visit it, idx: items
        {
            if it.rect.contains(pos)
            {
                ht = idx
                break
            }
        }

        if ht != hotItem
        {
            invalidate()
            hotItem = ht
        }

        if evt.kind == .MousePressed and hotItem != Swag.U64.Max and evt.button == .Left
        {
            selectedItem = hotItem
            invalidate()
        }

        if evt.kind == .MouseDoubleClick and selectedItem != Swag.U64.Max and evt.button == .Left
        {
            item := &items[selectedItem]
            catch File.touch(item.fi.fullname)
            mainWnd.recentWnd.addRecent(item.fi.fullname, item.capture.preview, front: true, sel: true)
            mainWnd.swapLibraryMode()
            mainWnd.updateState()
        }
    }

    mtd onPaintEvent(evt: *PaintEvent)
    {
        painter    := evt.bc.painter
        clientRect := getClientRectPadding()

        x := clientRect.x
        y := clientRect.y

        // Get loaded captures from thread
        mutexLoaded.lock()
        copyLoaded := loaded
        loaded.clear()
        mutexLoaded.unlock()

        const H = 128.0
        const W = H * 4.0/3.0
        const Margin = 8.0

        visit &it, i: items
        {
            // Retrieve the loaded capture if it exists
            if !it.capture
            {
                visit l, idx: copyLoaded
                {
                    if l.file == it.fi.fullname
                    {
                        it.capture = l
                        break
                    }
                }
            }

            // We have a capture but not yet a texture. Create it.
            if it.capture and !it.texture.isValid()
                it.texture = evt.bc.renderer.addImage(it.capture.preview)

            it.rect = Math.Rectangle{x, y, W, H}
            RecentView.IWnd.paintRecent(self, painter, it.texture, it.rect, i == hotItem, i == selectedItem)

            // Waiting for texture
            if !it.texture.isValid()
            {
                icon := getTheme().getIcon64(.HourGlass, 48)
                color := Color.fromArgb(50, Argb.White)
                icon.paint(painter, it.rect.horzCenter() - icon.sizeX*0.5, it.rect.vertCenter() - icon.sizeY*0.5, color)
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
    mtd opDrop()
    {
        visit c: loaded
            Memory.delete(c)
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
        visit fi: g_Library.files
        {
            if (filterYear == 0 or fi.lastWriteTime.year == filterYear) and
               (filterMonth == 0 or fi.lastWriteTime.month == filterMonth) and
               (filterDay == 0 or fi.lastWriteTime.day == filterDay)
            {
                res.add(fi)
            }
        }

        res.sort(func(a, b) {
            return b.lastWriteTime <=> a.lastWriteTime
        })

        renderer := getApp().getRenderer()
        visit &it: items
            renderer.removeTexture(&it.texture)

        // Add items
        items.clear()
        items.reserve(res.count)
        visit fi: res
        {
            var item: LibraryItem
            item.fi = fi
            items.add(item)
        }

        hotItem = Swag.U64.Max
        selectedItem = Swag.U64.Max

        // Launch thread to read all captures
        try
        {
            thread.init(closure|self|(th: Thread) {
                visit &it: self.items
                {
                    capture := catch Capture.load(it.fi.fullname)
                    if capture
                    {
                        capture.backImg.release()
                        self.mutexLoaded.lock()
                        self.loaded.add(capture)
                        self.mutexLoaded.unlock()
                        self.postInvalidateEvent()
                    }

                    if th.requestEnd
                        break
                }
            })
            thread.start()
        }

        invalidate()
    }
}

impl LibraryWnd
{
    mtd requestClose()
    {
        if view.thread.isValid()
        {
            view.thread.safeForceEnd()
            view.thread.wait()
        }
    }

    mtd fill()
    {
        if listCtrl.getLineCount()
            return

        line := listCtrl.createLine("All Captures", Icon.from(&main.icons24, 4))
        item := line.setItem(1, Format.toString("% ", g_Library.files.count))
        item.horzAlign = .Right

        g_Library.files.sort(func(a, b) {
            return b.lastWriteTime <=> a.lastWriteTime
        })

        var lastDateTime:   const *DateTime
        var lastLine:       *ListLine
        var lastLineYear:   *ListLine
        var lastLineMonth:  *ListLine
        var lastLineDay:    *ListLine

        icon1 := Icon.from(&main.icons24, 3)

        cptYear, cptMonth, cptDay := 0

        #[Swag.Mixin]
        func fillLastYear()
        {
            if lastLineYear
            {
                item = lastLineYear.setItem(1, Format.toString("% ", cptYear))
                item.horzAlign = .Right
                cptYear = 0
            }
        }

        #[Swag.Mixin]
        func fillLastMonth()
        {
            if lastLineMonth
            {
                item = lastLineMonth.setItem(1, Format.toString("% ", cptMonth))
                item.horzAlign = .Right
                cptMonth = 0
            }
        }

        #[Swag.Mixin]
        func fillLastDay()
        {
            if lastLineDay
            {
                item = lastLineDay.setItem(1, Format.toString("% ", cptDay))
                item.horzAlign = .Right
                cptDay = 0
            }
        }

        visit fi: g_Library.files
        {
            if !lastDateTime or (fi.lastWriteTime.year != lastDateTime.year)
            {
                fillLastYear()
                lastLine = listCtrl.createLine(Format.toString("%", fi.lastWriteTime.year), icon1)
                lastLine.userData0 = acast cast(u64) fi.lastWriteTime.year
                lastLineYear = lastLine
            }

            if !lastDateTime or (fi.lastWriteTime.month != lastDateTime.month)
            {
                fillLastMonth()
                lastLine = listCtrl.createLine(Format.toString("%", DateTime.monthName(fi.lastWriteTime.month)), icon1, lastLineYear)
                lastLine.userData0 = acast cast(u64) fi.lastWriteTime.year
                lastLine.userData1 = acast cast(u64) fi.lastWriteTime.month
                lastLineMonth = lastLine
            }

            if !lastDateTime or (fi.lastWriteTime.day != lastDateTime.day)
            {
                fillLastDay()
                lastLine = listCtrl.createLine(Format.toString("%, %", DateTime.dayOfWeekName(fi.lastWriteTime.dayOfWeek()), fi.lastWriteTime.day), icon1, lastLineMonth)
                lastLine.userData0 = acast cast(u64) fi.lastWriteTime.year
                lastLine.userData1 = acast cast(u64) fi.lastWriteTime.month
                lastLine.userData2 = acast cast(u64) fi.lastWriteTime.day
                lastLineDay = lastLine
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
        with res := Wnd.create'LibraryWnd(parent, "captme.librarywnd")
        {
            .main = parent
            .dockStyle = .Center
            .backgroundStyle = .View
        }

        with rightBar := Wnd.create'Wnd(res, {0, 0, 300, 0})
        {
            .dockStyle = .Right
        }

        with scrollWnd := Wnd.create'ScrollWnd(res)
        {
            .dockStyle = .Center
        }

        with view := Wnd.create'LibraryView(scrollWnd)
        {
            .libWnd = res
            .padding = 8
            .scrollWnd = scrollWnd
            .mainWnd = parent
        }

        scrollWnd.setView(view)
        res.view = view

        with list := ListCtrl.createMultiColumns(rightBar)
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
            .sigSelChanged += closure|view|(list) {
                sel := list.getSelectedLine()
                if !sel return
                view.filterYear = cast(u64) sel.userData0
                view.filterMonth = cast(u64) sel.userData1
                view.filterDay = cast(u64) sel.userData2
                catch view.fill()
            }
        }
        res.listCtrl = list

        return res
    }
}