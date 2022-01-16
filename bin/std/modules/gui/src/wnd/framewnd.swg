#global public
using Core, Pixel

enum FrameWndForm
{
    Square
    Round
    Flat
    Transparent
}

struct FrameWnd
{
    using wnd:  Wnd
    view:       *Wnd
    frameForm:  FrameWndForm = Transparent
}

impl IWnd for FrameWnd
{
    mtd onPaintEvent(evt: *PaintEvent)
    {
        if frameForm == .Transparent
            return

        bc      := evt.bc
        painter := bc.painter
        theme   := getTheme()
        colors  := getThemeColors()
        rects   := getThemeRects()

        var subRectBk:      *ThemeImageRect
        var subRectBorder:  *ThemeImageRect
        var colorBk:        Color = ?
        var colorBorder:    Color = ?

        switch frameForm
        {
        case Square:
            subRectBk = &rects.list_SquareBk
            subRectBorder = &rects.list_SquareBorder
        case Round:
            subRectBk = &rects.list_RoundBk
            subRectBorder = &rects.list_RoundBorder
        case Flat:
            subRectBk = &rects.list_FlatBk
        }

        switch
        {
        case bc.isDisabled:
            colorBorder = colors.list_DisabledBorder
            colorBk = colors.list_DisabledBk
        case hasFocus():
            colorBorder = colors.list_FocusBorder
            colorBk = colors.list_FocusBk
        default:
            colorBorder = colors.list_Border
            colorBk = colors.list_Bk
        }

        clientRect := getClientRect()
        theme.drawSubRect(painter, clientRect, subRectBk, colorBk)
        if frameForm != .Flat
            theme.drawSubRect(painter, clientRect, subRectBorder, colorBorder)
    }

    mtd onResizeEvent(evt: *ResizeEvent)
    {
        if !view
            return
        view.move(padding.x, padding.y)
        view.resize(position.width - padding.x - padding.z, position.height - padding.y - padding.w)
    }
}

impl FrameWnd
{
    #[Swag.Discardable]
    mtd(T) createView()->*T
    {
        view = Wnd.create'T(self, "", position)
        view.backgroundStyle = .Window
        sendResizeEvent()
        return cast(*T) view
    }

    // Set the editbox form
    mtd setFrameForm(form: FrameWndForm)
    {
        self.frameForm = #self.form

        #[Swag.Complete]
        switch self.frameForm
        {
        case Square:
            padding = getThemeMetrics().list_SquarePadding
        case Round:
            padding = getThemeMetrics().list_RoundPadding
        case Flat:
            padding = getThemeMetrics().list_FlatPadding
        case Transparent:
            padding = @{0, 0, 0, 0}
        }

        invalidate()
    }
}