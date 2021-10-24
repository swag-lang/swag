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

    colorBackground: WndColor

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

    mtd onCreateEvent(evt: *CreateEvent)    {}
    mtd onCloseEvent(evt: *CloseEvent)      {}
    mtd onResizeEvent(evt: *ResizeEvent)    {}
    mtd onKeyEvent(evt: *KeyEvent)          {}
    mtd onMouseEvent(evt: *MouseEvent)      {}
    mtd onPaintEvent(evt: *PaintEvent)      {}
}

impl Wnd
{
    func(T) create(parent: *Wnd)->*T
    {
        Debug.assert(parent != null)
        res := Memory.new'T()

        res.itf = cast(IWnd) dref res
        res.surface = parent.surface
        res.parent = parent
        parent.childs.add(&res.wnd)

        res.colorBackground.themeColor = &res.getTheme().colorWndBackground

        evt := CreateEvent{}
        res.sendEvent(&evt)

        return res
    }

    #[Swag.Inline]
    {
        mtd getApp()   => surface.app
        mtd getTheme() => &surface.app.theme
    }

    mtd paint(bc: *PaintContext)
    {
        // Background
        bc.painter.fillRect(position, colorBackground.getColor())

        // User paint
        evt := PaintEvent{}
        evt.painter = bc.painter
        evt.renderer = bc.renderer
        sendEvent(&evt)

        // Childs
        visit c: self.childs
        {
            c.paint(bc)
        }
    }

    mtd move(x, y: s32)
    {
        evt := MoveEvent{}
        evt.oldPos = @{position.x, position.y}
        position.x = x
        position.y = y
        sendEvent(&evt)
    }

    mtd resize(width, height: s32)
    {
        evt := ResizeEvent{}
        evt.oldSize = @{position.width, position.height}
        position.width = width
        position.height = height
        sendEvent(&evt)
    }

    mtd getWndAt(x, y: f32)->*Wnd
    {
        if !position.contains(x, y)
            return null

        visit c: childs
        {
            res := c.getWndAt(x, y)
            if res
                return res
        }

        return self
    }

    mtd postQuitEvent()
    {
        surface.app.postQuitEvent()
    }

    mtd postEvent(evt: *Event)
    {
        evt.target = self
        surface.app.postEvent(evt)
    }

    mtd sendEvent(evt: *Event)
    {
        evt.target = self
        surface.app.sendEvent(evt)
    }

    mtd processEvent(evt: *Event)
    {
        if hookOnEvent
            hookOnEvent(self, evt)
        else
            itf.onEvent(evt)
    }
}