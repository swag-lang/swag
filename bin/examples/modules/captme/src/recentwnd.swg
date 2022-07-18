const MaxRecent = 10

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
    selected:   uint = Swag.UInt.Max
    hotItem:    uint = Swag.UInt.Max
}

impl RecentView
{
    mtd select(idx: uint)
    {
        if selected == idx
            return

        if idx >= items.count
        {
            selected = Swag.UInt.Max
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
                main.getCapture().save()
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
        ht := Swag.UInt.Max
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

        if evt.kind == .MousePressed and hotItem != Swag.UInt.Max
        {
            if evt.button == .Left or evt.button == .Right
            {
                select(hotItem)
                if evt.button == .Right
                {
                    menu := MenuCtrl.createPopup(self)
                    menu.addItem(CmdIdDupCapture)
                    menu.addItem(CmdIdDeleteCapture)
                    menu.addSeparator()
                    menu.addItem(CmdIdFileLocation)
                    res := menu.doModal(evt.surfacePos, autoDestroy: true)
                }
            }
        }
    }

    mtd onPaintEvent(evt: *PaintEvent)
    {
        painter    := evt.bc.painter
        clientRect := getClientRectPadding()
        colors     := getThemeColors()

        painter.setParams(.Antialiased)
        x := clientRect.x
        visit i, idx: items
        {
            h := PreviewHeight + RecentPadding*2
            w := PreviewWidth + RecentPadding*2

            rect := Math.Rectangle{x, clientRect.y, w, clientRect.height}
            i.rect = rect

            painter.fillRoundRect(rect, 8, 8, colors.wnd_Bk)
            defer x = rect.right() + RecentPadding

            // Hot
            if idx == hotItem
                painter.drawRoundRect(rect, 8, 8, colors.hilightLight, 2)
            // Selected
            if idx == selected
                painter.drawRoundRect(rect, 8, 8, colors.hilightLight, 4)

            if !i.preview.isValid()
                continue

            txtRect := rect
            txtRect.inflate(-RecentPadding*0.5)

            ratio := Math.min(txtRect.width / i.preview.width, txtRect.height / i.preview.height)
            ratio =  Math.min(ratio, 1)

            txtRect.x = rect.horzCenter() - (i.preview.width * 0.5 * ratio)
            txtRect.y = rect.vertCenter() - (i.preview.height * 0.5 * ratio)
            txtRect.width = i.preview.width * ratio
            txtRect.height = i.preview.height * ratio

            painter.fillRect(txtRect, Argb.LightGray)
            painter.fillRect(txtRect, Brush.createHatch(.CheckBoardLarge, Argb.DarkGray))

            painter.setInterpolationMode(.Linear)
            painter.drawTexture(txtRect, i.preview)
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
        if !capture return

        capture.computePreview()
        item := recentView.items[recentView.selected]
        getApp().getRenderer().removeTexture(&item.preview)
        item.preview = getApp().getRenderer().addImage(capture.preview)
        invalidate()

        getApp().deleteTimer(evt.timer)
        timer = null
        capture.save()
    }
}

impl RecentWnd
{
    func create(parent: *MainWnd)->*RecentWnd
    {
        recent := Wnd.create'RecentWnd(parent, @{0, 0, 0, PreviewHeight + RecentPadding*2})
        recent.scrollWndFlags |= .SmallBar | .ForceHorizontal
        recent.backgroundStyle = .Window
        recent.main = parent

        recent.recentView = recent.createView'RecentView()
        recent.recentView.padding = RecentPadding
        recent.recentView.main = parent
        recent.recentView.recentWnd = recent
        recent.view.backgroundStyle = .View

        res := catch Directory.enumerateFiles(Library.getFolder(), "*.captme")
        if @err return recent
        res.sort(@(a, b) {
            return a.creationTime <=> b.creationTime
        })

        cptRecent := 0
        visit r, idx: res
        {
            capture := catch recent.addRecent(r.fullname)
            if @err continue

            if idx == res.count - 1
            {
                recent.recentView.selected = 0
                parent.editWnd.editView.setCapture(capture)
            }
            else
                Memory.delete(capture)

            cptRecent += 1
            if cptRecent > MaxRecent
                break
        }

        return recent
    }

    mtd updateRecent(capture: *Capture)
    {
        if recentView.selected >= recentView.items.count
            return

        if timer
            getApp().deleteTimer(timer)
        timer = getApp().addTimer(1's, self)
    }

    mtd addRecent(file: string)->*Capture throw
    {
        capture := Capture.load(file)
        if @err return null

        addRecent(capture.file, capture.preview)
        if recentView.items.count > MaxRecent
        {
            recentView.items.deletePtr(recentView.items.back())
            recentView.items.count -= 1
        }

        return capture
    }

    mtd addRecent(file: string, preview: Image)
    {
        item := recentView.items.newPtr()
        item.preview = getApp().getRenderer().addImage(preview)
        item.file = file
        recentView.items.insertAt(0, item)
        invalidate()
    }
}