#global public
using Core, Pixel

interface IWnd
{
    onEvent:        func(self, *Event)
    onCreateEvent:  func(self, *CreateEvent)
    onCloseEvent:   func(self, *CloseEvent)
    onResizeEvent:  func(self, *ResizeEvent)
    onPaintEvent:   func(self, *PaintEvent)
    onKeyEvent:     func(self, *KeyEvent)
    onMouseEvent:   func(self, *MouseEvent)
}

struct PaintContext
{
    painter:    *Painter
    renderer:   *RenderOgl
}

struct Wnd
{
    itf:        IWnd
    surface:    *Surface
    position:   Math.Rectangle
    parent:     *Wnd
    childs:     Array'(*Wnd)

    hookOnEvent:        func(*Wnd, *Event)
    hookOnCreateEvent:  func(*Wnd, *CreateEvent)
    hookOnCloseEvent:   func(*Wnd, *CloseEvent)
    hookOnResizeEvent:  func(*Wnd, *ResizeEvent)
    hookOnPaintEvent:   func(*Wnd, *PaintEvent)
    hookOnKeyEvent:     func(*Wnd, *KeyEvent)
    hookOnMouseEvent:   func(*Wnd, *MouseEvent)
}

impl IWnd for Wnd
{
    mtd onEvent(evt: *Event)
    {
        switch evt.kind
        {
        case Create:
            if hookOnCreateEvent
                hookOnCreateEvent(self, cast(*CreateEvent) evt)
            else
                itf.onCreateEvent(cast(*CreateEvent) evt)

        case Close:
            if hookOnCloseEvent
                hookOnCloseEvent(self, cast(*CloseEvent) evt)
            else
                itf.onCloseEvent(cast(*CloseEvent) evt)

        case Resize:
            if hookOnResizeEvent
                hookOnResizeEvent(self, cast(*ResizeEvent) evt)
            else
                itf.onResizeEvent(cast(*ResizeEvent) evt)

        case Paint:
            if hookOnPaintEvent
                hookOnPaintEvent(self, cast(*PaintEvent) evt)
            else
                itf.onPaintEvent(cast(*PaintEvent) evt)

        case KeyPressed, KeyReleased:
            if hookOnKeyEvent
                hookOnKeyEvent(self, cast(*KeyEvent) evt)
            else
                itf.onKeyEvent(cast(*KeyEvent) evt)

        case MousePressed, MouseReleased, MouseMove:
            if hookOnMouseEvent
                hookOnMouseEvent(self, cast(*MouseEvent) evt)
            else
                itf.onMouseEvent(cast(*MouseEvent) evt)
        }
    }

    mtd onCreateEvent(evt: *CreateEvent)
    {
    }

    mtd onCloseEvent(evt: *CloseEvent)
    {
    }

    mtd onResizeEvent(evt: *ResizeEvent)
    {
        position.width = evt.rect.width
        position.height = evt.rect.height
    }

    mtd onPaintEvent(evt: *PaintEvent)
    {
        evt.painter.fillRect(self.position, Color.fromArgb(0xFFEEEEEE))
    }

    mtd onKeyEvent(evt: *KeyEvent)
    {
    }

    mtd onMouseEvent(evt: *MouseEvent)
    {
        //Console.print(evt.surfacePos)
    }
}

impl Wnd
{
    mtd paint(bc: *PaintContext)
    {
        evt := PaintEvent{kind: EventKind.Paint}
        evt.target = self
        evt.painter = bc.painter
        evt.renderer = bc.renderer
        surface.app.sendEvent(&evt)

        visit c: self.childs
            c.paint(bc)
    }

    func(T) create(parent: *Wnd)->*T
    {
        Debug.assert(parent != null)

        res := Memory.new'T()
        res.itf = cast(IWnd) dref res
        res.surface = parent.surface
        res.parent = parent
        parent.childs.add(&res.wnd)

        evt := CreateEvent{kind: EventKind.Create}
        evt.target = cast(*Wnd) res
        parent.surface.app.sendEvent(&evt)

        return res
    }

    mtd postQuitEvent()
    {
        surface.app.postQuitEvent()
    }

    mtd sendEvent(event: *Event)
    {
        if hookOnEvent
            hookOnEvent(self, event)
        else
            itf.onEvent(event)
    }
}