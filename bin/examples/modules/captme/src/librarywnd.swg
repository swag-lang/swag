using Threading

struct LibraryWnd
{
    using wnd: Wnd
    view: *LibraryView
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
    mainWnd:        *MainWnd
    items:          Array'LibraryItem
    mutexLoaded:    Sync.Mutex
    loaded:         Array'*Capture
    thread:         Thread
    hotItem         = Swag.UInt.Max
    selectedItem    = Swag.UInt.Max
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
            mainWnd.recentWnd.addRecent(items[selectedItem].fi.fullname, items[selectedItem].capture.preview, true, true)
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
    mtd fill(filter: string = null) throw
    {
        if thread.isValid()
        {
            thread.safeForceEnd()
            thread.wait()
        }

        res := Directory.enumerateFiles(Library.getFolder(), "*.captme")

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
        catch view.fill()
    }

    func create(parent: *MainWnd)->*LibraryWnd
    {
        res := Wnd.create'LibraryWnd(parent, "captme.librarywnd")
        res.dockStyle = .Center
        res.backgroundStyle = .View

        scrollWnd := Wnd.create'ScrollWnd(res)
        scrollWnd.dockStyle = .Center

        view := Wnd.create'LibraryView(scrollWnd)
        view.padding = 8
        view.scrollWnd = scrollWnd
        view.mainWnd = parent
        scrollWnd.setView(view)
        res.view = view

        return res
    }
}