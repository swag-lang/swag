using Threading, Time

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
    items:           ArrayPtr'LibraryItem
    mutexLoaded:     Sync.Mutex
    thread:          Thread
    hotItem          = Swag.U32.Max
    focusItem        = Swag.U32.Max
    selModel:        SelModel
    filterYear:      u64
    filterMonth:     u64
    filterDay:       u64
    imgSpin:         *ImageRect
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
        let ctrlPressed  = Input.Keyboard.isKeyPressed(.Control) or Input.Keyboard.isKeyPressed(.RControl)

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
            let item = items[focusItem]
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
        if !g_UpdatingLib:
            imgSpin.hide()

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
        foreach it, i in items
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

            if capture
            {
                if !capture.linkedImage.isEmpty() and !libWnd.showLinkedImage:
                    continue
                if capture.linkedImage.isEmpty() and !libWnd.showUnLinkedImage:
                    continue
                if capture.linkedUpToDate and !capture.linkedImage.isEmpty() and !libWnd.showUpToDateLinkedImage:
                    continue
                if !capture.linkedUpToDate and !capture.linkedImage.isEmpty() and !libWnd.showUnSavedLinkedImage:
                    continue
            }

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
        foreach it in items
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
        let capture = catch Capture.load(it.fi.fullname, preview: true)
        if !capture:
            return

        capture.haar = ImageStatistics.extractHaarFeatures(capture.preview, 64)

        mutexLoaded.lock()
        it.capture = capture
        mutexLoaded.unlock()
    }

    #[Swag.Overload]
    mtd loadOne(th: Thread)
    {
        // Work on a copy, as 'items' can be changed by sorting during loading
        var copy: Array'*LibraryItem
        foreach it in items:
            copy.add(it)

        for
        {
            var visItem, notVisItem: *LibraryItem

            foreach it in copy
            {
                if th.requestEnd:
                    break
                if it.capture:
                    continue

                if it.visible
                {
                    visItem = it
                    break
                }

                if !notVisItem:
                    notVisItem = it
            }

            if visItem
            {
                loadOne(visItem)
                postInvalidateEvent()
            }
            elif notVisItem
            {
                loadOne(notVisItem)
            }
            else:
                break
        }

        postInvalidateEvent()
        g_UpdatingLib = false
    }

    mtd sortSimilar()
    {
        if items.isEmpty():
            return

        var remaining: Array'*LibraryItem
        foreach it in items:
            remaining.add(it)
        if remaining.isEmpty():
            return

        var nitems: Array'*LibraryItem

        var toCompare: *LibraryItem
        while !remaining.isEmpty()
        {
            if !toCompare or !toCompare.capture
            {
                toCompare = remaining.front()
                remaining.removeAt(0)
                nitems.add(toCompare)
                continue
            }

            if remaining.isEmpty():
                continue

            var best:      f32
            var bestI:     u64
            var foundMatch = false
            foreach it in remaining where it.capture and toCompare.capture.haar.size == it.capture.haar.size
            {
                let similarity = ImageStatistics.quickCompareHaarCoefficients(toCompare.capture.haar, it.capture.haar)
                if similarity > best
                {
                    best       = similarity
                    bestI      = @index
                    foundMatch = true
                }
            }

            if foundMatch
            {
                toCompare = remaining[bestI]
                nitems.add(toCompare)
                remaining.removeAt(bestI)
            }
        }

        mutexLoaded.lock()
        foreach it, i in nitems:
            items[i] = nitems[i]
        mutexLoaded.unlock()
    }

    mtd sort()
    {
        if items.isEmpty():
            return

        Cursor.wait()

        if libWnd.sortMethod == .Similar
        {
            sortSimilar()
        }
        else
        {
            items.sort(func|libWnd|(a, b)
            {
                var value: s32

                switch libWnd.sortMethod
                {
                case Recent:
                    value = b.fi.lastWriteTime <=> a.fi.lastWriteTime
                case FileSize:
                    value = b.fi.size <=> a.fi.size
                }

                if libWnd.invertSort:
                    return -value
                return value
            })
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

        let renderer = getApp().getRenderer()
        foreach it in items:
            renderer.removeTexture(&it.texture)

        // Add items
        items.clear()
        items.reserve(res.count)
        foreach fi in res
        {
            let ptr = items.addNewPtr()
            ptr.fi = fi
        }

        sort()

        hotItem = Swag.U32.Max
        selModel.unselectAll()

        g_UpdatingLib = true
        imgSpin.show()

        // Launch thread to read all captures
        try
        {
            thread.init(func|self|(th: Thread) { self.loadOne(th); })
            thread.start()
        }

        invalidate()
    }
}
