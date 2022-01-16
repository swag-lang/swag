#global public
using Core, Pixel

struct ListLine
{
    items: Array'(*ListItem)
}

struct ListItem
{
    name:   RichString
    icon:   Icon
}

enum ListForm
{
    Square
    Round
    Flat
}

struct List
{
    using scrollWnd:  ScrollWnd
    form: ListForm = Square
}

impl IWnd for List
{
    mtd onPaintEvent(evt: *PaintEvent)
    {
        bc     := evt.bc
        painter := bc.painter
        theme  := getTheme()
        colors := getThemeColors()
        rects  := getThemeRects()

        var subRectBk:      *ThemeImageRect
        var subRectBorder:  *ThemeImageRect
        var colorBk:        Color = ?
        var colorBorder:    Color = ?

        switch form
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
        if form != .Flat
            theme.drawSubRect(painter, clientRect, subRectBorder, colorBorder)
    }
}

private impl List
{
}

impl List
{
    #[Swag.Discardable]
    func createSimple(parent: *Wnd, position: Math.Rectangle, id: string = null)->*List
    {
        res := Wnd.create'List(parent, null, position, id)
        res.sendCreateEvent()
        return res
    }

    // Set the editbox form
    mtd setForm(form: ListForm)
    {
        self.form = #self.form

        #[Swag.Complete]
        switch self.form
        {
        case Square:
            padding = getThemeMetrics().editBox_SquarePadding
        case Round:
            padding = getThemeMetrics().editBox_RoundPadding
        case Flat:
            padding = getThemeMetrics().editBox_FlatPadding
        }

        invalidate()
    }
}