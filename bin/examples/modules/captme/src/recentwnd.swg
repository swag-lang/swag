const MaxRecent = 15

const RecentPadding = 8.0

struct RecentItem
{
    file:        String
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
        if selected == idx and !force do
            return

        if idx >= items.count
        {
            selected = Swag.U64.Max
            return
        }

        let view = main.editWnd.editView

        // Save current zoom
        if selected < items.count do
            items[selected].zoom = view.zoom

        // Select new recent item
        selected = idx
        let item    = items[idx]
        let capture = catch Capture.load(item.file)
        if capture
        {
            if main.getCapture() do
                catch main.getCapture().save()
            view.setCapture(capture)
            if item.zoom == 0 do
                view.zoomToFit(false)
            else do
                view.setZoom(item.zoom)
            invalidate()
        }
    }
}

impl IWnd for RecentView
{
    mtd impl onMouseEvent(evt: *MouseEvent)
    {
        if evt.kind == .MouseWheel do
            evt.accepted = false

        var pos = surfaceToLocal(evt.surfacePos)
        pos += recentWnd.getScrollPos()

        // Hit test
        var ht = Swag.U64.Max
        visit i, idx: items
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
                        .addItem(CmdIdFileLocation)
                    }

                    discard menu.doModal(evt.surfacePos, autoDestroy: true)
                }
            }
        }
    }

    func paintRecent(parent: *Wnd, painter: *Painter, texture: Texture, rect: Math.Rectangle, hot, selected: bool)
    {
        let colors = parent.getThemeColors()

        painter.fillRoundRect(rect, 8, 8, colors.wnd_Bk)

        if hot do
            painter.drawRoundRect(rect, 8, 8, colors.hilightLight, 2)
        if selected do
            painter.drawRoundRect(rect, 8, 8, colors.hilightLight, 4)

        if !texture.isValid() do
            return

        var txtRect = rect
        txtRect.inflate(-RecentPadding * 0.5)

        let ratio = Math.min(txtRect.width / texture.width, txtRect.height / texture.height)

        txtRect.x      = rect.horzCenter() - (texture.width * 0.5 * ratio)
        txtRect.y      = rect.vertCenter() - (texture.height * 0.5 * ratio)
        txtRect.width  = texture.width * ratio
        txtRect.height = texture.height * ratio

        painter.fillRect(txtRect, Argb.LightGray)
        painter.fillRect(txtRect, Brush.createHatch(.CheckBoardLarge, Argb.DarkGray))

        painter.setInterpolationMode(.Linear)
        painter.drawTexture(txtRect, texture)
    }

    mtd impl onPaintEvent(evt: *PaintEvent)
    {
        let painter    = evt.bc.painter
        var clientRect = getClientRectPadding()

        painter.setParams(.Antialiased)
        var x = clientRect.x
        visit i, idx: items
        {
            let w = clientRect.height * 4.0 / 3

            var rect = Math.Rectangle{x, clientRect.y, w, clientRect.height}
            i.rect = rect

            paintRecent(self, painter, i.preview, rect, idx == hotItem, idx == selected)
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
}

impl IWnd for RecentWnd
{
    mtd impl onTimerEvent(evt: *TimerEvent)
    {
        let capture = main.getCapture()
        if !capture do
            return

        capture.computePreview()
        let item = recentView.items[recentView.selected]
        getApp().getRenderer().removeTexture(&item.preview)
        item.preview = getApp().getRenderer().addImage(capture.preview)
        invalidate()

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

        g_Library.fill()

        var res = g_Library.files
        res.sort(func(a, b)
        {
            return b.lastWriteTime <=> a.lastWriteTime
        })

        var cptRecent = 0
        visit r, idx: res
        {
            let capture = catch recent.addRecent(r.fullname, front: false)
            defer,err Memory.delete(capture)
            if @err() != null do
                continue

            if idx == 0
            {
                recent.recentView.selected = 0
                main.editWnd.editView.setCapture(capture)
            }
            else do
                Memory.delete(capture)

            cptRecent += 1
            if cptRecent > MaxRecent do
                break
        }

        return recent
    }

    mtd updateRecent()
    {
        if recentView.selected >= recentView.items.count do
            return

        if timer do
            getApp().deleteTimer(timer)
        timer = getApp().addTimer(1's, self)
    }

    mtd removeRecent()
    {
        if recentView.selected >= recentView.items.count do
            return

        let item = recentView.items[recentView.selected]

        catch File.delete(item.file)
        if @err() != null
        {
            MessageDlg.error(surface, "Failed to delete capture file")
            return
        }

        g_Library.removeFile(item.file)

        // Be sure there's no pending save
        if timer do
            getApp().deleteTimer(timer)
        timer = null

        // Remove from recent
        recentView.items.deletePtr(item)
        recentView.items.removeAtOrdered(recentView.selected)

        // Select new one
        let sel = recentView.selected
        recentView.selected = Swag.U64.Max
        recentView.select(sel)
    }

    #[Swag.Overload]
    mtd addRecent(file: string, front = true, sel = false)->*Capture throw
    {
        let capture = Capture.load(file)
        addRecent(capture.file, capture.preview, front, sel)

        // Too much recent. Remove the oldest
        if front and recentView.items.count > MaxRecent
        {
            let back = recentView.items.back()
            if back.preview.isValid() do
                getApp().getRenderer().removeTexture(&back.preview)
            recentView.items.deletePtr(back)
            recentView.items.count -= 1
        }

        return capture
    }

    #[Swag.Overload]
    mtd addRecent(file: string, preview: Image, front = true, sel = false)
    {
        // Already there ?
        visit it, i: recentView.items
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

                if sel do
                    recentView.select(idxSel, true)
                return
            }
        }

        let item = recentView.items.newPtr()
        item.preview = getApp().getRenderer().addImage(preview)
        item.file    = file
        if front
        {
            recentView.items.insertAt(0, item)
            if sel do
                recentView.select(0, force: true)
        }
        else
        {
            recentView.items.add(item)
            if sel do
                recentView.select(recentView.items.count - 1, force: true)
        }

        invalidate()
    }
}
