#global public
using Core, Pixel

interface IWnd
{
    onEvent:            func(self, *Event)
    onCreateEvent:      func(self, *CreateEvent)
    onDestroyEvent:     func(self, *DestroyEvent)
    onResizeEvent:      func(self, *ResizeEvent)
    onPaintEvent:       func(self, *PaintEvent)
    onKeyEvent:         func(self, *KeyEvent)
    onMouseEvent:       func(self, *MouseEvent)
    onSysCommandEvent:  func(self, *SysCommandEvent)
}

struct Wnd
{
    itf:        IWnd
    surface:    *Surface
    style:      Style

    position:   Math.Rectangle

    parent:     *Wnd
    childs:     Array'(*Wnd)

    name: String

    drawBackground:     bool
    isDisabled:         bool
    isHidden:           bool
    isAllocated:        bool
    isPendingDestroy:   bool

    hookOnEvent:            func(*Wnd, *Event)
    hookOnCreateEvent:      func(*Wnd, *CreateEvent)
    hookOnDestroyEvent:     func(*Wnd, *DestroyEvent)
    hookOnResizeEvent:      func(*Wnd, *ResizeEvent)
    hookOnPaintEvent:       func(*Wnd, *PaintEvent)
    hookOnKeyEvent:         func(*Wnd, *KeyEvent)
    hookOnMouseEvent:       func(*Wnd, *MouseEvent)
    hookOnSysCommandEvent:  func(*Wnd, *SysCommandEvent)
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

        case Destroy:
            if hookOnDestroyEvent
                hookOnDestroyEvent(self, cast(*DestroyEvent) evt)
            else
                itf.onDestroyEvent(cast(*DestroyEvent) evt)

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

        case SysCommand:
            if hookOnSysCommandEvent
                hookOnSysCommandEvent(self, cast(*SysCommandEvent) evt)
            else
                itf.onSysCommandEvent(cast(*SysCommandEvent) evt)

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

    mtd onCreateEvent(evt: *CreateEvent)            {}
    mtd onDestroyEvent(evt: *DestroyEvent)          {}
    mtd onResizeEvent(evt: *ResizeEvent)            {}
    mtd onPaintEvent(evt: *PaintEvent)              {}
    mtd onSysCommandEvent(evt: *SysCommandEvent)    {}

    mtd onKeyEvent(evt: *KeyEvent)          { evt.accepted = false; }
    mtd onMouseEvent(evt: *MouseEvent)      { evt.accepted = false; }
}

impl Wnd
{
    //////////////////////////////////////////////
    // Setup
    //////////////////////////////////////////////

    func(T) create(parent: *Wnd, name: string, position: Math.Rectangle)->*T
    {
        Debug.assert(parent != null)

        res := Memory.new'T()
        res.name = name
        res.position = position
        res.isAllocated = true

        res.itf = cast(IWnd) dref res
        res.surface = parent.surface
        res.parent = parent
        parent.childs.add(res)

        return res
    }

    #[Swag.Inline]
    {
        mtd getApp()   => surface.app
        mtd getTheme() => &surface.app.theme
    }

    // Destroy the window
    mtd destroy()
    {
        if isPendingDestroy
            return
        isPendingDestroy = true
        surface.app.postDestroy.add(self)
        visit c: childs
            c.destroy()
    }

    //////////////////////////////////////////////
    // State
    //////////////////////////////////////////////

    // Show the window
    mtd show()
    {
        if !isHidden
            return
        isHidden = false
        invalidate()
    }

    // Hide the window
    mtd hide()
    {
        if isHidden
            return
        isHidden = true
        invalidate()
    }

    //////////////////////////////////////////////
    // Paint
    //////////////////////////////////////////////

    mtd paint(bc: *PaintContext)
    {
        // Clipping
        bc.painter.pushClippingRect(position)
        defer bc.painter.popClippingRect()
        if bc.painter.isEmptyClippingRect()
            return

        // Recompute style if necessary
        if parent
            style.compute(&parent.style)
        else
            style.compute(&bc.theme.style)
        bc.palette = style.ref.palette

        // Transmit disabled state to childs
        lastDisabled := bc.isDisabled
        if isDisabled
            bc.isDisabled = true

        // Background
        if drawBackground
            bc.painter.fillRect(position, bc.palette.windowBk)

        // User paint
        evt := PaintEvent{}
        evt.bc = bc
        sendEvent(&evt)

        // Childs
        bc.painter.translateTransform(position.x, position.y)
        visit c: self.childs
        {
            if c.isHidden
                continue

            curState := bc.painter.curState
            c.paint(bc)
            bc.painter.setState(curState)
        }

        bc.isDisabled = lastDisabled
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

    // Move and size the wnd
    mtd setRect(rect: Math.Rectangle)
    {
        move(rect.x, rect.y)
        resize(rect.width, rect.height)
    }

    // Move the wnd
    mtd move(x, y: f32)
    {
        evt := MoveEvent{}
        evt.oldPos = @{position.x, position.y}
        position.x = x
        position.y = y
        sendEvent(&evt)
    }

    // Resize the wnd
    mtd resize(width, height: f32)
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
        surface.app.captureMouse(self)
    }

    mtd releaseMouse()
    {
        surface.app.releaseMouse()
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

    mtd sendResizeEvent()
    {
        evt := ResizeEvent{}
        evt.oldSize.x = position.width
        evt.oldSize.y = position.height
        sendEvent(&evt)
    }

    mtd processEvent(evt: *Event)
    {
        if hookOnEvent
            hookOnEvent(self, evt)
        else
            itf.onEvent(evt)
    }
}