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
    Transparent
    Window
    Dialog
    DialogBar
}

enum FocusStategy
{
    None
    MousePressed
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
    onFocusEvent:       func(self, *FocusEvent)
}

struct Wnd
{
    itf:        IWnd
    surface:    *Surface
    style:      Style

    position:   Math.Rectangle
    minSize:    Math.Point
    maxSize:    Math.Point
    scrollPos:  Math.Point

    parent:     *Wnd
    childs:     Array'(*Wnd)

    name:       RichString
    id:         WndId
    cursor:     Cursor

    margin:     Math.Vector4
    padding:    Math.Vector4
    dockStyle:  DockStyle = None

    focusStrategy: FocusStategy = MousePressed

    backgroundStyle:    BackgroundStyle = Transparent
    isDisabled:         bool
    isHidden:           bool
    isAllocated:        bool
    isPendingDestroy:   bool

    hookOnEvent: func(*Wnd, *Event)->bool
}

impl IWnd for Wnd
{
    mtd onEvent(evt: *Event)
    {
        eat := false
        switch evt.kind
        {
        case Create:
            itf.onCreateEvent(cast(*CreateEvent) evt)
        case Destroy:
            itf.onDestroyEvent(cast(*DestroyEvent) evt)
        case Resize:
            itf.onResizeEvent(cast(*ResizeEvent) evt)
        case Paint:
            itf.onPaintEvent(cast(*PaintEvent) evt)
        case SysCommand:
            itf.onSysCommandEvent(cast(*SysCommandEvent) evt)
        case KeyPressed, KeyReleased, Char:
            itf.onKeyEvent(cast(*KeyEvent) evt)
        case MousePressed, MouseReleased, MouseMove, MouseEnter, MouseLeave, MouseDoubleClick, MouseWheel:
            itf.onMouseEvent(cast(*MouseEvent) evt)
        case SetTheme:
            itf.onSetThemeEvent(cast(*SetThemeEvent) evt)
        case Timer:
            itf.onTimerEvent(cast(*TimerEvent) evt)
        case SetFocus, KillFocus:
            itf.onFocusEvent(cast(*FocusEvent) evt)
        }
    }

    mtd onCreateEvent(evt: *CreateEvent)            {}
    mtd onDestroyEvent(evt: *DestroyEvent)          {}
    mtd onResizeEvent(evt: *ResizeEvent)            {}
    mtd onPaintEvent(evt: *PaintEvent)              {}
    mtd onSysCommandEvent(evt: *SysCommandEvent)    {}
    mtd onSetThemeEvent(evt: *SetThemeEvent)        {}
    mtd onTimerEvent(evt: *TimerEvent)              {}

    mtd onFocusEvent(evt: *FocusEvent)      { evt.accepted = false; }
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
        mtd getTopView()        => surface.wnd.view
        mtd getTheme()          => style.ref.theme
        mtd getThemeMetrics()   => &style.ref.theme.metrics
        mtd getThemeColors()    => &style.ref.theme.colors
        mtd getThemeRects()     => &style.ref.theme.rects
    }

    #[Swag.Inline]
    func(T) create(parent: *Wnd = null, id: WndId = null)->*T
    {
        return create(parent, "", @{})
    }

    #[Swag.Inline]
    func(T) create(parent: *Wnd, position: Math.Rectangle, id: WndId = null)->*T
    {
        return create(parent, "", position)
    }

    func(T) create(parent: *Wnd, name: string, position: Math.Rectangle, id: WndId = null)->*T
    {
        res := Memory.new'T()
        res.isAllocated = true
        res.itf = cast(IWnd) dref res
        res.name = name
        res.id = id
        res.position = position
        res.isAllocated = true
        res.setParent(parent)
        return res
    }

    // Destroy the window
    mtd destroy()
    {
        if isPendingDestroy
            return
        isPendingDestroy = true
        visit c: childs
            c.destroy()
        surface.app.postDestroy.add(self)
        invalidate()
    }

    // Set window parent
    mtd setParent(who: *Wnd)
    {
        Debug.assert(!parent)
        parent = who
        if who
        {
            surface = who.surface
            who.childs.add(self)
            who.invalidate()
        }

        computeStyle()
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
        getApp().unregisterWnd(self)
        invalidate()
    }

    // Enable the window
    mtd enable()
    {
        if !isDisabled
            return
        isDisabled = false
        invalidate()
    }

    // Disable the window
    mtd disable()
    {
        if isDisabled
            return
        isDisabled = true

        // Check global state
        if getApp().keybFocusWnd == self
            getTopView().setFocus()

        invalidate()
    }

    // Returns true if window, and all its parents, are enabled
    mtd isEnabled()->bool
    {
        check := self
        while check
        {
            if check.isDisabled
                return false
            check = check.parent
        }

        return true
    }

    //////////////////////////////////////////////
    // Paint
    //////////////////////////////////////////////

    mtd computeStyle()
    {
        if surface
            style.compute(self, &surface.app.style)
    }

    mtd paint(bc: *PaintContext)
    {
        // Clipping
        bc.painter.pushClippingRect(position)
        defer bc.painter.popClippingRect()
        if bc.painter.isEmptyClippingRect()
            return

        // Recompute style if necessary
        computeStyle()

        // Transmit disabled state to childs
        lastDisabled := bc.isDisabled
        if isDisabled
            bc.isDisabled = true

        // Background
        theme := getTheme()
        switch backgroundStyle
        {
        case Window:
            bc.painter.fillRect(position, theme.colors.wnd_Bk)
        case Dialog:
            bc.painter.fillRect(position, theme.colors.dlg_Bk)
        case DialogBar:
            theme.drawSubRect(bc.painter, position, &theme.rects.dlg_BtnBar, theme.colors.dlg_BtnBarBk)
        }

        // User paint
        bc.painter.translateTransform(position.x, position.y)
        evt := PaintEvent{}
        evt.bc = bc
        sendEvent(&evt)

        //
        bc.painter.translateTransform(-scrollPos.x, -scrollPos.y)

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

    // Returns the client area, with 'padding' applied
    mtd getClientRectPadding()->Math.Rectangle
    {
        var result: retval = getClientRect()
        result.applyPadding(padding)
        return result
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
            result.x -= p.scrollPos.x
            result.y -= p.scrollPos.y
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

    // Force the window to be the last painted (on top of siblings)
    mtd bringToTop()
    {
        Debug.assert(parent != null)
        if parent.childs.back() == self
            return
        (idx, found) := Slice.findLinear(parent.childs.toSlice(), self)
        Debug.assert(found)
        parent.childs[idx] = parent.childs.back()
        parent.childs[parent.childs.count - 1] = self
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

    private mtd clampSize(pos: *Math.Rectangle)
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

        // Top level window is the last one painted, so it's the last
        // in the array.
        // So we must visit in revert order
        for i := cast(s32) childs.count - 1; i >= 0; i -= 1
        {
            c := childs[i]
            if c.isHidden and !getHidden
                continue
            if c.isDisabled and !getDisabled
                continue

            res := c.getWndAt(x - position.x + scrollPos.x, y - position.y + scrollPos.y)
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

    // Retreive the child with the given id
    mtd getChildById(childId: string)->*Wnd
    {
        visit c: childs
        {
            if c.id == childId
                return c
            res := c.getChildById(childId)
            if res
                return res
        }

        return null
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

    // Set the keyboard focus
    mtd setFocus()
    {
        if surface.app.keybFocusWnd == self
            return
        if !isEnabled()
            return

        if surface.app.keybFocusWnd
        {
            evt := FocusEvent{kind: EventKind.KillFocus}
            evt.other = self
            surface.app.keybFocusWnd.sendEvent(&evt)
        }

        evt := FocusEvent{kind: EventKind.SetFocus}
        evt.other = surface.app.keybFocusWnd
        surface.app.keybFocusWnd = self
        sendEvent(&evt)
    }

    // Get the window with the keyboard focus
    mtd getFocus()->*Wnd
    {
        return surface.app.keybFocusWnd
    }

    // Returns true if the window has the keyboard focus
    mtd hasFocus() => surface.app.keybFocusWnd == self

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

    mtd sendCreateEvent()
    {
        evt := CreateEvent{}
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