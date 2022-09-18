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
    items:  Array'LibraryItem
    thread: Thread
}

impl IWnd for LibraryView
{
    mtd onPaintEvent(evt: *PaintEvent)
    {
        painter := evt.bc.painter
        clientRect := getClientRectPadding()

        x := clientRect.x
        y := clientRect.y

        const W = 128.0
        const H = 128.0
        const Margin = 8.0

        visit &fi, i: items
        {
            if fi.capture and !fi.texture.isValid()
            {
                fi.texture = evt.bc.renderer.addImage(fi.capture.preview)
            }

            if fi.texture.isValid()
            {
                rect := Math.Rectangle{x, y, W, H}
                painter.drawTexture(rect, fi.texture)
            }
            else
            {
                painter.fillRect(x, y, W, H, Argb.Red)
            }

            x += W + Margin
            if x >= clientRect.right()
            {
                x = clientRect.x
                y += H + Margin
            }
        }
    }
}

impl LibraryView
{
    mtd fill(filter: string = null) throw
    {
        Debug.assert(!thread.isValid())

        renderer := getApp().getRenderer()
        visit &it: items
        {
            Memory.delete(it.capture)
            texture := &it.texture
            renderer.removeTexture(texture)
        }

        res := Directory.enumerateFiles(Library.getFolder(), "*.captme")

        items.clear()
        items.reserve(res.count)
        visit fi: res
        {
            var item: LibraryItem
            item.fi = dref fi
            items.add(item)
        }

        try
        {
            thread.init(@|self|(th: Thread) {
                visit &it: self.items
                {
                    it.capture = catch Capture.load(it.fi.fullname)
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
            view.thread.wait()
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
        scrollWnd.setView(view)
        catch view.fill()

        res.view = view

        return res
    }
}