using Threading, Time

struct LibraryWnd
{
    using wnd: Wnd

    files:      Array'File.FileInfo
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
    hotItem         = Swag.UInt.Max
    selectedItem    = Swag.UInt.Max
    filterYear:     uint
    filterMonth:    uint
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
        ht := Swag.UInt.Max
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

        if evt.kind == .MousePressed and hotItem != Swag.UInt.Max and evt.button == .Left
        {
            selectedItem = hotItem
            invalidate()
        }

        if evt.kind == .MouseDoubleClick and selectedItem != Swag.UInt.Max and evt.button == .Left
        {
            item := items[selectedItem]
            catch File.touch(item.fi.fullname)
            mainWnd.recentWnd.addRecent(item.fi.fullname, item.capture.preview, sel: true)
            mainWnd.swapLibraryMode()
        }
    }

    mtd onPaintEvent(evt: *PaintEvent)
    {
        painter    := evt.bc.painter
        colors     := getThemeColors()
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
            // Retreive the loaded capture if it exists
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
    mtd fill() throw
    {
        if thread.isValid()
        {
            thread.safeForceEnd()
            thread.wait()
        }

        // Filter
        var res: Array'File.FileInfo
        visit fi: libWnd.files
        {
            if (filterYear == 0 or fi.lastWriteTime.year == filterYear) and
               (filterMonth == 0 or fi.lastWriteTime.month == filterMonth)
            {
                res.add(dref fi)
            }
        }

        res.sort(@(a, b) {
            return b.lastWriteTime <=> a.lastWriteTime
        })

        renderer := getApp().getRenderer()
        visit &it: items
        {
            renderer.removeTexture(&it.texture)
            Memory.delete(it.capture)
        }

        // Add items
        items.clear()
        items.reserve(res.count)
        visit fi: res
        {
            var item: LibraryItem
            item.fi = dref fi
            items.add(item)
        }

        hotItem = Swag.UInt.Max
        selectedItem = Swag.UInt.Max

        // Launch thread to read all captures
        try
        {
            thread.init(@|self|(th: Thread) {
                visit &it: self.items
                {
                    capture := catch Capture.load(it.fi.fullname)
                    if capture
                    {
                        self.mutexLoaded.lock()
                        capture.backImg.release()
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
        if files.count
            return

        files = catch Directory.enumerateFiles(Library.getFolder(), "*.captme")

        listCtrl.clear()

        line := listCtrl.createLine("All Captures", Icon.from(&main.icons24, 4))
        item := line.setItem(1, Format.toString("% ", files.count))
        item.horzAlign = .Right

        files.sort(@(a, b) {
            return b.lastWriteTime <=> a.lastWriteTime
        })

        var lastDateTime:   const *DateTime
        var lastLine:       *ListLine
        var lastLineYear:   *ListLine
        var lastLineMonth:  *ListLine

        icon1 := Icon.from(&main.icons24, 3)

        cptYear  := 0
        cptMonth := 0

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

        visit fi: files
        {
            if !lastDateTime or (fi.lastWriteTime.year != lastDateTime.year)
            {
                fillLastYear()
                lastLine = listCtrl.createLine(Format.toString("%", fi.lastWriteTime.year), icon1)
                lastLine.userData0 = acast cast(uint) fi.lastWriteTime.year
                lastLineYear = lastLine
            }

            if !lastDateTime or (fi.lastWriteTime.month != lastDateTime.month)
            {
                fillLastMonth()
                lastLine = listCtrl.createLine(Format.toString("%", DateTime.monthName(fi.lastWriteTime.month)), icon1, lastLineYear)
                lastLine.userData0 = acast cast(uint) fi.lastWriteTime.year
                lastLine.userData1 = acast cast(uint) fi.lastWriteTime.month
                lastLineMonth = lastLine
            }

            lastDateTime = &fi.lastWriteTime
            cptYear += 1
            cptMonth += 1
        }

        fillLastYear()
        fillLastMonth()

        listCtrl.computeLayout()
        catch view.fill()
    }

    func create(parent: *MainWnd)->*LibraryWnd
    {
        res := Wnd.create'LibraryWnd(parent, "captme.librarywnd")
        res.main = parent
        res.dockStyle = .Center
        res.backgroundStyle = .View

        rightBar := Wnd.create'Wnd(res, @{0, 0, 300, 0})
        rightBar.dockStyle = .Right

        scrollWnd := Wnd.create'ScrollWnd(res)
        scrollWnd.dockStyle = .Center

        view := Wnd.create'LibraryView(scrollWnd)
        view.libWnd = res
        view.padding = 8
        view.scrollWnd = scrollWnd
        view.mainWnd = parent
        scrollWnd.setView(view)
        res.view = view

        list := ListCtrl.createMultiColumns(rightBar)
        list.addColumn("name", 200)
        list.addColumn("count", 70)
        list.listView.style.addStyleSheetColors("listItem_SelectedBk $listItem_FocusSelectedBk")
        list.lineHeight = 30
        list.listFlags |= .NoMouseEmptySel
        list.scrollWnd.scrollWndFlags |= .SmallBar | .ForceVertical | .TransparentBar
        list.header.hide()
        list.backgroundStyle = .Window
        list.listFlags |= .ForceMarginExpandMark
        list.dockStyle = .Center
        list.sigSelChanged += @|view|(list) {
            sel := list.getSelectedLine()
            if !sel return
            view.filterYear = cast(uint) sel.userData0
            view.filterMonth = cast(uint) sel.userData1
            catch view.fill()
        }
        res.listCtrl = list

        return res
    }
}