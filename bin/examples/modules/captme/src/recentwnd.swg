const MaxRecent = 15

const RecentPadding = 8.0

struct RecentItem
{
    file:       String
    preview:    Texture
    rect:       Math.Rectangle
    zoom:       f32
}

struct RecentView
{
    using wnd: Wnd

    main:       *MainWnd
    recentWnd:  *RecentWnd
    items:      ArrayPtr'RecentItem
    selected:   u64 = Swag.U64.Max
    hotItem:    u64 = Swag.U64.Max
}

impl RecentView
{
    mtd select(idx: u64, force = false)
    {
        if selected == idx and !force
            return

        if idx >= items.count
        {
            selected = Swag.U64.Max
            return
        }

        view := main.editWnd.editView

        // Save current zoom
        if selected < items.count
            items[selected].zoom = view.zoom

        // Select new recent item
        selected = idx
        item := items[idx]
        capture := catch Capture.load(item.file)
        if capture
        {
            if main.getCapture()
                catch main.getCapture().save()
            view.setCapture(capture)
            if item.zoom == 0
                view.zoomToFit(false)
            else
                view.setZoom(item.zoom)
            invalidate()
        }
    }
}

impl IWnd for RecentView
{
    mtd onMouseEvent(evt: *MouseEvent)
    {
        if evt.kind == .MouseWheel
            evt.accepted = false

        pos := surfaceToLocal(evt.surfacePos)
        pos += recentWnd.getScrollPos()

        // Hit test
        ht := Swag.U64.Max
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
                    with menu := MenuCtrl.createPopup(self)
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
        colors := parent.getThemeColors()

        painter.fillRoundRect(rect, 8, 8, colors.wnd_Bk)

        if hot
            painter.drawRoundRect(rect, 8, 8, colors.hilightLight, 2)
        if selected
            painter.drawRoundRect(rect, 8, 8, colors.hilightLight, 4)

        if !texture.isValid()
            return

        txtRect := rect
        txtRect.inflate(-RecentPadding*0.5)

        ratio := Math.min(txtRect.width / texture.width, txtRect.height / texture.height)

        txtRect.x = rect.horzCenter() - (texture.width * 0.5 * ratio)
        txtRect.y = rect.vertCenter() - (texture.height * 0.5 * ratio)
        txtRect.width = texture.width * ratio
        txtRect.height = texture.height * ratio

        painter.fillRect(txtRect, Argb.LightGray)
        painter.fillRect(txtRect, Brush.createHatch(.CheckBoardLarge, Argb.DarkGray))

        painter.setInterpolationMode(.Linear)
        painter.drawTexture(txtRect, texture)
    }

    mtd onPaintEvent(evt: *PaintEvent)
    {
        painter    := evt.bc.painter
        clientRect := getClientRectPadding()

        painter.setParams(.Antialiased)
        x := clientRect.x
        visit i, idx: items
        {
            w := clientRect.height * 4.0/3

            rect := Math.Rectangle{x, clientRect.y, w, clientRect.height}
            i.rect = rect

            paintRecent(self, painter, i.preview, rect, idx == hotItem, idx == selected)
            x = rect.right() + RecentPadding
        }

        recentWnd.setScrollSize(x, 0)
    }
}

struct RecentWnd
{
    using scrollWnd: ScrollWnd
    main:       *MainWnd
    recentView: *RecentView
    timer:      *Timer
}

impl IWnd for RecentWnd
{
    mtd onTimerEvent(evt: *TimerEvent)
    {
        capture := main.getCapture()
        if !capture
            return

        capture.computePreview()
        item := recentView.items[recentView.selected]
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
        with recent := Wnd.create'RecentWnd(parent, {0, 0, 0, PreviewHeight + RecentPadding*2})
        {
            .scrollWndFlags |= .SmallBar | .ForceHorizontal
            .backgroundStyle = .Window
            .main = main
            .style.addStyleSheetColors("scrollBar_Bk $view_Bk")

            .recentView = recent.createView'RecentView()
            .recentView.padding = RecentPadding
            .recentView.main = main
            .recentView.recentWnd = recent
            .view.backgroundStyle = .View
        }

        g_Library.fill()

        res := g_Library.files
        res.sort(func(a, b) {
            return b.lastWriteTime <=> a.lastWriteTime
        })

        cptRecent := 0
        visit r, idx: res
        {
            capture := catch recent.addRecent(r.fullname, front: false)
            defer(err) Memory.delete(capture)
            if @err continue

            if idx == 0
            {
                recent.recentView.selected = 0
                main.editWnd.editView.setCapture(capture)
            }
            else
                Memory.delete(capture)

            cptRecent += 1
            if cptRecent > MaxRecent
                break
        }

        return recent
    }

    mtd updateRecent()
    {
        if recentView.selected >= recentView.items.count
            return

        if timer
            getApp().deleteTimer(timer)
        timer = getApp().addTimer(1's, self)
    }

    mtd removeRecent()
    {
        if recentView.selected >= recentView.items.count
            return

        item := recentView.items[recentView.selected]

        catch File.delete(item.file)
        if @err
        {
            MessageDlg.error(surface, @err, "Failed to delete capture file !")
            return
        }

        g_Library.removeFile(item.file)

        // Be sure there's no pending save
        if timer
            getApp().deleteTimer(timer)
        timer = null

        // Remove from recent
        recentView.items.deletePtr(item)
        recentView.items.removeAtOrdered(recentView.selected)

        // Select new one
        sel := recentView.selected
        recentView.selected = Swag.U64.Max
        recentView.select(sel)
    }

    mtd addRecent(file: string, front = true, sel = false)->*Capture throw
    {
        capture := Capture.load(file)
        addRecent(capture.file, capture.preview, front, sel)

        // Too much recent. Remove the oldest
        if front and recentView.items.count > MaxRecent
        {
            back := recentView.items.back()
            if back.preview.isValid()
                getApp().getRenderer().removeTexture(&back.preview)
            recentView.items.deletePtr(back)
            recentView.items.count -= 1
        }

        return capture
    }

    mtd addRecent(file: string, preview: Image, front = true, sel = false)
    {
        // Already there ?
        visit it, i: recentView.items
        {
            if it.file == file
            {
                idxSel := i

                // Move the item to the first position
                if front and i
                {
                    recentView.items.swap(0, i)
                    idxSel = 0
                }

                if sel
                    recentView.select(idxSel, true)
                return
            }
        }

        item := recentView.items.newPtr()
        item.preview = getApp().getRenderer().addImage(preview)
        item.file = file
        if front
        {
            recentView.items.insertAt(0, item)
            if sel
                recentView.select(0, force: true)
        }
        else
        {
            recentView.items.add(item)
            if sel
                recentView.select(recentView.items.count - 1, force: true)
        }

        invalidate()
    }
}