using Threading, Time

private var g_UpdatingLib: bool
private var g_DirtyList: bool = true
private const ItemMargin = 8.0

struct LibraryItem
{
    fi:           File.FileInfo
    capture:      *Capture
    texture:      Texture
    rect:         Math.Rectangle
    visible:      bool
    filtered:     bool
}

impl LibraryItem
{
    mtd opDrop()
    {
        Memory.delete(me.capture)
    }
}

struct LibraryView
{
    using wnd:       Wnd
    scrollWnd:       *ScrollWnd
    libWnd:          *LibraryWnd
    mainWnd:         *MainWnd
    allItems:        ArrayPtr'LibraryItem
    items:           Array'*LibraryItem
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
            me.hotItem = Swag.U32.Max
            me.invalidate()
            return
        }

        if evt.kind == .MouseWheel:
            evt.accepted = false

        var pos = me.surfaceToLocal(evt.surfacePos)
        pos += me.scrollWnd.getScrollPos()

        // Hit test
        var ht = Swag.U32.Max
        foreach it, idx in me.items
        {
            if it.rect.contains(pos)
            {
                ht = cast(u32) idx
                break
            }
        }

        if ht != me.hotItem
        {
            me.invalidate()
            me.hotItem = ht
        }

        let shiftPressed = Input.Keyboard.isKeyPressed(.Shift) or Input.Keyboard.isKeyPressed(.RShift)
        let ctrlPressed  = Input.Keyboard.isKeyPressed(.Control) or Input.Keyboard.isKeyPressed(.RControl)

        if evt.kind == .MouseReleased and me.hotItem != Swag.U32.Max and evt.button == .Left
        {
            if !ctrlPressed and !shiftPressed:
                me.selModel.unselectAll()

            if shiftPressed and me.focusItem < me.items.count:
                me.selModel.select(me.hotItem, me.focusItem, true)
            elif ctrlPressed and me.selModel.isSelected(me.hotItem):
                me.selModel.select(me.hotItem, false)
            else:
                me.selModel.select(me.hotItem, true)

            me.focusItem = me.hotItem
            me.invalidate()
            me.setFocus()
        }

        if evt.kind == .MouseDoubleClick and me.focusItem < me.items.count and evt.button == .Left
        {
            let item = me.items[me.focusItem]
            catch File.touch(item.fi.fullname)
            me.mainWnd.recentWnd.addRecent(item.fi.fullname, item.capture.preview, front: true, sel: true, linked: item.capture.linkedImage)
            me.mainWnd.recentWnd.setScrollPos(0, 0)
            me.mainWnd.swapLibraryMode()
            me.mainWnd.updateState()
        }

        if evt.kind == .MousePressed and evt.button == .Right and me.focusItem < me.items.count
        {
            with let menu = MenuCtrl.createPopup(me)
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
            me.imgSpin.hide()

        let painter    = evt.bc.painter
        var clientRect = me.getClientRectPadding()

        var x = clientRect.x
        var y = clientRect.y

        let (w, h) = me.getPreviewSize()
        if g_DirtyList
        {
            g_DirtyList = false
            me.filter()
        }

        let spos = me.scrollWnd.getScrollPos()
        foreach it, i in me.items
        {
            it.visible = true
            if y + h < spos.y:
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

            it.rect = Math.Rectangle{x, y, w, h}
            let selected = selModel.isSelected(cast() i)

            if capture:
                RecentView.IWnd.paintRecent(me, painter, it.texture, it.rect, i == hotItem, selected, capture.linkedUpToDate, capture.linkedImage)
            else:
                RecentView.IWnd.paintRecent(me, painter, it.texture, it.rect, i == hotItem, selected, false, null)

            // Waiting for texture
            if !it.texture.isValid()
            {
                var icon  = getTheme().getIcon64(.HourGlass, 48)
                var color = Color.fromArgb(50, Argb.White)
                icon.paint(painter, it.rect.horzCenter() - icon.sizeX * 0.5, it.rect.vertCenter() - icon.sizeY * 0.5, color)
            }

            // Next item
            x += w + ItemMargin
            if x + w + ItemMargin >= clientRect.right()
            {
                x = clientRect.x
                y += h + ItemMargin
            }
        }

        me.scrollWnd.setScrollSize(0, y + h + ItemMargin)
    }
}

impl LibraryView
{
    mtd const numImagesPerLine()->u32
    {
        let (w, ?) = me.getPreviewSize()
        var clientRect = me.getClientRectPadding()
        let n          = clientRect.width / (w + ItemMargin)
        return cast(u32) n
    }

    mtd const getPreviewSize()->{ w: f32, h: f32 }
    {
        var H: f32 = 128.0
        var W: f32 = 0
        switch me.libWnd.previewSize
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

        return {W, H}
    }

    mtd updateRecent(capture: *Capture)
    {
        foreach it in me.items
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

    mtd filter()
    {
        me.items.clear()
        foreach it in me.allItems
        {
            it.filtered = true

            mutexLoaded.lock()
            let capture = it.capture
            mutexLoaded.unlock()

            if !capture:
                continue
            if !capture.linkedImage.isEmpty() and !libWnd.showLinkedImage:
                continue
            if capture.linkedImage.isEmpty() and !libWnd.showUnLinkedImage:
                continue
            if capture.linkedUpToDate and !capture.linkedImage.isEmpty() and !libWnd.showUpToDateLinkedImage:
                continue
            if !capture.linkedUpToDate and !capture.linkedImage.isEmpty() and !libWnd.showUnSavedLinkedImage:
                continue

            it.filtered = false
            items.add(it)
        }

        me.invalidate()
    }

    #[Swag.Overload]
    mtd loadOne(it: *LibraryItem)
    {
        let capture = catch Capture.load(it.fi.fullname, preview: true)
        if !capture:
            return

        capture.haar = ImageStatistics.extractHaarFeatures(capture.preview, 64)
        g_DirtyList  = true

        me.mutexLoaded.lock()
        it.capture = capture
        me.mutexLoaded.unlock()
    }

    #[Swag.Overload]
    mtd loadFiles(th: Thread)
    {
        // Work on a copy, as 'items' can be changed by sorting during loading
        var copy: Array'*LibraryItem
        foreach it in me.allItems:
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
                me.loadOne(visItem)
                me.postInvalidateEvent()
            }
            elif notVisItem
            {
                me.loadOne(notVisItem)
            }
            else:
                break
        }

        me.postInvalidateEvent()
        g_UpdatingLib = false
    }

    mtd sortSimilar()
    {
        if me.items.isEmpty():
            return

        var remaining: Array'*LibraryItem
        foreach it in me.items:
            remaining.add(it)
        if remaining.isEmpty():
            return

        var nitems: Array'*LibraryItem

        var toCompare: *LibraryItem
        while !remaining.isEmpty()
        {
            if !toCompare or !toCompare.capture or toCompare.filtered
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

        me.mutexLoaded.lock()
        foreach it, i in nitems:
            items[i] = nitems[i]
        me.mutexLoaded.unlock()
    }

    mtd sortFiles()
    {
        me.items.sort(func|me.libWnd|(a, b)
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

    mtd sort()
    {
        if me.items.isEmpty():
            return

        Cursor.wait()

        if me.libWnd.sortMethod == .Similar:
            me.sortSimilar()
        else:
            me.sortFiles()

        me.invalidate()
    }

    mtd fill() throw
    {
        if me.thread.isValid()
        {
            me.thread.safeForceEnd()
            me.thread.wait()
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

        let renderer = me.getApp().getRenderer()
        foreach it in me.items:
            renderer.removeTexture(&it.texture)

        // Add items
        me.allItems.clear()
        me.allItems.reserve(res.count)
        foreach fi in res
        {
            let ptr = allItems.addNewPtr()
            ptr.fi = fi
        }

        me.filter()
        me.sort()

        me.hotItem = Swag.U32.Max
        me.clearSelection()

        g_UpdatingLib = true
        me.imgSpin.show()

        // Launch thread to read all captures
        try
        {
            me.thread.init(mtd||(th: Thread) { me.loadFiles(th); })
            me.thread.start()
        }

        me.invalidate()
    }

    mtd clearSelection()
    {
        me.focusItem = Swag.U32.Max
        me.selModel.unselectAll()
    }

    mtd setFocusItem(focus: u32)
    {
        me.selModel.unselectAll()
        me.focusItem = focus
        me.selModel.select(me.focusItem, me.focusItem, true)
        me.invalidate()
    }

    mtd scrollToFocus()
    {
        if me.focusItem == Swag.U32.Max:
            return
        if me.items[me.focusItem].visible:
            return
        me.scrollWnd.setScrollPos(0, me.items[me.focusItem].rect.y - 4)
    }
}
