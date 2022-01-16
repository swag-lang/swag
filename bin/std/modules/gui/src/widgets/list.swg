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

struct List
{
    using frameWnd: FrameWnd
    lines:          Array'(*ListLine)
    lineHeight:     f32 = 20
    isVirtual:      bool
}

struct ListView
{
    using wnd:  Wnd
    list:       *List
}

impl IWnd for ListView
{
    mtd onPaintEvent(evt: *PaintEvent)
    {
        y := 0'f32
        font := getTheme().res.fontDefault.regular
        visit line: list.lines
        {
            evt.bc.painter.drawString(0, y, "Zaa", font, Argb.White)
            y += 20
        }
    }
}

impl List
{
    #[Swag.Discardable]
    func createSimple(parent: *Wnd, position: Math.Rectangle, id: string = null)->*List
    {
        list := Wnd.create'List(parent, "", position, id)

        scrollWnd := list.createView'ScrollWnd()
        scrollWnd.dockStyle = .Center
        scrollWnd.focusStrategy = .None
        scrollWnd.backgroundStyle = .Transparent

        listView := scrollWnd.createView'ListView()
        listView.backgroundStyle = .Transparent
        listView.list = list

        return list
    }

    mtd computeLayout()
    {
        scrollWnd    := cast(*ScrollWnd) view
        clientRect   := scrollWnd.getClientRect()
        scrollWidth  := clientRect.width
        scrollHeight := cast(f32) lines.count * lineHeight
        scrollWnd.setScrollSize(scrollWidth, scrollHeight)
        invalidate()
    }

    mtd createLine()->*ListLine
    {
        Debug.assert(!isVirtual)
        line := Memory.new'ListLine()
        lines.add(line)
        return line
    }
}