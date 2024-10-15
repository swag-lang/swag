#global public
using Core, Pixel

alias WndId     = string
alias HookEvent = func||(*Wnd, IEvent)->bool

// Automatic placement and size of the window in the parent client rectangle
enum DockStyle
{
    None
    Top                    // Stack to the top, keeping the vertical size
    Left                   // Stack to the left, keeping the horizontal size
    Right                  // Stack to the right, keeping the horizontal size
    Bottom                 // Stack to the bottom, keeping the vertical size
    Center                 // Fill the remaining parent rect
    CenterKeepSizeVert     // Fill the remaining parent rect horizontally, but keep the vertical size
}

#[Swag.EnumFlags]
enum AnchorStyle
{
    None
    X
    Y
    Width
    Height
}

enum BackgroundStyle
{
    Transparent
    Window
    Dialog
    DialogBar
    View
}

enum FocusStategy
{
    None
    MousePressed
}

struct KeyShortcut
{
    mdf:        Input.KeyModifiers = Zero
    key:        Input.Key = None
    id:         WndId
    target:     *Wnd
}

interface IWnd
{
    mtd onEvent(evt: IEvent);
    mtd onHookEvent(evt: IEvent);

    mtd onCreateEvent(evt: *CreateEvent);
    mtd onDestroyEvent(evt: *DestroyEvent);
    mtd onStateEvent(evt: *StateEvent);
    mtd onResizeEvent(evt: *ResizeEvent);
    mtd onPrePaintEvent(evt: *PaintEvent);
    mtd onPaintEvent(evt: *PaintEvent);
    mtd onPostPaintEvent(evt: *PaintEvent);
    mtd onKeyEvent(evt: *KeyEvent);
    mtd onMouseEvent(evt: *MouseEvent);
    mtd onSysCommandEvent(evt: *SysCommandEvent);
    mtd onSysUserEvent(evt: *SysUserEvent);
    mtd onSetThemeEvent(evt: *SetThemeEvent);
    mtd onTimerEvent(evt: *TimerEvent);
    mtd onFocusEvent(evt: *FocusEvent);
    mtd onCommandEvent(evt: *CommandEvent);
    mtd onComputeStateEvent(evt: *CommandStateEvent);
    mtd onApplyStateEvent(evt: *CommandStateEvent);
    mtd onFrameEvent(evt: *FrameEvent);
    mtd onSerializeStateEvent(evt: *SerializeStateEvent);
    mtd onNotifyEvent(evt: *NotifyEvent);
}

#[Swag.EnumFlags]
enum WndFlags
{
    Zero            = 0
    NoScroll
    ClipChildren
    Disabled
    Hidden
    PreChildsPaint
    PostChildsPaint
    TopMost
}

struct Wnd
{
    itf:                   IWnd
    type:                  typeinfo
    surface:               *Surface
    style:                 ThemeStyle
    wndFlags               = WndFlags.ClipChildren
    userData:              *void

    position:              Math.Rectangle
    minSize:               Math.Point
    maxSize:               Math.Point
    scrollPos:             Math.Point

    invalidatePadding:     Math.Vector4

    parent:                *Wnd
    owner:                 *Wnd
    childs:                Array'(*Wnd)

    name:                  RichString
    id:                    WndId
    cursor:                Cursor
    toolTip:               String

    keyShortcuts:          Array'KeyShortcut
    actions:               Array'IActionUI

    margin:                Math.Vector4
    padding:               Math.Vector4
    dockStyle:             DockStyle = None
    anchorStyle:           AnchorStyle = None

    focusStrategy:         FocusStategy = MousePressed

    backgroundStyle:       BackgroundStyle = Transparent
    isAllocated:           bool
    isPendingDestroy:      bool
    createEventDone:       bool

    hookOnEvent:           HookEvent                         // Will be called before processing the event of the window
    hookOnChildEvent:      HookEvent                         // Will be called before processing whatever event of a child window
}

impl IWnd for Wnd
{
    mtd impl onEvent(evt: IEvent)
    {
        switch evt
        {
        case CreateEvent as ptr:
            itf.onCreateEvent(ptr)
        case DestroyEvent as ptr:
            itf.onDestroyEvent(ptr)
        case StateEvent as ptr:
            itf.onStateEvent(ptr)
        case ResizeEvent as ptr:
            itf.onResizeEvent(ptr)
        case PaintEvent as ptr where ptr.kind == .PrePaint:
            itf.onPrePaintEvent(ptr)
        case PaintEvent as ptr where ptr.kind == .Paint:
            itf.onPaintEvent(ptr)
        case PaintEvent as ptr where ptr.kind == .PostPaint:
            itf.onPostPaintEvent(ptr)
        case SysCommandEvent as ptr:
            itf.onSysCommandEvent(ptr)
        case SysUserEvent as ptr:
            itf.onSysUserEvent(ptr)
        case KeyEvent as ptr:
            itf.onKeyEvent(ptr)
        case MouseEvent as ptr where ptr.kind != .MouseAccept:
            itf.onMouseEvent(ptr)
        case SetThemeEvent as ptr:
            itf.onSetThemeEvent(ptr)
        case TimerEvent as ptr:
            itf.onTimerEvent(ptr)
        case FocusEvent as ptr:
            itf.onFocusEvent(ptr)
        case CommandEvent as ptr:
            if !executeAction(ptr):
                itf.onCommandEvent(ptr)
        case CommandStateEvent as ptr where ptr.kind == .ComputeCommandState:
            if !updateAction(ptr):
                itf.onComputeStateEvent(ptr)
        case CommandStateEvent as ptr where ptr.kind == .ApplyCommandState:
            itf.onApplyStateEvent(cast(*CommandStateEvent) evt)
        case FrameEvent as ptr:
            itf.onFrameEvent(ptr)
        case SerializeStateEvent as ptr:
            itf.onSerializeStateEvent(ptr)
        case NotifyEvent as ptr:
            itf.onNotifyEvent(ptr)
        }
    }

    mtd impl onHookEvent(evt: IEvent) {}

    mtd impl onCreateEvent(evt: *CreateEvent) {}
    mtd impl onDestroyEvent(evt: *DestroyEvent) {}
    mtd impl onStateEvent(evt: *StateEvent) {}
    mtd impl onResizeEvent(evt: *ResizeEvent) {}
    mtd impl onPrePaintEvent(evt: *PaintEvent) {}
    mtd impl onPaintEvent(evt: *PaintEvent) {}
    mtd impl onPostPaintEvent(evt: *PaintEvent) {}
    mtd impl onSetThemeEvent(evt: *SetThemeEvent) { invalidate(); sendResizeEvent(); }
    mtd impl onTimerEvent(evt: *TimerEvent) {}
    mtd impl onSerializeStateEvent(evt: *SerializeStateEvent) {}
    mtd impl onFrameEvent(evt: *FrameEvent) {}

    mtd impl onSysCommandEvent(evt: *SysCommandEvent) { evt.accepted = false; }
    mtd impl onSysUserEvent(evt: *SysUserEvent) { evt.accepted = false; }
    mtd impl onKeyEvent(evt: *KeyEvent) { evt.accepted = false; }
    mtd impl onMouseEvent(evt: *MouseEvent) { evt.accepted = false; }
    mtd impl onFocusEvent(evt: *FocusEvent) { evt.accepted = false; }
    mtd impl onCommandEvent(evt: *CommandEvent) { evt.accepted = false; }
    mtd impl onComputeStateEvent(evt: *CommandStateEvent) { evt.accepted = false; }
    mtd impl onApplyStateEvent(evt: *CommandStateEvent) { evt.accepted = false; }
    mtd impl onNotifyEvent(evt: *NotifyEvent) { evt.accepted = false; }
}

impl Wnd
{
    //////////////////////////////////////////////
    // Setup
    //////////////////////////////////////////////

    #[Swag.Inline]
    {
        mtd getApp()          => surface.app
        mtd getTopView()      => surface.wnd.view
        mtd getTopWnd()       => surface.wnd
        mtd getTheme()        => style.refr.theme
        mtd getThemeMetrics() => &style.refr.theme.metrics
        mtd getThemeColors()  => &style.refr.theme.colors
        mtd getThemeRects()   => &style.refr.theme.rects
        mtd getTiming()       => &surface.app.timing
        mtd getFont()         => style.getFont()
    }

    #[Swag.Inline, Swag.Overload]
    func(T) create(parent: *Wnd = null, id: WndId = null, hook: HookEvent = null)->*T
    {
        return create(parent, "", {}, id: id, hook: hook)
    }

    #[Swag.Inline, Swag.Overload]
    func(T) create(parent: nl *Wnd, position: Math.Rectangle, id: WndId = null, hook: HookEvent = null)->*T
    {
        return create(parent, "", position, id: id, hook: hook)
    }

    #[Swag.Overload]
    func(T) create(parent: nl *Wnd, name: nl string, position: Math.Rectangle, id: WndId = null, hook: HookEvent = null)->*T
    {
        let res = Memory.new'T()
        res.isAllocated = true
        res.itf         = cast(IWnd) dref res
        res.type        = T
        res.name        = name
        res.id          = id
        res.position    = position
        res.isAllocated = true
        res.hookOnEvent = hook

        if parent
        {
            res.setParent(parent)
            res.sendCreateEvent()
            res.sendResizeEvent()
        }

        return res
    }

    // Destroy the window
    mtd destroyNow()
    {
        Debug.assert(!isPendingDestroy)
        foreach c in childs:
            c.destroyNow()
        surface.app.destroyWnd(self)
    }

    // Destroy the window
    mtd destroy()
    {
        wndFlags.add(.Hidden)
        if isPendingDestroy:
            return
        isPendingDestroy = true
        foreach c in childs:
            c.destroy()
        surface.app.postDestroy.add(self)
        invalidate()

        if parent and !parent.isPendingDestroy:
            parent.applyLayout()
    }

    internal mtd firstTopMostIndex()->u64
    {
        foreach c, i in childs
        {
            if c.wndFlags.has(.TopMost):
                return i
        }

        return Swag.U64.Max
    }

    // Set window parent
    mtd setParent(who: *Wnd)
    {
        Debug.assert(who != null)
        if who == parent:
            return

        Debug.assert(!parent)
        parent = who
        if who
        {
            surface = who.surface

            // Add the window at the back, or before the first topmost window
            if !who.childs.count:
                who.childs.add(self)
            else
            {
                var idxToInsert = who.childs.count
                if !(wndFlags & .TopMost)
                {
                    idxToInsert = who.firstTopMostIndex()
                    if idxToInsert == Swag.U64.Max:
                        idxToInsert = who.childs.count
                }

                who.childs.insertAt(idxToInsert, self)
            }

            who.invalidate()
        }

        computeStyle()
        sendCreateEvent()
    }

    // Get the owner of the window
    mtd getOwner()->*Wnd
    {
        return owner orelse parent
    }

    //////////////////////////////////////////////
    // State
    //////////////////////////////////////////////

    // Show the window
    mtd show(b = true)
    {
        if !b
        {
            hide()
            return
        }

        if !wndFlags.has(.Hidden):
            return
        wndFlags.remove(.Hidden)
        parent.invalidate()
    }

    // Hide the window
    mtd hide()
    {
        if wndFlags.has(.Hidden):
            return
        wndFlags.add(.Hidden)
        getApp().unregisterWnd(self)
        parent.invalidate()
    }

    // Enable/Disable the window
    mtd enable(state = true)
    {
        if state
        {
            if !wndFlags.has(.Disabled):
                return
            wndFlags.remove(.Disabled)
        }
        else
        {
            if wndFlags.has(.Disabled):
                return
            wndFlags.add(.Disabled)

            // Check global state
            if getApp().keybFocusWnd == self:
                getTopView().setFocus()
        }

        invalidate()
    }

    // Disable the window
    mtd disable()
    {
        enable(false)
    }

    // Returns true if window and all its parents are visible
    mtd isVisible(onlyMe = false)->bool
    {
        var toCheck = self
        while toCheck
        {
            if toCheck.wndFlags.has(.Hidden):
                return false
            if onlyMe:
                return true
            toCheck = toCheck.parent
        }

        return true
    }

    // Returns true if window, or one of its parent, is hidden
    mtd isHidden()->bool => !isVisible()

    // Returns true if the window has the visible state.
    // Will return true even if one of the parent is hidden.
    mtd isVisibleState()->bool => !wndFlags.has(.Hidden)

    // Returns true if window, and all its parents, are enabled
    mtd isEnabled(onlyMe = false)->bool
    {
        var toCheck = self
        while toCheck
        {
            if toCheck.wndFlags.has(.Disabled):
                return false
            if onlyMe:
                return true
            toCheck = toCheck.parent
        }

        return true
    }

    // Returns true if window, or one of its parent, is disabled
    mtd isDisabled()->bool => !isEnabled()

    // Returns true if the window has the enabled state.
    // Will return true even if one of the parent is hidden.
    mtd isEnabledState()->bool => !wndFlags.has(.Hidden)

    //////////////////////////////////////////////
    // Paint
    //////////////////////////////////////////////

    mtd computeStyle()
    {
        if surface:
            style.compute(self, &surface.app.style)
    }

    mtd paint(bc: *PaintContext)
    {
        let painter = bc.painter
        if painter.isEmptyClippingRect():
            return

        // Recompute style if necessary
        computeStyle()

        // Transmit disabled state to childs
        let lastDisabled = bc.isDisabled
        if wndFlags.has(.Disabled):
            bc.isDisabled = true

        // Background
        let theme = getTheme()
        switch backgroundStyle
        {
        case Window:
            painter.fillRect(position, theme.colors.wnd_Bk)
        case Dialog:
            painter.fillRect(position, theme.colors.dlg_Bk)
        case View:
            painter.fillRect(position, theme.colors.view_Bk)
        case DialogBar:
            theme.drawSubRect(bc.painter, position, &theme.rects.dlg_BtnBar, theme.colors.dlg_BtnBarBk)
        }

        painter.translateTransform(position.x, position.y)
        var curTrs = painter.getTransform()
        var evt    = PaintEvent{}
        evt.bc = bc

        // Pre paint. Clipping is parent. No scroll
        if wndFlags.has(.PreChildsPaint)
        {
            evt.kind = .PrePaint
            painter.pushState()
            sendEvent(&evt)
            painter.popState()
        }

        // Clipping
        if wndFlags.has(.ClipChildren):
            painter.pushClippingRect(getClientRect())

        if !painter.isEmptyClippingRect()
        {
            // Me
            if !wndFlags.has(.NoScroll):
                painter.translateTransform(-scrollPos.x, -scrollPos.y)
            evt.kind = .Paint
            painter.pushState()
            sendEvent(&evt)
            painter.popState()
            painter.setTransform(curTrs)

            // Childs
            foreach c in childs
            {
                if c.wndFlags.has(.Hidden):
                    continue

                painter.pushState()
                if !c.wndFlags.has(.NoScroll):
                    painter.translateTransform(-scrollPos.x, -scrollPos.y)
                c.paint(bc)
                painter.popState()
            }
        }

        // Restore clipping
        if wndFlags.has(.ClipChildren):
            painter.popClippingRect()

        // Post paint. Clipping is parent. No scroll
        if wndFlags.has(.PostChildsPaint)
        {
            painter.setTransform(curTrs)
            evt.kind = .PostPaint
            painter.pushState()
            sendEvent(&evt)
            painter.popState()
        }

        bc.isDisabled = lastDisabled
        style.dirty   = false
    }

    // Force the window to be repainted
    mtd invalidate()
    {
        var rect = getSurfaceRect()
        rect.applyPadding(invalidatePadding)
        rect.applyPadding({-32, -32, -32, -32})
        if surface:
            surface.invalidateRect(rect)
    }

    // Force o local position to be repainted
    mtd invalidateRect(rect: Math.Rectangle)
    {
        surface.invalidateRect(localToSurface(rect))
    }

    //////////////////////////////////////////////
    // Coordinates
    //////////////////////////////////////////////

    // Be sure rect is fully inside the parent rectangle (if possible)
    mtd fitPosInParent(marginW, marginH: f32 = 0, simX, simY = false)
    {
        var pos = position
        if pos.x + pos.width > parent.position.width - marginW
        {
            if simX:
                pos.x -= (pos.width + marginH)
            else:
                pos.x = parent.position.width - pos.width - marginW
        }

        if pos.y + pos.height > parent.position.height - marginH
        {
            if simY:
                pos.y -= (pos.height + marginH)
            else:
                pos.y = parent.position.height - pos.height - marginH
        }

        if pos != position:
            setPosition(pos)
    }

    // Convert a screen coordinate to a surface coordinate (relative to my surface)
    mtd screenToSurface(pos: Math.Point)->Math.Point
    {
        return {pos.x - surface.position.x, pos.y - surface.position.y}
    }

    // Convert a surface coordinate to a local coordinate (relative to me)
    mtd surfaceToLocal(pos: Math.Point)->Math.Point
    {
        var r = getSurfaceRect()
        return {pos.x - r.x, pos.y - r.y}
    }

    // Convert a local coordinate to a surface coordinate
    #[Swag.Overload]
    mtd localToSurface(pos: Math.Point)->Math.Point
    {
        var r = getSurfaceRect()
        return {pos.x + r.x, pos.y + r.y}
    }

    // Convert a local coordinate to a surface coordinate
    #[Swag.Overload]
    mtd localToSurface(pos: Math.Rectangle)->Math.Rectangle
    {
        var r = getSurfaceRect()
        return {pos.x + r.x, pos.y + r.y, pos.width, pos.height}
    }

    // Returns the client area
    mtd getClientRect()->Math.Rectangle
    {
        return {0, 0, position.width, position.height}
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
        var p       = parent
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

    // Get the wnd position relative to a given parent
    mtd getRectIn(wnd: *Wnd)->Math.Rectangle
    {
        var result: retval = position
        var p       = parent
        while p and p != wnd
        {
            result.x += p.position.x
            result.y += p.position.y
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
            if left != Swag.F32.Inf:
                .x = left
            if top != Swag.F32.Inf:
                .y = top
            if right != Swag.F32.Inf:
                .z = right
            if bottom != Swag.F32.Inf:
                .w = bottom
        }

        if parent:
            parent.sendResizeEvent()
    }

    #[Swag.Overload]
    mtd setAllMargins(value: f32) = setMargin(value, value, value, value)
    #[Swag.Overload]
    mtd setAllMargins(value: Math.Vector4) = setMargin(value.x, value.y, value.z, value.w)

    // Set padding for childs
    // If a value is Swag.F32.Inf, then it won't be changed
    mtd setPadding(left, top, right, bottom: f32 = Swag.F32.Inf)
    {
        with padding
        {
            if left != Swag.F32.Inf:
                .x = left
            if top != Swag.F32.Inf:
                .y = top
            if right != Swag.F32.Inf:
                .z = right
            if bottom != Swag.F32.Inf:
                .w = bottom
        }

        sendResizeEvent()
    }

    #[Swag.Overload]
    mtd setAllPaddings(value: f32) = setPadding(value, value, value, value)
    #[Swag.Overload]
    mtd setAllPaddings(value: Math.Vector4) = setPadding(value.x, value.y, value.z, value.w)

    // Force the window to be the first painted (before all siblings)
    mtd bringToFront()
    {
        Debug.assert(parent != null)
        if parent.childs.front() == self:
            return

        let idx = Slice.findLinear(parent.childs.toSlice(), self)
        Debug.assert(idx != Swag.U64.Max)

        parent.childs.removeAtOrdered(idx)
        parent.childs.insertAt(0, self)
    }

    // Force the window to be the last painted (on top of siblings)
    mtd bringToTop()
    {
        Debug.assert(parent != null)
        if parent.childs.back() == self:
            return

        let idx = Slice.findLinear(parent.childs.toSlice(), self)
        Debug.assert(idx != Swag.U64.Max)

        // Put the window at the back, or before the first topmost window
        var idxToReplace = parent.childs.count - 1
        if !wndFlags.has(.TopMost)
        {
            idxToReplace = parent.firstTopMostIndex()
            if idxToReplace == Swag.U64.Max:
                return
            idxToReplace -= 1
        }

        swap(parent.childs.buffer + idx, parent.childs.buffer + idxToReplace)
    }

    // Move and size the wnd
    #[Swag.Overload]
    mtd setPosition(rect: Math.Rectangle, computeLayout = true)
    {
        move(rect.x, rect.y)
        resize(rect.width, rect.height, computeLayout)
    }

    // Move and size the wnd
    #[Swag.Overload]
    mtd setPosition(x, y, w, h: f32, computeLayout = true)
    {
        move(x, y)
        resize(w, h, computeLayout)
    }

    // Move the wnd
    mtd move(x, y: f32)
    {
        if position.x == x and position.y == y:
            return
        var evt = MoveEvent{}
        evt.oldPos = {position.x, position.y}
        position.x = Math.round(x)
        position.y = Math.round(y)
        sendEvent(&evt)
    }

    internal mtd clampSize(pos: *Math.Rectangle)
    {
        if minSize.x > 0:
            pos.width = Math.max(pos.width, minSize.x)
        if minSize.y > 0:
            pos.height = Math.max(pos.height, minSize.y)
        if maxSize.x > 0:
            pos.width = Math.min(pos.width, maxSize.x)
        if maxSize.y > 0:
            pos.height = Math.min(pos.height, maxSize.y)
    }

    // Resize the wnd
    mtd resize(width, height: f32, computeLayout = true)
    {
        if position.width == width and position.height == height:
            return

        var evt = ResizeEvent{}
        evt.oldSize     = {position.width, position.height}
        position.width  = Math.round(width)
        position.height = Math.round(height)
        clampSize(&position)

        sendEvent(&evt)

        if computeLayout:
            applyLayout(evt.oldSize)
    }

    // Apply the current layout (childs constraints)
    #[Swag.Overload]
    mtd applyLayout()
    {
        applyLayout({position.width, position.height})
    }

    #[Swag.Overload]
    mtd applyLayout(oldSize: Math.Point)
    {
        var clientRect = getClientRectPadding()

        var centerLayout = Array.createTemp'(*Wnd)()
        var oldSizes     = Array.createTemp'Math.Point()

        foreach c in childs
        {
            if c.anchorStyle == .None:
                continue

            var pos = c.position
            if c.anchorStyle & .X and oldSize.x > 0
            {
                let oldM = oldSize.x - c.position.x
                pos.x = position.width - oldM
            }

            if c.anchorStyle & .Y and oldSize.y > 0
            {
                let oldM = oldSize.y - c.position.y
                pos.y = position.height - oldM
            }

            if c.anchorStyle & .Width and oldSize.x > 0
            {
                if position.width - oldSize.x:
                    pos.width += position.width - oldSize.x
            }

            if c.anchorStyle & .Height and oldSize.y > 0
            {
                pos.height += position.height - oldSize.y
            }

            c.setPosition(pos, false)
        }

        foreach c in childs
        {
            oldSizes.add({c.position.width, c.position.height})
            if !c.isVisibleState():
                continue
            switch c.dockStyle
            {
            case Left:
                var pos = c.position
                pos.x = clientRect.x + c.margin.x
                pos.y = clientRect.y + c.margin.y
                pos.setBottom(clientRect.bottom() - c.margin.w)
                if c.maxSize.y > 0 and c.maxSize.y < pos.height and !(c.anchorStyle & .Y):
                    pos.y = clientRect.vertCenter() - c.maxSize.y * 0.5
                c.setPosition(pos, false)
                let diff = c.margin.x + pos.width + c.margin.z
                clientRect.width -= diff
                clientRect.x += diff

            case Right:
                var pos = c.position
                pos.x = clientRect.right() - pos.width - c.margin.z
                pos.y = clientRect.y + c.margin.y
                pos.setBottom(clientRect.bottom() - c.margin.w)
                if c.maxSize.y > 0 and c.maxSize.y < pos.height and !(c.anchorStyle & .Y):
                    pos.y = clientRect.vertCenter() - c.maxSize.y * 0.5
                c.setPosition(pos, false)
                let diff = c.margin.x + pos.width + c.margin.z
                clientRect.width -= diff

            case Top:
                var pos = c.position
                pos.x = clientRect.x + c.margin.x
                pos.y = clientRect.y + c.margin.y
                pos.setRight(clientRect.right() - c.margin.z)
                if c.maxSize.x > 0 and c.maxSize.x < pos.width and !(c.anchorStyle & .X):
                    pos.x = clientRect.horzCenter() - c.maxSize.x * 0.5
                c.setPosition(pos, false)
                let diff = c.margin.y + pos.height + c.margin.w
                clientRect.y += diff
                clientRect.height -= diff

            case Bottom:
                var pos = c.position
                pos.x = clientRect.x + c.margin.x
                pos.y = clientRect.bottom() - c.margin.w - pos.height
                pos.setRight(clientRect.right() - c.margin.z)
                if c.maxSize.x > 0 and c.maxSize.x < pos.width and !(c.anchorStyle & .X):
                    pos.x = clientRect.horzCenter() - c.maxSize.x * 0.5
                c.setPosition(pos, false)
                let diff = c.margin.w + pos.height + c.margin.y
                clientRect.height -= diff

            case Center, CenterKeepSizeVert:
                centerLayout.add(c)
            }
        }

        foreach c in centerLayout
        {
            var pos = clientRect

            // Fill the remaining room horizontally, but keep vertical size and center
            if c.dockStyle == .CenterKeepSizeVert
            {
                pos.y      = pos.vertCenter() - c.position.height * 0.5
                pos.height = c.position.height
            }

            // Fill the remaining room
            else
            {
                pos.applyPadding(c.margin)
            }

            c.setPosition(pos, false)
        }

        foreach c, i in childs
        {
            if !c.isVisible():
                continue
            c.applyLayout(oldSizes[i])
        }
    }

    // Get the child window at the given coordinate
    // Coordinate is expressed in the parent system
    mtd getWndAt(x, y: f32, getDisabled, getHidden = false)->nl *Wnd
    {
        if !position.contains(x, y):
            return null

        // Top level window is the last one painted, so it's the last
        // in the array.
        // So we must foreach in revert order
        for var i = cast(s32) childs.count - 1; i >= 0; i -= 1
        {
            let c = childs[i]
            if c.wndFlags.has(.Hidden) and !getHidden:
                continue
            if c.wndFlags.has(.Disabled) and !getDisabled:
                continue

            var pt = Math.Point{x - position.x, y - position.y}
            if !c.wndFlags.has(.NoScroll):
                pt += scrollPos
            let res = c.getWndAt(pt.x, pt.y)
            if res:
                return res
        }

        return self
    }

    // Returns true if 'child' is in the child hieararchy of 'self'
    mtd isParentOf(child: *Wnd)->bool
    {
        if !child:
            return false
        var p = child.parent
        while p
        {
            if p == self:
                return true
            p = p.parent
        }

        return false
    }

    // Returns true if 'child' is in the owner/child hieararchy of 'self'
    mtd isOwnerOf(child: *Wnd)->bool
    {
        if !child:
            return false
        var p = child.getOwner()
        while p
        {
            if p == self:
                return true
            p = p.getOwner()
        }

        return false
    }

    // Retrieve the child with the given id
    mtd getChildById(childId: string)->nl *Wnd
    {
        foreach c in childs
        {
            if c.id == childId:
                return c
            let res = c.getChildById(childId)
            if res:
                return res
        }

        return null
    }

    // Retrieve the parent with the given id
    mtd getParentById(parentId: string)->nl *Wnd
    {
        var p = parent
        while p
        {
            if p.id == parentId:
                return p
            p = p.parent
        }

        return null
    }

    //////////////////////////////////////////////
    // Mouse
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

    //////////////////////////////////////////////
    // Keyboard
    //////////////////////////////////////////////

    // Set the keyboard focus
    mtd setFocus()
    {
        if surface.app.keybFocusWnd == self:
            return
        if !isEnabled():
            return

        let last = surface.app.keybFocusWnd

        // Ask the window with the current focus if we can steal it
        if last
        {
            var evt = FocusEvent{kind: FocusEvent.Kind.CanKillFocus, other: self}
            last.sendEvent(&evt)
            if !evt.canChangeFocus:
                return
        }

        // Ask the new window if it wants the focus
        {
            var evt = FocusEvent{kind: FocusEvent.Kind.CanSetFocus, other: last}
            sendEvent(&evt)
            if !evt.canChangeFocus:
                return
        }

        if last
        {
            var evt = FocusEvent{kind: FocusEvent.Kind.KillFocus, other: self}
            last.sendEvent(&evt)
            last.invalidate()

            last.ownerNotifyEvent(.ChildFocusChanged)
        }

        surface.setFocus()

        var evt = FocusEvent{kind: FocusEvent.Kind.SetFocus, other: last}
        surface.app.keybFocusWnd = self
        sendEvent(&evt)
        invalidate()

        ownerNotifyEvent(.ChildFocusChanged)
    }

    // Get the window with the keyboard focus
    mtd getFocus()->*Wnd
    {
        return surface.app.keybFocusWnd
    }

    // Returns true if the window has the keyboard focus
    mtd hasFocus()->bool => surface.app.keybFocusWnd == self

    // Register a keyboard shortcut
    mtd registerKeyShortcut(mdf: Input.KeyModifiers, key: Input.Key, wndId: WndId)
    {
        foreach &c in keyShortcuts
        {
            if c.mdf == mdf and c.key == key
            {
                c.id = wndId
                return
            }
        }

        keyShortcuts.add({mdf, key, wndId})
    }

    // Get the id associated with a shortcut. null if none
    mtd getKeyShortcut(mdf: Input.KeyModifiers, key: Input.Key)->nl WndId
    {
        foreach c in keyShortcuts
        {
            if c.mdf == mdf and c.key == key:
                return c.id
        }

        let p = getOwner()
        if p:
            return p.getKeyShortcut(mdf, key)
        return null
    }

    // Get the name associated with a given id shortcut
    mtd getKeyShortcutNameFor(wndId: WndId)->String
    {
        var result: retval
        foreach c in keyShortcuts
        {
            if c.id == wndId
            {
                result = Input.getKeyName(c.mdf, c.key)
                return result
            }
        }

        let p = getOwner()
        if p:
            return p.getKeyShortcutNameFor(wndId)
        return result
    }

    //////////////////////////////////////////////
    // Events
    //////////////////////////////////////////////

    // Register a new timer for the given 'target'
    mtd addTimer(time: Time.Duration)->*Timer
    {
        return getApp().addTimer(time, self)
    }

    mtd postQuitEvent()
    {
        surface.app.postQuitEvent()
    }

    mtd postEvent(evt: IEvent)
    {
        let ptr = evt as Event
        ptr.target = self
        surface.app.postEvent(evt)
    }

    mtd postInvalidateEvent()
    {
        let evt = Event.create'InvalidateEvent()
        postEvent(evt)
    }

    mtd sendEvent(evt: IEvent)
    {
        let ptr = evt as Event
        ptr.target = self
        surface.app.sendEvent(evt)
    }

    mtd sendStateEvent(kind: StateEvent.Kind)
    {
        var evt = StateEvent{kind: kind}
        sendEvent(&evt)
    }

    mtd sendResizeEvent()
    {
        var evt = ResizeEvent{}
        evt.oldSize.x = position.width
        evt.oldSize.y = position.height
        sendEvent(&evt)
    }

    mtd postResizeEvent()
    {
        let evt = Event.create'ResizeEvent()
        evt.oldSize.x = position.width
        evt.oldSize.y = position.height
        postEvent(evt)
    }

    mtd sendCreateEvent()
    {
        if createEventDone:
            return
        createEventDone = true
        var evt = CreateEvent{}
        sendEvent(&evt)
    }

    mtd postCommandEvent(cmdId: WndId)
    {
        if cmdId == null:
            return
        let evt = Event.create'CommandEvent()
        evt.id     = cmdId
        evt.source = self
        postEvent(evt)
    }

    mtd processEvent(evt: IEvent)
    {
        var eat = false
        if hookOnEvent:
            eat = hookOnEvent(self, evt)
        if !eat:
            itf.onEvent(evt)
    }

    // Send a notification event
    mtd notifyEvent(kind: NotifyEvent.Kind)
    {
        var ntfy = NotifyEvent{}
        ntfy.ntfyKind = kind
        sendEvent(&ntfy)
    }

    // Send a notification event
    mtd ownerNotifyEvent(kind: NotifyEvent.Kind)
    {
        let p = getOwner()
        if !p:
            return
        p.notifyEvent(kind)
    }

    //////////////////////////////////////////////
    // Commands
    //////////////////////////////////////////////

    // Main function to update command state of various windows/widgets.
    // This will update the state of this window, and all of its childs
    mtd updateCommandState()
    {
        var evtState = sendComputeCommandStateEvent(id)
        if evtState.accepted
        {
            evtState.kind = .ApplyCommandState
            sendEvent(&evtState)
            return
        }

        foreach c in childs:
            c.updateCommandState()
    }

    // Send the command state event with the given id to the window.
    // Returns the updated command state.
    mtd sendComputeCommandStateEvent(cmdId: WndId)->CommandStateEvent
    {
        if cmdId == null:
            return {}

        var evtState: retval

        // User will fill the state
        evtState.kind   = .ComputeCommandState
        evtState.id     = cmdId
        evtState.source = self
        sendEvent(&evtState)

        // Is there a keyboard shortcut or a global hot key ?
        var sk = getKeyShortcutNameFor(cmdId)
        if !sk.length:
            sk = getApp().getHotKeyShortcutNameFor(cmdId)
        if sk.length
        {
            evtState.setFlags.add(.Shortcut)
            evtState.shortcut = sk
        }

        return evtState
    }

    // Send the command event with the given id to the window.
    // The command will be updated first, and if it is not disabled, it will be sent.
    mtd sendCommandEvent(cmdId: WndId)
    {
        if cmdId == null:
            return

        // We first need to be sure that the target can deal with the event
        var evtState = CommandStateEvent{}
        evtState.kind = .ComputeCommandState
        evtState.id   = cmdId
        sendEvent(&evtState)

        // If it's not disabled, then we can send
        if !evtState.setFlags.has(.Disabled) or !evtState.disabled
        {
            var evtc = CommandEvent{}
            evtc.id = cmdId
            sendEvent(&evtc)
        }
    }

    //////////////////////////////////////////////
    // Actions
    //////////////////////////////////////////////

    // Register one new action
    mtd(T) registerAction()
    {
        var itfAct = @mkinterface(null, #typeof(T), IActionUI)
        actions.add(itfAct)
    }

    internal mtd updateAction(evt: *CommandStateEvent)->bool
    {
        var cxt: ActionContext
        cxt.wnd = self
        cxt.id  = evt.id

        foreach act in actions
        {
            if act.accept(cxt)
            {
                evt.accepted = true
                if act.update(cxt, evt):
                    return true
            }
        }

        return false
    }

    internal mtd executeAction(evt: *CommandEvent)->bool
    {
        var cxt: ActionContext
        cxt.wnd = self
        cxt.id  = evt.id

        foreach act in actions
        {
            if act.accept(cxt)
            {
                evt.accepted = true
                if act.execute(cxt):
                    return true
            }
        }

        return false
    }

    //////////////////////////////////////////////
    // Serialization
    //////////////////////////////////////////////

    mtd serializeState(ser: *Serialization.Serializer) throw
    {
        var evt: SerializeStateEvent
        evt.accepted = false
        evt.ser      = ser
        evt.target   = self
        processEvent(&evt)

        if !evt.accepted
        {
            foreach c in childs:
                c.serializeState(ser)
        }
    }
}
