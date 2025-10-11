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

struct RecentInfos
{
    hot:                bool
    selected:           bool
    linkedUpToDate:     bool
    linked:             String
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
        if .selected == idx and !force do
            return

        if idx >= .items.count
        {
            .selected = Swag.U64.Max
            return
        }

        let view = .main.editWnd.editView

        // Save current zoom
        if .selected < .items.count do
            .items[.selected].zoom = view.zoom

        // Select new recent item
        .selected = idx
        let item    = .items[idx]
        let capture = catch Capture.load(item.file)
        if capture
        {
            if .main.getCapture() do
                catch .main.getCapture().save()
            view.setCapture(capture)
            if item.zoom == 0 do
                view.zoomToFit(false)
            else do
                view.setZoom(item.zoom)
            view.main.freezeSelection(false)
            .invalidate()
        }
    }
}

impl IWnd for RecentView
{
    mtd impl onMouseEvent(evt: *MouseEvent)
    {
        if evt.kind == .MouseWheel do
            evt.accepted = false

        var pos = .surfaceToLocal(evt.surfacePos)
        pos += .recentWnd.getScrollPos()

        // Hit test
        var ht = Swag.U64.Max
        foreach i, idx in .items
        {
            if i.rect.contains(pos)
            {
                ht = idx
                break
            }
        }

        if ht != .hotItem
        {
            .invalidate()
            .hotItem = ht
        }

        if evt.kind == .MousePressed and .hotItem != Swag.U64.Max
        {
            if evt.button == .Left or evt.button == .Right
            {
                .select(.hotItem)
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

    func paintRecent(parent: *Wnd, painter: *Painter, texture: Texture, rect: Math.Rectangle, infos: RecentInfos)
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

        if infos.linked != null
        {
            let backColor = infos.linkedUpToDate ? colors.bk : ColorYellow
            var icon11    = Icon.from(&g_Main.icons48, 21)
            icon11.sizeX, icon11.sizeY = 24
            icon11.paint(painter, rect.x + 4, rect.y + 4, backColor)

            var icon12 = Icon.from(&g_Main.icons48, 22)
            icon12.sizeX, icon12.sizeY = 24
            icon12.paint(painter, rect.x + 4, rect.y + 4, backColor.getContrasting())

            var icon = parent.getTheme().getIcon24(.Link)
            icon.sizeX, icon.sizeY = 16
            icon.paint(painter, rect.x + 8, rect.y + 8, backColor.getContrasting())
        }

        if infos.hot do
            painter.drawRoundRect(rect, 8, 8, colors.hilightLight, 2)
        if infos.selected do
            painter.drawRoundRect(rect, 8, 8, colors.hilightLight, 4)
    }

    mtd impl onPaintEvent(evt: *PaintEvent)
    {
        let painter    = evt.bc.painter
        let clientRect = .getClientRectPadding()

        var x = clientRect.x
        foreach item, idx in .items
        {
            let w = clientRect.height * 4.0 / 3

            let rect = Math.Rectangle{x, clientRect.y, w, clientRect.height}
            item.rect = rect

            with var infos: RecentInfos
            {
                .hot            = idx == me.hotItem
                .selected       = idx == me.selected
                .linkedUpToDate = item.linkedUpToDate
                .linked         = item.linked
            }

            paintRecent(me, painter, item.preview, rect, infos)
            x = rect.right() + RecentPadding
        }

        .recentWnd.setScrollSize(x, 0)
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
        if .timer do
            .getApp().deleteTimer(.timer)
    }

    mtd impl onTimerEvent(evt: *TimerEvent)
    {
        let capture = .main.getCapture()
        if capture and capture.changed
        {
            capture.computePreview()
            let item = .recentView.items[.recentView.selected]
            .getApp().getRenderer().removeTexture(&item.preview)
            item.preview = .getApp().getRenderer().addImage(capture.preview)
            .invalidate()

            .main.libWnd.view.updateRecent(capture)

            catch capture.save()
        }

        .getApp().deleteTimer(evt.timer)
        .timer = null
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
        if .loadDone do
            return
        .loadDone = true

        g_Library.fill()
        g_Library.onDeleteFile += mtd||(file)
        {
            foreach it, i in .recentView.items
            {
                if it.file == file
                {
                    // Remove from recent
                    .recentView.items.deletePtr(it)
                    .recentView.items.removeAtOrdered(i)

                    // Select new one
                    if .recentView.selected == i
                    {
                        .recentView.selected = Swag.U64.Max
                        .recentView.select(i)
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
            let capture = catch .addRecent(r.fullname, front: false)
            defer #err Memory.delete(capture)
            if @err != null do
                continue

            if idx == 0
            {
                .recentView.selected = 0
                .main.editWnd.editView.setCapture(capture)
            }
            else do
                Memory.delete(capture)

            cptRecent += 1
            if cptRecent > MaxRecent do
                break
        }
    }

    mtd updateRecent()
    {
        if .recentView.selected >= .recentView.items.count do
            return

        if .timer do
            .getApp().deleteTimer(.timer)
        .timer = .getApp().addTimer(1's, me)

        let capture = .main.getCapture()
        if !capture do
            return

        let item = .recentView.items[.recentView.selected]
        item.linked         = capture.linkedImage
        item.linkedUpToDate = capture.linkedUpToDate
        .invalidate()
    }

    mtd removeRecent()
    {
        if .recentView.selected >= .recentView.items.count do
            return

        let item = .recentView.items[.recentView.selected]

        // Be sure there's no pending save
        if .timer do
            .getApp().deleteTimer(.timer)
        .timer = null

        // Delete file
        g_Library.removeFile(item.file)
    }

    #[Swag.Overload]
    mtd addRecent(file: string, front = true, sel = false)->*Capture throw
    {
        let capture = Capture.load(file)
        .addRecent(capture.file, capture.preview, front, sel, capture.linkedUpToDate, capture.linkedImage)

        // Too much recent. Remove the oldest
        if front and .recentView.items.count > MaxRecent
        {
            let back = .recentView.items.back()
            if back.preview.isValid() do
                .getApp().getRenderer().removeTexture(&back.preview)
            .recentView.items.deletePtr(back)
            .recentView.items.count -= 1
        }

        return capture
    }

    #[Swag.Overload]
    mtd addRecent(file: string, preview: Image, front = true, sel = false, linkedUpToDate = true, linked: string = null)
    {
        // Already there ?
        foreach it, i in .recentView.items
        {
            if it.file == file
            {
                var idxSel = i

                // Move the item to the first position
                if front and i
                {
                    .recentView.items.swap(0, i)
                    idxSel = 0
                }

                if sel do
                    .recentView.select(idxSel, true)

                it.linked = linked
                return
            }
        }

        let item = .recentView.items.newPtr()
        item.preview        = .getApp().getRenderer().addImage(preview)
        item.file           = file
        item.linked         = linked
        item.linkedUpToDate = linkedUpToDate
        if front
        {
            .recentView.items.insertAt(0, item)
            if sel
            {
                .recentView.select(0, force: true)
                .setScrollPos(0, 0)
            }
        }
        else
        {
            .recentView.items.add(item)
            if sel do
                .recentView.select(.recentView.items.count - 1, force: true)
        }

        .invalidate()
    }
}
