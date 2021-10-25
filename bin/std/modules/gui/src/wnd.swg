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

struct Wnd
{
    itf:        IWnd
    surface:    *Surface

    position:   Math.Rectangle

    parent:     *Wnd
    childs:     Array'(*Wnd)

    name: String

    drawBackground: bool
    isDisabled:     bool
    isHidden:       bool

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

        case MousePressed, MouseReleased, MouseMove, MouseEnter, MouseLeave:
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
    //////////////////////////////////////////////
    // Setup
    //////////////////////////////////////////////

    func(T) create(parent: *Wnd)->*T
    {
        Debug.assert(parent != null)
        res := Memory.new'T()

        res.itf = cast(IWnd) dref res
        res.surface = parent.surface
        res.parent = parent
        parent.childs.add(&res.wnd)

        evt := CreateEvent{}
        res.sendEvent(&evt)

        return res
    }

    #[Swag.Inline]
    {
        mtd getApp()        => surface.app
        mtd getTheme()      => &surface.app.theme
        mtd getPalette()    => &surface.app.theme.palette
    }

    //////////////////////////////////////////////
    // Paint
    //////////////////////////////////////////////

    mtd paint(bc: *PaintContext)
    {
        // Transmit disabled state to childs
        if isDisabled
            bc.isDisabled = true

        // Background
        if drawBackground
            bc.painter.fillRect(position, getPalette().windowBk)

        // User paint
        evt := PaintEvent{}
        evt.bc = bc
        sendEvent(&evt)

        // Childs
        visit c: self.childs
        {
            if c.isHidden
                continue
            c.paint(bc)
        }
    }

    mtd invalidate()
    {
        surface.invalidateRect(getSurfaceRect())
    }

    //////////////////////////////////////////////
    // Coordinates
    //////////////////////////////////////////////

    // Convert a screen coordinate to a surface coordinate (relative to my surface)
    mtd screenToSurface(pos: Math.Point)->Math.Point
    {
        return @{pos.x - surface.position.x, pos.y - surface.position.y}
    }

    // Convert a surface coordinate to a local coordinate (relative to me)
    mtd surfaceToLocal(pos: Math.Point)->Math.Point
    {
        r := getSurfaceRect()
        return @{pos.x - r.x, pos.y - r.y}
    }

    // Returns the client area
    mtd getClientRect()->Math.Rectangle
    {
        return @{0, 0, position.width, position.height}
    }

    // Get the wnd position in the surface
    mtd getSurfaceRect()->Math.Rectangle
    {
        var result: retval = position
        p := parent
        while p
        {
            result.x += p.position.x
            result.y += p.position.y
            p = p.parent
        }

        return result
    }

    // Move the wnd
    mtd move(x, y: s32)
    {
        evt := MoveEvent{}
        evt.oldPos = @{position.x, position.y}
        position.x = x
        position.y = y
        sendEvent(&evt)
    }

    // Resize the wnd
    mtd resize(width, height: s32)
    {
        evt := ResizeEvent{}
        evt.oldSize = @{position.width, position.height}
        position.width = width
        position.height = height
        sendEvent(&evt)
    }

    // Get the child window at the given coordinate
    // Coordinate is expressed in the parent system
    mtd getWndAt(x, y: f32, getDisabled, getHidden = false)->*Wnd
    {
        if !position.contains(x, y)
            return null

        visit c: childs
        {
            if c.isHidden and !getHidden
                continue
            if c.isDisabled and !getDisabled
                continue

            res := c.getWndAt(x - position.x, y - position.y)
            if res
                return res
        }

        return self
    }

    //////////////////////////////////////////////
    // Helpers
    //////////////////////////////////////////////

    mtd captureMouse()
    {
        surface.app.mouseCaptureWnd = self
    }

    mtd releaseMouse()
    {
        surface.app.mouseCaptureWnd = null
    }

    //////////////////////////////////////////////
    // Events
    //////////////////////////////////////////////

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