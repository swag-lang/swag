#global public
using Core, Pixel

enum DockStyle
{
    None
    Top
    Left
    Right
    Bottom
    Center
}

enum BackgroundStyle
{
    None
    Window
    Dialog
    DialogBar
}

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
    onSetThemeEvent:    func(self, *SetThemeEvent)
    onTimerEvent:       func(self, *TimerEvent)
}

struct Wnd
{
    itf:        IWnd
    surface:    *Surface
    style:      Style

    position:   Math.Rectangle
    minSize:    Math.Point
    maxSize:    Math.Point

    parent:     *Wnd
    childs:     Array'(*Wnd)

    name:       RichString
    id:         String
    cursor:     Cursor

    margin:     Math.Vector4
    padding:    Math.Vector4
    dockStyle:  DockStyle = None

    backgroundStyle:    BackgroundStyle = None
    isDisabled:         bool
    isHidden:           bool
    isAllocated:        bool
    isPendingDestroy:   bool

    hookOnEvent:            func(*Wnd, *Event)->bool
    hookOnCreateEvent:      func(*Wnd, *CreateEvent)->bool
    hookOnDestroyEvent:     func(*Wnd, *DestroyEvent)->bool
    hookOnResizeEvent:      func(*Wnd, *ResizeEvent)->bool
    hookOnPaintEvent:       func(*Wnd, *PaintEvent)->bool
    hookOnKeyEvent:         func(*Wnd, *KeyEvent)->bool
    hookOnMouseEvent:       func(*Wnd, *MouseEvent)->bool
    hookOnSysCommandEvent:  func(*Wnd, *SysCommandEvent)->bool
    hookOnSetThemeEvent:    func(*Wnd, *SetThemeEvent)->bool
    hookOnTimerEvent:       func(*Wnd, *TimerEvent)->bool
}

impl IWnd for Wnd
{
    mtd onEvent(evt: *Event)
    {
        eat := false
        switch evt.kind
        {
        case Create:
            if hookOnCreateEvent
                eat = hookOnCreateEvent(self, cast(*CreateEvent) evt)
            if !eat
                itf.onCreateEvent(cast(*CreateEvent) evt)

        case Destroy:
            if hookOnDestroyEvent
                eat = hookOnDestroyEvent(self, cast(*DestroyEvent) evt)
            if !eat
                itf.onDestroyEvent(cast(*DestroyEvent) evt)

        case Resize:
            if hookOnResizeEvent
                eat = hookOnResizeEvent(self, cast(*ResizeEvent) evt)
            if !eat
                itf.onResizeEvent(cast(*ResizeEvent) evt)

        case Paint:
            if hookOnPaintEvent
                eat = hookOnPaintEvent(self, cast(*PaintEvent) evt)
            if !eat
                itf.onPaintEvent(cast(*PaintEvent) evt)

        case SysCommand:
            if hookOnSysCommandEvent
                eat = hookOnSysCommandEvent(self, cast(*SysCommandEvent) evt)
            if !eat
                itf.onSysCommandEvent(cast(*SysCommandEvent) evt)

        case KeyPressed, KeyReleased:
            if hookOnKeyEvent
                eat = hookOnKeyEvent(self, cast(*KeyEvent) evt)
            if !eat
                itf.onKeyEvent(cast(*KeyEvent) evt)

        case MousePressed, MouseReleased, MouseMove, MouseEnter, MouseLeave:
            if hookOnMouseEvent
                eat = hookOnMouseEvent(self, cast(*MouseEvent) evt)
            if !eat
                itf.onMouseEvent(cast(*MouseEvent) evt)

        case SetTheme:
            if hookOnSetThemeEvent
                eat = hookOnSetThemeEvent(self, cast(*SetThemeEvent) evt)
            if !eat
                itf.onSetThemeEvent(cast(*SetThemeEvent) evt)

        case Timer:
            if hookOnTimerEvent
                eat = hookOnTimerEvent(self, cast(*TimerEvent) evt)
            if !eat
                itf.onTimerEvent(cast(*TimerEvent) evt)
        }
    }

    mtd onCreateEvent(evt: *CreateEvent)            {}
    mtd onDestroyEvent(evt: *DestroyEvent)          {}
    mtd onResizeEvent(evt: *ResizeEvent)            {}
    mtd onPaintEvent(evt: *PaintEvent)              {}
    mtd onSysCommandEvent(evt: *SysCommandEvent)    {}
    mtd onSetThemeEvent(evt: *SetThemeEvent)        {}
    mtd onTimerEvent(evt: *TimerEvent)              {}

    mtd onKeyEvent(evt: *KeyEvent)          { evt.accepted = false; }
    mtd onMouseEvent(evt: *MouseEvent)      { evt.accepted = false; }
}

impl Wnd
{
    //////////////////////////////////////////////
    // Setup
    //////////////////////////////////////////////

    #[Swag.Inline]
    {
        mtd getApp()            => surface.app
        mtd getTheme()          => surface.app.theme
        mtd getThemeMetrics()   => &surface.app.theme.metrics
        mtd getThemeColors()    => &surface.app.theme.colors
    }

    #[Swag.Inline]
    func(T) create(parent: *Wnd, id: string = null)->*T
    {
        return create(parent, "", @{})
    }

    #[Swag.Inline]
    func(T) create(parent: *Wnd, position: Math.Rectangle, id: string = null)->*T
    {
        return create(parent, "", position)
    }

    func(T) create(parent: *Wnd, name: string, position: Math.Rectangle, id: string = null)->*T
    {
        res := Memory.new'T()
        res.isAllocated = true
        res.itf = cast(IWnd) dref res
        res.name = name
        res.id = id
        res.position = position
        res.isAllocated = true
        res.parent = parent
        if parent
        {
            res.surface = parent.surface
            parent.childs.add(res)
        }

        return res
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
            style.compute(self, &parent.style)
        else
            style.compute(self, &surface.app.style)
        bc.theme = style.ref.theme

        // Transmit disabled state to childs
        lastDisabled := bc.isDisabled
        if isDisabled
            bc.isDisabled = true

        // Background
        switch backgroundStyle
        {
        case Window:
            bc.painter.fillRect(position, bc.theme.colors.wnd_Bk)
        case Dialog:
            bc.painter.fillRect(position, bc.theme.colors.dlg_Bk)
        case DialogBar:
            bc.theme.drawSubRect(bc.painter, position, &bc.theme.rects.dlg_BtnBar, bc.theme.colors.dlg_BtnBarBk)
        }

        // User paint
        bc.painter.translateTransform(position.x, position.y)
        evt := PaintEvent{}
        evt.bc = bc
        sendEvent(&evt)

        // Childs
        visit c: childs
        {
            if c.isHidden
                continue

            curState := bc.painter.curState
            c.paint(bc)
            bc.painter.setState(curState)
        }

        bc.isDisabled = lastDisabled
        style.dirty = false
    }

    // Force the window to be repainted
    mtd invalidate()
    {
        surface.invalidateRect(getSurfaceRect())
    }

    // Force o local position to be repainted
    mtd invalidateRect(rect: Math.Rectangle)
    {
        surface.invalidateRect(localToSurface(rect))
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

    // Convert a local coordinate to a surface coordinate
    mtd localToSurface(pos: Math.Point)->Math.Point
    {
        r := getSurfaceRect()
        return @{pos.x + r.x, pos.y + r.y}
    }

    // Convert a local coordinate to a surface coordinate
    mtd localToSurface(pos: Math.Rectangle)->Math.Rectangle
    {
        r := getSurfaceRect()
        return @{pos.x + r.x, pos.y + r.y, pos.width, pos.height}
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
    mtd setPosition(rect: Math.Rectangle)
    {
        move(rect.x, rect.y)
        resize(rect.width, rect.height)
    }

    // Move the wnd
    mtd move(x, y: f32)
    {
        if position.x == x and position.y == y
            return
        evt := MoveEvent{}
        evt.oldPos = @{position.x, position.y}
        position.x = x
        position.y = y
        sendEvent(&evt)
    }

    protected mtd clampSize(pos: *Math.Rectangle)
    {
        if minSize.x > 0
            pos.width = Math.max(pos.width, minSize.x)
        if minSize.y > 0
            pos.height = Math.max(pos.height, minSize.y)
        if maxSize.x > 0
            pos.width = Math.min(pos.width, maxSize.x)
        if maxSize.y > 0
            pos.height = Math.min(pos.height, maxSize.y)
    }

    // Resize the wnd
    mtd resize(width, height: f32)
    {
        if position.width == width and position.height == height
            return

        evt := ResizeEvent{}
        evt.oldSize = @{position.width, position.height}
        position.width = width
        position.height = height
        clampSize(&position)

        sendEvent(&evt)
        applyLayout()
    }

    // Apply the current layout (childs constraints)
    mtd applyLayout()
    {
        clientRect := getClientRect()
        clientRect.applyPadding(padding)

        var centerLayout: Array'(*Wnd)
        visit c: childs
        {
            switch c.dockStyle
            {
            case Left:
                pos := c.position
                pos.x = clientRect.x + c.margin.x
                pos.y = clientRect.y + c.margin.y
                pos.setBottom(clientRect.bottom() - c.margin.w)
                c.setPosition(pos)
                diff := c.margin.x + pos.width + c.margin.z
                clientRect.width -= diff
                clientRect.x += diff

            case Right:
                pos := c.position
                pos.x = clientRect.right() - pos.width - c.margin.z
                pos.y = clientRect.y + c.margin.y
                pos.setBottom(clientRect.bottom() - c.margin.w)
                c.setPosition(pos)
                diff := c.margin.x + pos.width + c.margin.z
                clientRect.width -= diff

            case Top:
                pos := c.position
                pos.x = clientRect.x + c.margin.x
                pos.y = clientRect.y + c.margin.y
                pos.setRight(clientRect.right() - c.margin.z)
                c.setPosition(pos)
                diff := c.margin.y + pos.height + c.margin.w
                clientRect.y += diff
                clientRect.height -= diff

            case Bottom:
                pos := c.position
                pos.x = clientRect.x + c.margin.x
                pos.y = clientRect.bottom() - c.margin.w - pos.height
                pos.setRight(clientRect.right() - c.margin.z)
                c.setPosition(pos)
                diff := c.margin.w + pos.height + c.margin.y
                clientRect.height -= diff

            case Center:
                centerLayout.add(c)
            }
        }

        visit c: centerLayout
        {
            pos := clientRect
            c.setPosition(pos)
        }

        visit c: childs
            c.applyLayout()
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

    // Returns true if 'child' is in the child hieararchy of 'self'
    mtd isParentOf(child: *Wnd)->bool
    {
        p := child.parent
        while p
        {
            if p == self
                return true
            p = p.parent
        }

        return false
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

    mtd captureKeyboard()
    {
        surface.app.keybCaptureWnd = self
    }

    mtd releaseKeyboard()
    {
        surface.app.keybCaptureWnd = null
    }

    mtd setFocus()
    {
        surface.app.keybFocusWnd = self
    }

    mtd getFocus()->*Wnd
    {
        return surface.app.keybFocusWnd
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
        eat := false
        if hookOnEvent
            eat = hookOnEvent(self, evt)
        if !eat
            itf.onEvent(evt)
    }
}