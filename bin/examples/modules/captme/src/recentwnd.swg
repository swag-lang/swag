const MaxRecent = 10

struct RecentItem
{
    file:       String
    preview:    Texture
    rect:       Math.Rectangle
}

struct RecentView
{
    using wnd: Wnd

    main:       *MainWnd
    items:      ArrayPtr'RecentItem
    hotItem:    uint = Swag.UInt.Max
}

impl RecentView
{
    mtd select(idx: uint)
    {
        if idx >= items.count return
        capture := catch Capture.load(items[idx].file)
        if capture
        {
            if main.getCapture()
                main.getCapture().save()
            main.editWnd.editView.setCapture(capture)
            invalidate()
        }
    }
}

impl IWnd for RecentView
{
    mtd onMouseEvent(evt: *MouseEvent)
    {
        pos := surfaceToLocal(evt.surfacePos)

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
            select(hotItem)
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
            h := clientRect.height
            w := h * (16.0 / 9)

            rect := Math.Rectangle{x, clientRect.y, w, h}
            i.rect = rect

            painter.fillRoundRect(rect, 8, 8, colors.wnd_Bk)
            defer x = rect.right() + 8

            // Hot
            if idx == hotItem
                painter.drawRoundRect(rect, 8, 8, colors.hilightLight, 2)
            // Selected
            if main.getCapture() and i.file == main.getCapture().file
                painter.drawRoundRect(rect, 8, 8, colors.hilightLight, 4)

            if !i.preview.isValid()
                continue

            txtRect := rect
            txtRect.inflate(-4)

            ratio := Math.min(rect.width / i.preview.width, rect.height / i.preview.height)
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
    }
}

struct RecentWnd
{
    using scrollWnd: ScrollWnd
    recentView: *RecentView
}

impl RecentWnd
{
    func create(parent: *MainWnd)->*RecentWnd
    {
        recent := Wnd.create'RecentWnd(parent, @{0, 0, 0, 128})
        recent.backgroundStyle = .Window

        recent.recentView = recent.createView'RecentView()
        recent.recentView.padding = 8
        recent.recentView.main = parent
        recent.view.backgroundStyle = .View

        res := catch Directory.enumerateFiles(Library.getFolder(), "*.captme")
        if @err return recent
        res.sort(@(a, b) {
            return a.creationTime <=> b.creationTime
        })

        cptRecent := 0
        visit r: res
        {
            capture := catch Capture.load(r.fullname)
            if @err continue
            cptRecent += 1
            if cptRecent > MaxRecent
                break
            recent.addRecent(capture.file, capture.preview)
            Memory.delete(capture)
        }

        return recent
    }

    mtd updateRecent(capture: *Capture)
    {
        if recentView.items.count == 0
            return
        capture.computePreview()
        getApp().getRenderer().removeTexture(&recentView.items[0].preview)
        recentView.items[0].preview = getApp().getRenderer().addImage(capture.preview)
        invalidate()
    }

    mtd addRecent(file: String, preview: Image)
    {
        item := recentView.items.newPtr()
        item.preview = getApp().getRenderer().addImage(preview)
        item.file = file
        recentView.items.insertAt(0, item)
        invalidate()
    }
}