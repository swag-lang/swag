const MaxRecent = 15

const RecentPadding = 8.0

struct RecentItem
{
    file:               String
    linked:             String
    linkedUpToDate:     bool
    preview:            Texture
    rect:               Math.Rectangle
    zoom:               f32
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
        if me.selected == idx and !force:
            return

        if idx >= me.items.count
        {
            me.selected = Swag.U64.Max
            return
        }

        let view = me.main.editWnd.editView

        // Save current zoom
        if me.selected < me.items.count:
            me.items[me.selected].zoom = view.zoom

        // Select new recent item
        me.selected = idx
        let item    = me.items[idx]
        let capture = catch Capture.load(item.file)
        if capture
        {
            if me.main.getCapture():
                catch me.main.getCapture().save()
            view.setCapture(capture)
            if item.zoom == 0:
                view.zoomToFit(false)
            else:
                view.setZoom(item.zoom)
            view.main.freezeSelection(false)
            me.invalidate()
        }
    }
}

impl IWnd for RecentView
{
    mtd impl onMouseEvent(evt: *MouseEvent)
    {
        if evt.kind == .MouseWheel:
            evt.accepted = false

        var pos = me.surfaceToLocal(evt.surfacePos)
        pos += me.recentWnd.getScrollPos()

        // Hit test
        var ht = Swag.U64.Max
        foreach i, idx in me.items
        {
            if i.rect.contains(pos)
            {
                ht = idx
                break
            }
        }

        if ht != me.hotItem
        {
            me.invalidate()
            me.hotItem = ht
        }

        if evt.kind == .MousePressed and me.hotItem != Swag.U64.Max
        {
            if evt.button == .Left or evt.button == .Right
            {
                me.select(me.hotItem)
                if evt.button == .Right
                {
                    with let menu = MenuCtrl.createPopup(me)
                    {
                        .addItem(ActionFile.UpdateLinkedImage)
                        .addSeparator()
                        .addItem(ActionFile.DupCapture)
                        .addItem(ActionFile.DeleteCapture)
                        .addSeparator()
                        .addItem(ActionCopyPaste.CopyAsImage)
                        .addSeparator()
                        .addItem(ActionFile.CaptureFileLocation)
                        .addItem(ActionFile.LinkedFileLocation)
                        .addSeparator()
                        .addItem(ActionFile.FileDetails)
                    }

                    discard menu.doModal(evt.surfacePos, autoDestroy: true)
                }
            }
        }
    }

    func paintRecent(parent: *Wnd, painter: *Painter, texture: Texture, rect: Math.Rectangle, hot, selected, linkedUpToDate: bool, linked: String)
    {
        let colors = parent.getThemeColors()

        painter.setParams(.Antialiased)
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
            let backColor = linkedUpToDate ? colors.bk : ColorYellow
            var icon11 = Icon.from(&g_Main.icons48, 21)
            icon11.sizeX, icon11.sizeY = 24
            icon11.paint(painter, rect.x + 4, rect.y + 4, backColor)

            var icon12 = Icon.from(&g_Main.icons48, 22)
            icon12.sizeX, icon12.sizeY = 24
            icon12.paint(painter, rect.x + 4, rect.y + 4, backColor.getContrasting())

            var icon = parent.getTheme().getIcon24(.Link)
            icon.sizeX, icon.sizeY = 16
            icon.paint(painter, rect.x + 8, rect.y + 8, backColor.getContrasting())
        }

        if hot:
            painter.drawRoundRect(rect, 8, 8, colors.hilightLight, 2)
        if selected:
            painter.drawRoundRect(rect, 8, 8, colors.hilightLight, 4)
    }

    mtd impl onPaintEvent(evt: *PaintEvent)
    {
        let painter    = evt.bc.painter
        let clientRect = me.getClientRectPadding()

        var x = clientRect.x
        foreach item, idx in me.items
        {
            let w = clientRect.height * 4.0 / 3

            let rect = Math.Rectangle{x, clientRect.y, w, clientRect.height}
            item.rect = rect
            paintRecent(me, painter, item.preview, rect, idx == me.hotItem, idx == me.selected, item.linkedUpToDate, item.linked)
            x = rect.right() + RecentPadding
        }

        me.recentWnd.setScrollSize(x, 0)
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
        if me.timer:
            me.getApp().deleteTimer(me.timer)
    }

    mtd impl onTimerEvent(evt: *TimerEvent)
    {
        let capture = me.main.getCapture()
        if capture and capture.changed
        {
            capture.computePreview()
            let item = me.recentView.items[me.recentView.selected]
            me.getApp().getRenderer().removeTexture(&item.preview)
            item.preview = me.getApp().getRenderer().addImage(capture.preview)
            me.invalidate()

            me.main.libWnd.view.updateRecent(capture)

            catch capture.save()
        }

        me.getApp().deleteTimer(evt.timer)
        me.timer = null
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
        if me.loadDone:
            return
        me.loadDone = true

        g_Library.fill()
        g_Library.onDeleteFile += mtd||(file)
        {
            foreach it, i in me.recentView.items
            {
                if it.file == file
                {
                    // Remove from recent
                    me.recentView.items.deletePtr(it)
                    me.recentView.items.removeAtOrdered(i)

                    // Select new one
                    if me.recentView.selected == i
                    {
                        me.recentView.selected = Swag.U64.Max
                        me.recentView.select(i)
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
            let capture = catch me.addRecent(r.fullname, front: false)
            defer #err Memory.delete(capture)
            if @err != null:
                continue

            if idx == 0
            {
                me.recentView.selected = 0
                me.main.editWnd.editView.setCapture(capture)
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
        if me.recentView.selected >= me.recentView.items.count:
            return

        if me.timer:
            me.getApp().deleteTimer(me.timer)
        me.timer = me.getApp().addTimer(1's, me)

        let capture = me.main.getCapture()
        if !capture:
            return

        let item = me.recentView.items[me.recentView.selected]
        item.linked         = capture.linkedImage
        item.linkedUpToDate = capture.linkedUpToDate
        me.invalidate()
    }

    mtd removeRecent()
    {
        if me.recentView.selected >= me.recentView.items.count:
            return

        let item = me.recentView.items[me.recentView.selected]

        // Be sure there's no pending save
        if me.timer:
            me.getApp().deleteTimer(me.timer)
        me.timer = null

        // Delete file
        g_Library.removeFile(item.file)
    }

    #[Swag.Overload]
    mtd addRecent(file: string, front = true, sel = false)->*Capture throw
    {
        let capture = Capture.load(file)
        me.addRecent(capture.file, capture.preview, front, sel, capture.linkedUpToDate, capture.linkedImage)

        // Too much recent. Remove the oldest
        if front and me.recentView.items.count > MaxRecent
        {
            let back = me.recentView.items.back()
            if back.preview.isValid():
                me.getApp().getRenderer().removeTexture(&back.preview)
            me.recentView.items.deletePtr(back)
            me.recentView.items.count -= 1
        }

        return capture
    }

    #[Swag.Overload]
    mtd addRecent(file: string, preview: Image, front = true, sel = false, linkedUpToDate = true, linked: string = null)
    {
        // Already there ?
        foreach it, i in me.recentView.items
        {
            if it.file == file
            {
                var idxSel = i

                // Move the item to the first position
                if front and i
                {
                    me.recentView.items.swap(0, i)
                    idxSel = 0
                }

                if sel:
                    me.recentView.select(idxSel, true)

                it.linked = linked
                return
            }
        }

        let item = me.recentView.items.newPtr()
        item.preview        = me.getApp().getRenderer().addImage(preview)
        item.file           = file
        item.linked         = linked
        item.linkedUpToDate = linkedUpToDate
        if front
        {
            me.recentView.items.insertAt(0, item)
            if sel
            {
                me.recentView.select(0, force: true)
                me.setScrollPos(0, 0)
            }
        }
        else
        {
            me.recentView.items.add(item)
            if sel:
                me.recentView.select(me.recentView.items.count - 1, force: true)
        }

        me.invalidate()
    }
}
