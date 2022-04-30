using Core, Gui, Pixel

struct EditWnd
{
    using scrollWnd: ScrollWnd
    editView:        *EditView
}

struct EditView
{
    using wnd:  Wnd
    scroll:     *ScrollWnd
    capture:    *Capture
    zoom:       f32 = 1
}

impl EditWnd
{
    func create(parent: *Wnd)->*EditWnd
    {
        scroll := Wnd.create'EditWnd(parent)
        view   := Wnd.create'EditView(scroll)
        view.scroll = scroll
        scroll.editView = view
        scroll.view = view

        view.capture = Memory.new'Capture()
        view.capture.width = 400
        view.capture.height = 400
        scroll.setScrollSize(500, 500)
        return scroll
    }
}

impl IWnd for EditView
{
    mtd onPaintEvent(evt: *PaintEvent)
    {
        painter := evt.bc.painter

        // Background
        rect := getClientRect()
        rect.offset(scrollPos)
        painter.fillRect(rect, getThemeColors().scrollBar_Bk)
        if !capture
            return

        // Capture
        var captureRect: Math.Rectangle
        rect = scroll.getClientScrollRect()
        captureRect.width  = capture.width * zoom
        captureRect.height = capture.height * zoom
        captureRect.x = rect.horzCenter() - captureRect.width * 0.5
        captureRect.y = rect.vertCenter() - captureRect.height * 0.5
        capture.paint(painter, captureRect)
    }
}