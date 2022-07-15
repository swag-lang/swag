struct RecentItem
{
    file:    String
    preview: Texture
}

struct RecentView
{
    using wnd: Wnd

    items: ArrayPtr'RecentItem
}

impl IWnd for RecentView
{
    mtd onPaintEvent(evt: *PaintEvent)
    {
        painter    := evt.bc.painter
        clientRect := getClientRectPadding()
        colors     := getThemeColors()

        painter.setParams(.Antialiased)
        x := clientRect.x
        visit i: items
        {
            h := clientRect.height
            w := h * (16.0 / 9)

            rect := Math.Rectangle{x, clientRect.y, w, h}
            painter.fillRoundRect(rect, 8, 8, colors.wnd_Bk)
            rect.inflate(-4)

            txtRect := rect

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
            x = rect.right() + padding.x
        }
    }
}

impl RecentView
{
    mtd updateRecent(capture: *Capture)
    {
        if items.count == 0
            return
        capture.computePreview()
        getApp().getRenderer().removeTexture(&items[0].preview)
        items[0].preview = getApp().getRenderer().addImage(capture.preview)
        invalidate()
    }

    mtd addRecent(file: String, preview: Image)
    {
        item := items.newPtr()
        item.preview = getApp().getRenderer().addImage(preview)
        item.file = file
        items.insertAt(0, item)
        invalidate()
    }
}

struct RecentWnd
{
    using scrollWnd: ScrollWnd
    recentView: *RecentView
}

impl RecentWnd
{
    func create(parent: *Wnd)->*RecentWnd
    {
        recent := Wnd.create'RecentWnd(parent, @{0, 0, 0, 128})
        recent.backgroundStyle = .Window

        recent.recentView = recent.createView'RecentView()
        recent.recentView.padding = 8
        recent.view.backgroundStyle = .View

        return recent
    }
}