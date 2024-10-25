const MaxRecent = 15

const RecentPadding = 8.0

struct RecentItem
{
    file:        String
    linked:      String
    preview:     Texture
    rect:        Math.Rectangle
    zoom:        f32
}

struct RecentView
{
    using wnd:     Wnd

    main:          *MainWnd
    recentWnd:     *RecentWnd
    items:         ArrayPtr'RecentItem
    selected:      u64 = Swag.U64.Max
    hotItem:       u64 = Swag.U64.Max
}

impl RecentView
{
    mtd select(idx: u64, force = false)
    {
        if selected == idx and !force:
            return

        if idx >= items.count
        {
            selected = Swag.U64.Max
            return
        }

        let view = main.editWnd.editView

        // Save current zoom
        if selected < items.count:
            items[selected].zoom = view.zoom

        // Select new recent item
        selected = idx
        let item    = items[idx]
        let capture = catch Capture.load(item.file)
        if capture
        {
            if main.getCapture():
                catch main.getCapture().save()
            view.setCapture(capture)
            if item.zoom == 0:
                view.zoomToFit(false)
            else:
                view.setZoom(item.zoom)
            invalidate()
        }
    }
}

impl IWnd for RecentView
{
    mtd impl onMouseEvent(evt: *MouseEvent)
    {
        if evt.kind == .MouseWheel:
            evt.accepted = false

        var pos = surfaceToLocal(evt.surfacePos)
        pos += recentWnd.getScrollPos()

        // Hit test
        var ht = Swag.U64.Max
        foreach i, idx in items
        {
            if i.rect.contains(pos)
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

        if evt.kind == .MousePressed and hotItem != Swag.U64.Max
        {
            if evt.button == .Left or evt.button == .Right
            {
                select(hotItem)
                if evt.button == .Right
                {
                    with let menu = MenuCtrl.createPopup(self)
                    {
                        .addItem(CmdIdDupCapture)
                        .addItem(CmdIdDeleteCapture)
                        .addSeparator()
                        .addItem(CmdIdCopyAsImage)
                        .addSeparator()
                        .addItem(CmdIdCaptureFileLocation)
                        .addItem(CmdIdLinkedFileLocation)
                        .addSeparator()
                        .addItem(CmdIdFileDetails)
                    }

                    discard menu.doModal(evt.surfacePos, autoDestroy: true)
                }
            }
        }
    }

    func paintRecent(parent: *Wnd, painter: *Painter, texture: Texture, rect: Math.Rectangle, hot, selected: bool, linked: String)
    {
        let colors = parent.getThemeColors()

        painter.fillRoundRect(rect, 8, 8, colors.wnd_Bk)

        if texture.isValid()
        {
            var txtRect = rect
            txtRect.inflate(-RecentPadding * 0.5)

            let ratio = Math.min(txtRect.width / texture.width, txtRect.height / texture.height)

            txtRect.x      = rect.horzCenter() - (texture.width * 0.5 * ratio)
            txtRect.y      = rect.vertCenter() - (texture.height * 0.5 * ratio)
            txtRect.width  = texture.width * ratio
            txtRect.height = texture.height * ratio

            with painter
            {
                .fillRect(txtRect, Argb.LightGray)
                .fillRect(txtRect, Brush.createHatch(.CheckBoardLarge, Argb.DarkGray))
                .setInterpolationMode(.Linear)
                .drawTexture(txtRect, texture)
            }
        }

        if linked != null
        {
            painter.fillRoundRect({rect.x + 4, rect.y + 4, 24, 24}, 8, 8, colors.wnd_Bk)
            painter.drawRoundRect({rect.x + 4, rect.y + 4, 24, 24}, 8, 8, Argb.White, 2)
            var icon = parent.getTheme().getIcon24(.Link)
            icon.sizeX, icon.sizeY = 16
            icon.paint(painter, rect.x + 8, rect.y + 8, Argb.White)
        }

        if hot:
            painter.drawRoundRect(rect, 8, 8, colors.hilightLight, 2)
        if selected:
            painter.drawRoundRect(rect, 8, 8, colors.hilightLight, 4)
    }

    mtd impl onPaintEvent(evt: *PaintEvent)
    {
        let painter    = evt.bc.painter
        var clientRect = getClientRectPadding()

        painter.setParams(.Antialiased)
        var x = clientRect.x
        foreach i, idx in items
        {
            let w = clientRect.height * 4.0 / 3

            var rect = Math.Rectangle{x, clientRect.y, w, clientRect.height}
            i.rect = rect

            paintRecent(self, painter, i.preview, rect, idx == hotItem, idx == selected, i.linked)
            x = rect.right() + RecentPadding
        }

        recentWnd.setScrollSize(x, 0)
    }
}

struct RecentWnd
{
    using scrollWnd:     ScrollWnd
    main:                *MainWnd
    recentView:          *RecentView
    timer:               *Timer
    loadDone:            bool
}

impl IWnd for RecentWnd
{
    mtd impl onDestroyEvent(evt: *DestroyEvent)
    {
        if timer:
            getApp().deleteTimer(timer)
    }

    mtd impl onTimerEvent(evt: *TimerEvent)
    {
        let capture = main.getCapture()
        if !capture:
            return

        capture.computePreview()
        let item = recentView.items[recentView.selected]
        getApp().getRenderer().removeTexture(&item.preview)
        item.preview = getApp().getRenderer().addImage(capture.preview)
        invalidate()

        main.libWnd.view.updateRecent(capture)

        getApp().deleteTimer(evt.timer)
        timer = null
        catch capture.save()
    }
}

impl RecentWnd
{
    func create(parent: *Wnd, main: *MainWnd)->*RecentWnd
    {
        with let recent = Wnd.create'RecentWnd(parent, {0, 0, 0, PreviewHeight + RecentPadding * 2})
        {
            .scrollWndFlags |= .SmallBar | .ForceHorizontal
            .backgroundStyle = .Window
            .main            = main
            .style.addStyleSheetColors("scrollBar_Bk $view_Bk")

            .recentView           = recent.createView'RecentView()
            .recentView.padding   = RecentPadding
            .recentView.main      = main
            .recentView.recentWnd = recent
            .view.backgroundStyle = .View
        }

        return recent
    }

    mtd loadRecent()
    {
        if loadDone:
            return
        loadDone = true

        g_Library.fill()
        g_Library.onDeleteFile += func|self|(file)
        {
            foreach it, i in self.recentView.items
            {
                if it.file == file
                { 
                    // Remove from recent
                    self.recentView.items.deletePtr(it)
                    self.recentView.items.removeAtOrdered(i)

                    // Select new one
                    if self.recentView.selected == i
                    {
                        self.recentView.selected = Swag.U64.Max
                        self.recentView.select(i)    
                    }

                    break                               
                }
            }            
        }

        var res = g_Library.files
        res.sort(func(a, b)
        {
            return b.lastWriteTime <=> a.lastWriteTime
        })

        var cptRecent = 0
        foreach r, idx in res
        {
            let capture = catch addRecent(r.fullname, front: false)
            defer<err> Memory.delete(capture)
            if @err() != null:
                continue

            if idx == 0
            {
                recentView.selected = 0
                main.editWnd.editView.setCapture(capture)
            }
            else:
                Memory.delete(capture)

            cptRecent += 1
            if cptRecent > MaxRecent:
                break
        }
    }

    mtd updateRecent()
    {
        if recentView.selected >= recentView.items.count:
            return

        if timer:
            getApp().deleteTimer(timer)
        timer = getApp().addTimer(1's, self)

        let item    = recentView.items[recentView.selected]
        let capture = main.getCapture()
        item.linked = capture.linkedImage
    }

    mtd removeRecent()
    {
        if recentView.selected >= recentView.items.count:
            return

        let item = recentView.items[recentView.selected]

        // Be sure there's no pending save
        if timer:
            getApp().deleteTimer(timer)
        timer = null    

        // Delete file
        g_Library.removeFile(item.file)
    }

    #[Swag.Overload]
    mtd addRecent(file: string, front = true, sel = false)->*Capture throw
    {
        let capture = Capture.load(file)
        addRecent(capture.file, capture.preview, front, sel, capture.linkedImage)

        // Too much recent. Remove the oldest
        if front and recentView.items.count > MaxRecent
        {
            let back = recentView.items.back()
            if back.preview.isValid():
                getApp().getRenderer().removeTexture(&back.preview)
            recentView.items.deletePtr(back)
            recentView.items.count -= 1
        }

        return capture
    }

    #[Swag.Overload]
    mtd addRecent(file: string, preview: Image, front = true, sel = false, linked: string = null)
    {
        // Already there ?
        foreach it, i in recentView.items
        {
            if it.file == file
            {
                var idxSel = i

                // Move the item to the first position
                if front and i
                {
                    recentView.items.swap(0, i)
                    idxSel = 0
                }

                if sel:
                    recentView.select(idxSel, true)

                it.linked = linked
                return
            }
        }

        let item = recentView.items.newPtr()
        item.preview = getApp().getRenderer().addImage(preview)
        item.file    = file
        item.linked  = linked
        if front
        {
            recentView.items.insertAt(0, item)
            if sel:
                recentView.select(0, force: true)
        }
        else
        {
            recentView.items.add(item)
            if sel:
                recentView.select(recentView.items.count - 1, force: true)
        }

        invalidate()
    }
}
