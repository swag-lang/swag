#global public
using Core, Pixel

alias WndId = string

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
    onPrePaintEvent:    func(self, *PaintEvent)
    onPaintEvent:       func(self, *PaintEvent)
    onPostPaintEvent:   func(self, *PaintEvent)
    onKeyEvent:         func(self, *KeyEvent)
    onMouseEvent:       func(self, *MouseEvent)
    onSysCommandEvent:  func(self, *SysCommandEvent)
    onSetThemeEvent:    func(self, *SetThemeEvent)
    onTimerEvent:       func(self, *TimerEvent)
    onFocusEvent:       func(self, *FocusEvent)
    onCommandEvent:     func(self, *CommandEvent)
}

#[Swag.EnumFlags]
enum WndFlags
{
    None
    NoScroll
    ClipChildren
    Disabled
    Hidden
    PreChildsPaint
    PostChildsPaint
}

struct Wnd
{
    itf:        IWnd
    type:       typeinfo
    surface:    *Surface
    style:      ThemeStyle
    wndFlags    = WndFlags.ClipChildren

    position:       Math.Rectangle
    minSize:        Math.Point
    maxSize:        Math.Point
    scrollPos:      Math.Point

    parent:     *Wnd
    childs:     Array'(*Wnd)

    name:       RichString
    id:         WndId
    cursor:     Cursor

    margin:     Math.Vector4
    padding:    Math.Vector4
    dockStyle:  DockStyle = None

    focusStrategy: FocusStategy = MousePressed

    backgroundStyle:  BackgroundStyle = Transparent
    isAllocated:      bool
    isPendingDestroy: bool
    createEventDone:  bool

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
        case PrePaint:
            itf.onPrePaintEvent(cast(*PaintEvent) evt)
        case Paint:
            itf.onPaintEvent(cast(*PaintEvent) evt)
        case PostPaint:
            itf.onPostPaintEvent(cast(*PaintEvent) evt)
        case SysCommand:
            itf.onSysCommandEvent(cast(*SysCommandEvent) evt)
        case KeyPressed, KeyReleased, Rune:
            itf.onKeyEvent(cast(*KeyEvent) evt)
        case MousePressed, MouseReleased, MouseMove, MouseEnter, MouseLeave, MouseDoubleClick, MouseWheel:
            itf.onMouseEvent(cast(*MouseEvent) evt)
        case SetTheme:
            itf.onSetThemeEvent(cast(*SetThemeEvent) evt)
        case Timer:
            itf.onTimerEvent(cast(*TimerEvent) evt)
        case SetFocus, KillFocus:
            itf.onFocusEvent(cast(*FocusEvent) evt)
        case Command:
            itf.onCommandEvent(cast(*CommandEvent) evt)
        }
    }

    mtd onCreateEvent(evt: *CreateEvent)            {}
    mtd onDestroyEvent(evt: *DestroyEvent)          {}
    mtd onResizeEvent(evt: *ResizeEvent)            {}
    mtd onPrePaintEvent(evt: *PaintEvent)           {}
    mtd onPaintEvent(evt: *PaintEvent)              {}
    mtd onPostPaintEvent(evt: *PaintEvent)          {}
    mtd onSysCommandEvent(evt: *SysCommandEvent)    {}
    mtd onSetThemeEvent(evt: *SetThemeEvent)        {}
    mtd onTimerEvent(evt: *TimerEvent)              {}

    mtd onFocusEvent(evt: *FocusEvent)      { evt.accepted = false; }
    mtd onKeyEvent(evt: *KeyEvent)          { evt.accepted = false; }
    mtd onMouseEvent(evt: *MouseEvent)      { evt.accepted = false; }
    mtd onCommandEvent(evt: *CommandEvent)  { evt.accepted = false; }
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
        mtd getTiming()         => &surface.app.timing
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
        res.type = T
        res.name = name
        res.id = id
        res.position = position
        res.isAllocated = true
        if parent
        {
            res.setParent(parent)
            evt := CreateEvent{}
            res.sendEvent(&evt)
            res.createEventDone = true
            res.sendResizeEvent()
        }

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
        Debug.assert(who != null)
        if who == parent
            return

        Debug.assert(!parent)
        parent = who
        if who
        {
            surface = who.surface
            who.childs.add(self)
            who.invalidate()
        }

        computeStyle()

        if !createEventDone
        {
            createEventDone = true
            evt := CreateEvent{}
            sendEvent(&evt)
        }
    }

    //////////////////////////////////////////////
    // State
    //////////////////////////////////////////////

    // Show the window
    mtd show()
    {
        if !(wndFlags & .Hidden)
            return
        wndFlags &= ~.Hidden
        invalidate()
    }

    // Hide the window
    mtd hide()
    {
        if wndFlags & .Hidden
            return
        wndFlags |= .Hidden
        getApp().unregisterWnd(self)
        invalidate()
    }

    // Enable the window
    mtd enable()
    {
        if !(wndFlags & .Disabled)
            return
        wndFlags &= ~.Disabled
        invalidate()
    }

    // Disable the window
    mtd disable()
    {
        if wndFlags & .Disabled
            return
        wndFlags |= .Disabled

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
            if check.wndFlags & .Disabled
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
        if bc.painter.isEmptyClippingRect()
            return

        // Recompute style if necessary
        computeStyle()

        // Transmit disabled state to childs
        lastDisabled := bc.isDisabled
        if wndFlags & .Disabled
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

        bc.painter.translateTransform(position.x, position.y)
        curTrs := bc.painter.getTransform()
        evt := PaintEvent{}
        evt.bc = bc

        // Pre paint. Clipping is parent. No scroll
        if wndFlags & .PreChildsPaint
        {
            evt.kind = .PrePaint
            sendEvent(&evt)
        }

        // Clipping
        if wndFlags & .ClipChildren
            bc.painter.pushClippingRect(getClientRect())

        // Maint
        if !(wndFlags & .NoScroll)
            bc.painter.translateTransform(-scrollPos.x, -scrollPos.y)
        evt.kind = .Paint
        sendEvent(&evt)
        bc.painter.setTransform(curTrs)

        // Childs
        if !bc.painter.isEmptyClippingRect()
        {
            visit c: childs
            {
                if c.wndFlags & .Hidden
                    continue

                curState := bc.painter.curState
                if !(c.wndFlags & .NoScroll)
                    bc.painter.translateTransform(-scrollPos.x, -scrollPos.y)
                c.paint(bc)
                bc.painter.setState(curState)
            }
        }

        // Restore clipping
        if wndFlags & .ClipChildren
            bc.painter.popClippingRect()

        // Post paint. Clipping is parent. No scroll
        if wndFlags & .PostChildsPaint
        {
            bc.painter.setTransform(curTrs)
            evt.kind = .PostPaint
            sendEvent(&evt)
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

    // Set margin for childs
    // If a value is Swag.F32.Inf, then it won't be changed
    mtd setMargin(left, top, right, bottom: f32 = Swag.F32.Inf)
    {
        with margin
        {
            if left   != Swag.F32.Inf x = left
            if top    != Swag.F32.Inf y = top
            if right  != Swag.F32.Inf z = right
            if bottom != Swag.F32.Inf w = bottom
        }

        if parent
            parent.sendResizeEvent()
    }

    mtd setAllMargins(value: f32)          = setMargin(value, value, value, value)
    mtd setAllMargins(value: Math.Vector4) = setMargin(value.x, value.y, value.z, value.w)

    // Set padding for childs
    // If a value is Swag.F32.Inf, then it won't be changed
    mtd setPadding(left, top, right, bottom: f32 = Swag.F32.Inf)
    {
        with padding
        {
            if left   != Swag.F32.Inf x = left
            if top    != Swag.F32.Inf y = top
            if right  != Swag.F32.Inf z = right
            if bottom != Swag.F32.Inf w = bottom
        }

        sendResizeEvent()
    }

    mtd setAllPaddings(value: f32)          = setPadding(value, value, value, value)
    mtd setAllPaddings(value: Math.Vector4) = setPadding(value.x, value.y, value.z, value.w)

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

    // Move and size the wnd
    mtd setPosition(rect: Math.Rectangle)
    {
        move(rect.x, rect.y)
        resize(rect.width, rect.height)
    }

    // Move and size the wnd
    mtd setPosition(x, y, w, h: f32)
    {
        move(x, y)
        resize(w, h)
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
        clientRect := getClientRectPadding()

        //centerLayout := Array.createTemp'(*Wnd)()
        var centerLayout: Array'(*Wnd){allocator: @getcontext().tempAllocator}
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
            pos.applyPadding(c.margin)
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
            if c.wndFlags & .Hidden and !getHidden
                continue
            if c.wndFlags & .Disabled and !getDisabled
                continue


            pt := Math.Point{x - position.x, y - position.y}
            if !(c.wndFlags & .NoScroll)
                pt += scrollPos
            res := c.getWndAt(pt.x, pt.y)
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

    // Retrieve the child with the given id
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

    // Retrieve the parent with the given id
    mtd getParentById(parentId: string)->*Wnd
    {
        p := parent
        while p
        {
            if p.id == parentId
                return p
            p = p.parent
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

    mtd getMouseCapture()->*Wnd
    {
        return surface.app.getMouseCapture()
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
            surface.app.keybFocusWnd.invalidate()
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

    mtd postCommandEvent(cmdId: WndId)
    {
        if cmdId == null
            return
        evt := Event.create'CommandEvent()
        evt.id = cmdId
        evt.source = self
        postEvent(evt)
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