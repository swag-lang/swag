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
    form:       ListForm = Square
    lines:      Array'(*ListLine)
    lineHeight: f32 = 20
    isVirtual:  bool
}

struct ListView
{
    using wnd: Wnd
}

impl IWnd for List
{
    mtd onPaintEvent(evt: *PaintEvent)
    {
        paintBackBorders(evt.bc)
        ScrollWnd.IWnd.onPaintEvent(self, evt)
    }
}

impl IWnd for ListView
{
    mtd onPaintEvent(evt: *PaintEvent)
    {
        list := cast(*List) parent

        y := 0'f32
        font := getTheme().res.fontDefault.regular
        visit line: list.lines
        {
            evt.bc.painter.drawString(0, y, "aaa", font, Argb.White)
            y += 20
        }
    }
}

private impl List
{
    mtd paintBackBorders(bc: *PaintContext)
    {
        painter := bc.painter
        theme   := getTheme()
        colors  := getThemeColors()
        rects   := getThemeRects()

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

impl List
{
    #[Swag.Discardable]
    func createSimple(parent: *Wnd, position: Math.Rectangle, id: string = null)->*List
    {
        list     := Wnd.create'List(parent, "", position, id)
        listView := list.createView'ListView()
        listView.backgroundStyle = .Transparent

        list.sendCreateEvent()
        return list
    }

    mtd computeLayout()
    {
        clientRect   := getClientRect()
        scrollWidth  := clientRect.width
        scrollHeight := cast(f32) lines.count * lineHeight
        setScrollSize(scrollWidth, scrollHeight)
        invalidate()
    }

    mtd createLine()->*ListLine
    {
        Debug.assert(!isVirtual)
        line := Memory.new'ListLine()
        lines.add(line)
        return line
    }

    // Set the editbox form
    mtd setForm(form: ListForm)
    {
        self.form = #self.form

        #[Swag.Complete]
        switch self.form
        {
        case Square:
            padding = getThemeMetrics().list_SquarePadding
        case Round:
            padding = getThemeMetrics().list_RoundPadding
        case Flat:
            padding = getThemeMetrics().list_FlatPadding
        }

        invalidate()
    }
}