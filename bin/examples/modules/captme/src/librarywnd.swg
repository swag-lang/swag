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
}

struct LibraryView
{
    using wnd: Wnd
    scrollWnd:      *ScrollWnd
    items:          Array'LibraryItem
    mutexLoaded:    Sync.Mutex
    loaded:         Array'*Capture
    thread:         Thread
}

impl IWnd for LibraryView
{
    mtd onPaintEvent(evt: *PaintEvent)
    {
        painter := evt.bc.painter
        clientRect := getClientRectPadding()

        x := clientRect.x
        y := clientRect.y

        // Get loaded captures from thread
        mutexLoaded.lock()
        copyLoaded := loaded
        loaded.clear()
        mutexLoaded.unlock()

        const W = 128.0
        const H = 128.0
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

            rect := Math.Rectangle{x, y, W, H}
            if it.texture.isValid()
                painter.drawTexture(rect, it.texture)
            else if it.capture
                painter.fillRect(x, y, W, H, Argb.Red)
            else
                painter.fillRect(x, y, W, H, Argb.White)

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
        Debug.assert(!thread.isValid())

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

        // Launch thread to read all captures
        try
        {
            thread.init(@|self|(th: Thread) {
                visit &it: self.items
                {
                    self.mutexLoaded.lock()
                    capture := catch Capture.load(it.fi.fullname)
                    if capture
                    {
                        capture.backImg.release()
                        self.loaded.add(capture)
                    }
                    self.mutexLoaded.unlock()

                    self.postInvalidateEvent()

                    sleep(1's)
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

    func create(parent: *Wnd)->*LibraryWnd
    {
        res := Wnd.create'LibraryWnd(parent, "captme.librarywnd")
        res.dockStyle = .Center
        res.backgroundStyle = .View

        scrollWnd := Wnd.create'ScrollWnd(res)
        scrollWnd.dockStyle = .Center

        view := Wnd.create'LibraryView(scrollWnd)
        view.padding = 8
        view.scrollWnd = scrollWnd
        scrollWnd.setView(view)
        catch view.fill()

        res.view = view

        return res
    }
}