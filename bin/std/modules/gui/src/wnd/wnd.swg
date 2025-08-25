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
    mtd onThemeEvent(evt: *ThemeEvent);
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
            me.itf.onCreateEvent(ptr)
        case DestroyEvent as ptr:
            me.itf.onDestroyEvent(ptr)
        case StateEvent as ptr:
            me.itf.onStateEvent(ptr)
        case ResizeEvent as ptr:
            me.itf.onResizeEvent(ptr)
        case PaintEvent as ptr where ptr.kind == .PrePaint:
            me.itf.onPrePaintEvent(ptr)
        case PaintEvent as ptr where ptr.kind == .Paint:
            me.itf.onPaintEvent(ptr)
        case PaintEvent as ptr where ptr.kind == .PostPaint:
            me.itf.onPostPaintEvent(ptr)
        case SysCommandEvent as ptr:
            me.itf.onSysCommandEvent(ptr)
        case SysUserEvent as ptr:
            me.itf.onSysUserEvent(ptr)
        case KeyEvent as ptr:
            me.itf.onKeyEvent(ptr)
        case MouseEvent as ptr where ptr.kind != .MouseAccept:
            me.itf.onMouseEvent(ptr)
        case ThemeEvent as ptr:
            me.itf.onThemeEvent(ptr)
        case TimerEvent as ptr:
            me.itf.onTimerEvent(ptr)
        case FocusEvent as ptr:
            me.itf.onFocusEvent(ptr)
        case CommandEvent as ptr:
            if !me.executeAction(ptr):
                me.itf.onCommandEvent(ptr)
        case CommandStateEvent as ptr where ptr.kind == .ComputeCommandState:
            if !me.updateAction(ptr):
                me.itf.onComputeStateEvent(ptr)
        case CommandStateEvent as ptr where ptr.kind == .ApplyCommandState:
            me.itf.onApplyStateEvent(cast(*CommandStateEvent) evt)
        case FrameEvent as ptr:
            me.itf.onFrameEvent(ptr)
        case SerializeStateEvent as ptr:
            me.itf.onSerializeStateEvent(ptr)
        case NotifyEvent as ptr:
            me.itf.onNotifyEvent(ptr)
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
    mtd impl onThemeEvent(evt: *ThemeEvent) { me.invalidate(); me.sendResizeEvent(); }
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
        mtd getApp()          => me.surface.app
        mtd getTopView()      => me.surface.wnd.view
        mtd getTopWnd()       => me.surface.wnd
        mtd getTheme()        => me.style.refr.theme
        mtd getThemeMetrics() => &me.style.refr.theme.metrics
        mtd getThemeColors()  => &me.style.refr.theme.colors
        mtd getThemeRects()   => &me.style.refr.theme.rects
        mtd getTiming()       => &me.surface.app.timing
        mtd getFont()         => me.style.getFont()
    }

    #[Swag.Inline, Swag.Overload]
    func(T) create(parent: *Wnd = null, id: WndId = null, hook: HookEvent = null)->*T
    {
        return create(parent, "", {}, id: id, hook: hook)
    }

    #[Swag.Inline, Swag.Overload]
    func(T) create(parent: #null *Wnd, position: Math.Rectangle, id: WndId = null, hook: HookEvent = null)->*T
    {
        return create(parent, "", position, id: id, hook: hook)
    }

    #[Swag.Overload]
    func(T) create(parent: #null *Wnd, name: #null string, position: Math.Rectangle, id: WndId = null, hook: HookEvent = null)->*T
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
        Debug.assert(!me.isPendingDestroy)
        foreach c in me.childs:
            c.destroyNow()
        me.surface.app.destroyWnd(me)
    }

    // Destroy the window
    mtd destroy()
    {
        me.wndFlags.add(.Hidden)
        if me.isPendingDestroy:
            return
        me.isPendingDestroy = true
        foreach c in me.childs:
            c.destroy()
        me.surface.app.postDestroy.add(me)
        me.invalidate()

        if me.parent and !me.parent.isPendingDestroy:
            me.parent.applyLayout()
    }

    internal mtd firstTopMostIndex()->u64
    {
        foreach c, i in me.childs
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
        if who == me.parent:
            return

        Debug.assert(!me.parent)
        me.parent = who
        if who
        {
            me.surface = who.surface

            // Add the window at the back, or before the first topmost window
            if !who.childs.count:
                who.childs.add(me)
            else
            {
                var idxToInsert = who.childs.count
                if !(me.wndFlags & .TopMost)
                {
                    idxToInsert = who.firstTopMostIndex()
                    if idxToInsert == Swag.U64.Max:
                        idxToInsert = who.childs.count
                }

                who.childs.insertAt(idxToInsert, me)
            }

            who.invalidate()
        }

        me.computeStyle()
        me.sendCreateEvent()
    }

    // Get the owner of the window
    mtd getOwner()->*Wnd
    {
        return me.owner orelse me.parent
    }

    //////////////////////////////////////////////
    // State
    //////////////////////////////////////////////

    // Show the window
    mtd show(b = true)
    {
        if !b
        {
            me.hide()
            return
        }

        if !me.wndFlags.has(.Hidden):
            return
        me.wndFlags.remove(.Hidden)
        me.parent.invalidate()
    }

    // Hide the window
    mtd hide()
    {
        if me.wndFlags.has(.Hidden):
            return
        me.wndFlags.add(.Hidden)
        me.getApp().unregisterWnd(me)
        me.parent.invalidate()
    }

    // Enable/Disable the window
    mtd enable(state = true)
    {
        if state
        {
            if !me.wndFlags.has(.Disabled):
                return
            me.wndFlags.remove(.Disabled)
        }
        else
        {
            if me.wndFlags.has(.Disabled):
                return
            me.wndFlags.add(.Disabled)

            // Check global state
            if me.getApp().keybFocusWnd == me:
                me.getTopView().setFocus()
        }

        me.invalidate()
    }

    // Disable the window
    mtd disable()
    {
        me.enable(false)
    }

    // Returns true if window and all its parents are visible
    mtd isVisible(onlyMe = false)->bool
    {
        var toCheck = me
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
    mtd isHidden()->bool => !me.isVisible()

    // Returns true if the window has the visible state.
    // Will return true even if one of the parent is hidden.
    mtd isVisibleState()->bool => !me.wndFlags.has(.Hidden)

    // Returns true if window, and all its parents, are enabled
    mtd isEnabled(onlyMe = false)->bool
    {
        var toCheck = me
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
    mtd isDisabled()->bool => !me.isEnabled()

    // Returns true if the window has the enabled state.
    // Will return true even if one of the parent is hidden.
    mtd isEnabledState()->bool => !me.wndFlags.has(.Hidden)

    //////////////////////////////////////////////
    // Paint
    //////////////////////////////////////////////

    mtd computeStyle()
    {
        if me.surface:
            me.style.compute(me, &me.surface.app.style)
    }

    mtd paint(bc: *PaintContext)
    {
        let painter = bc.painter
        if painter.isEmptyClippingRect():
            return

        // Recompute style if necessary
        me.computeStyle()

        // Transmit disabled state to childs
        let lastDisabled = bc.isDisabled
        if me.wndFlags.has(.Disabled):
            bc.isDisabled = true

        // Background
        let theme = me.getTheme()
        switch me.backgroundStyle
        {
        case Window:
            painter.fillRect(me.position, theme.colors.wnd_Bk)
        case Dialog:
            painter.fillRect(me.position, theme.colors.dlg_Bk)
        case View:
            painter.fillRect(me.position, theme.colors.view_Bk)
        case DialogBar:
            theme.drawSubRect(bc.painter, me.position, &theme.rects.dlg_BtnBar, theme.colors.dlg_BtnBarBk)
        }

        painter.translateTransform(me.position.x, me.position.y)
        var curTrs = painter.getTransform()
        var evt    = PaintEvent{}
        evt.bc = bc

        // Pre paint. Clipping is parent. No scroll
        if me.wndFlags.has(.PreChildsPaint)
        {
            evt.kind = .PrePaint
            painter.pushState()
            me.sendEvent(&evt)
            painter.popState()
        }

        // Clipping
        if me.wndFlags.has(.ClipChildren):
            painter.pushClippingRect(me.getClientRect())

        if !painter.isEmptyClippingRect()
        {
            // Me
            if !me.wndFlags.has(.NoScroll):
                painter.translateTransform(-me.scrollPos.x, -me.scrollPos.y)
            evt.kind = .Paint
            painter.pushState()
            me.sendEvent(&evt)
            painter.popState()
            painter.setTransform(curTrs)

            // Childs
            foreach c in me.childs
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
        if me.wndFlags.has(.ClipChildren):
            painter.popClippingRect()

        // Post paint. Clipping is parent. No scroll
        if me.wndFlags.has(.PostChildsPaint)
        {
            painter.setTransform(curTrs)
            evt.kind = .PostPaint
            painter.pushState()
            me.sendEvent(&evt)
            painter.popState()
        }

        bc.isDisabled  = lastDisabled
        me.style.dirty = false
    }

    // Force the window to be repainted
    mtd invalidate()
    {
        var rect = me.getSurfaceRect()
        rect.applyPadding(me.invalidatePadding)
        rect.applyPadding({-32, -32, -32, -32})
        if me.surface:
            me.surface.invalidateRect(rect)
    }

    // Force o local position to be repainted
    mtd invalidateRect(rect: Math.Rectangle)
    {
        me.surface.invalidateRect(me.localToSurface(rect))
    }

    //////////////////////////////////////////////
    // Coordinates
    //////////////////////////////////////////////

    // Be sure rect is fully inside the parent rectangle (if possible)
    mtd fitPosInParent(marginW, marginH: f32 = 0, simX, simY = false)
    {
        var pos = me.position
        if pos.x + pos.width > me.parent.position.width - marginW
        {
            if simX:
                pos.x -= (pos.width + marginH)
            else:
                pos.x = me.parent.position.width - pos.width - marginW
        }

        if pos.y + pos.height > me.parent.position.height - marginH
        {
            if simY:
                pos.y -= (pos.height + marginH)
            else:
                pos.y = me.parent.position.height - pos.height - marginH
        }

        if pos != me.position:
            me.setPosition(pos)
    }

    // Convert a screen coordinate to a surface coordinate (relative to my surface)
    mtd const screenToSurface(pos: Math.Point)->Math.Point
    {
        return {pos.x - me.surface.position.x, pos.y - me.surface.position.y}
    }

    // Convert a surface coordinate to a local coordinate (relative to me)
    mtd const surfaceToLocal(pos: Math.Point)->Math.Point
    {
        var r = me.getSurfaceRect()
        return {pos.x - r.x, pos.y - r.y}
    }

    // Convert a local coordinate to a surface coordinate
    #[Swag.Overload]
    mtd const localToSurface(pos: Math.Point)->Math.Point
    {
        var r = me.getSurfaceRect()
        return {pos.x + r.x, pos.y + r.y}
    }

    // Convert a local coordinate to a surface coordinate
    #[Swag.Overload]
    mtd const localToSurface(pos: Math.Rectangle)->Math.Rectangle
    {
        var r = me.getSurfaceRect()
        return {pos.x + r.x, pos.y + r.y, pos.width, pos.height}
    }

    // Returns the client area
    mtd const getClientRect()->Math.Rectangle
    {
        return {0, 0, me.position.width, me.position.height}
    }

    // Returns the client area, with 'padding' applied
    mtd const getClientRectPadding()->Math.Rectangle
    {
        var result: retval = me.getClientRect()
        result.applyPadding(me.padding)
        return result
    }

    // Get the wnd position in the surface
    mtd const getSurfaceRect()->Math.Rectangle
    {
        var result: retval = me.position
        var p       = me.parent
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
    mtd const getRectIn(wnd: *Wnd)->Math.Rectangle
    {
        var result: retval = me.position
        var p       = me.parent
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
        with me.margin
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

        if me.parent:
            me.parent.sendResizeEvent()
    }

    #[Swag.Overload]
    mtd setAllMargins(value: f32) = me.setMargin(value, value, value, value)
    #[Swag.Overload]
    mtd setAllMargins(value: Math.Vector4) = me.setMargin(value.x, value.y, value.z, value.w)

    // Set padding for childs
    // If a value is Swag.F32.Inf, then it won't be changed
    mtd setPadding(left, top, right, bottom: f32 = Swag.F32.Inf)
    {
        with me.padding
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

        me.sendResizeEvent()
    }

    #[Swag.Overload]
    mtd setAllPaddings(value: f32) = me.setPadding(value, value, value, value)
    #[Swag.Overload]
    mtd setAllPaddings(value: Math.Vector4) = me.setPadding(value.x, value.y, value.z, value.w)

    // Force the window to be the first painted (before all siblings)
    mtd bringToFront()
    {
        Debug.assert(me.parent != null)
        if me.parent.childs.front() == me:
            return

        let idx = Slice.findLinear(me.parent.childs.toSlice(), me)
        Debug.assert(idx != Swag.U64.Max)

        me.parent.childs.removeAtOrdered(idx)
        me.parent.childs.insertAt(0, me)
    }

    // Force the window to be the last painted (on top of siblings)
    mtd bringToTop()
    {
        Debug.assert(me.parent != null)
        if me.parent.childs.back() == me:
            return

        let idx = Slice.findLinear(me.parent.childs.toSlice(), me)
        Debug.assert(idx != Swag.U64.Max)

        // Put the window at the back, or before the first topmost window
        var idxToReplace = me.parent.childs.count - 1
        if !me.wndFlags.has(.TopMost)
        {
            idxToReplace = me.parent.firstTopMostIndex()
            if idxToReplace == Swag.U64.Max:
                return
            idxToReplace -= 1
        }

        swap(me.parent.childs.buffer + idx, me.parent.childs.buffer + idxToReplace)
    }

    // Move and size the wnd
    #[Swag.Overload]
    mtd setPosition(rect: Math.Rectangle, computeLayout = true)
    {
        me.move(rect.x, rect.y)
        me.resize(rect.width, rect.height, computeLayout)
    }

    // Move and size the wnd
    #[Swag.Overload]
    mtd setPosition(x, y, w, h: f32, computeLayout = true)
    {
        me.move(x, y)
        me.resize(w, h, computeLayout)
    }

    // Move the wnd
    mtd move(x, y: f32)
    {
        if me.position.x == x and me.position.y == y:
            return
        var evt = MoveEvent{}
        evt.oldPos    = {me.position.x, me.position.y}
        me.position.x = Math.round(x)
        me.position.y = Math.round(y)
        me.sendEvent(&evt)
    }

    internal mtd clampSize(pos: *Math.Rectangle)
    {
        if me.minSize.x > 0:
            pos.width = Math.max(pos.width, me.minSize.x)
        if me.minSize.y > 0:
            pos.height = Math.max(pos.height, me.minSize.y)
        if me.maxSize.x > 0:
            pos.width = Math.min(pos.width, me.maxSize.x)
        if me.maxSize.y > 0:
            pos.height = Math.min(pos.height, me.maxSize.y)
    }

    // Resize the wnd
    mtd resize(width, height: f32, computeLayout = true)
    {
        if me.position.width == width and me.position.height == height:
            return

        var evt = ResizeEvent{}
        evt.oldSize        = {me.position.width, me.position.height}
        me.position.width  = Math.round(width)
        me.position.height = Math.round(height)
        me.clampSize(&me.position)

        me.sendEvent(&evt)

        if computeLayout:
            me.applyLayout(evt.oldSize)
    }

    // Apply the current layout (childs constraints)
    #[Swag.Overload]
    mtd applyLayout()
    {
        me.applyLayout({me.position.width, me.position.height})
    }

    #[Swag.Overload]
    mtd applyLayout(oldSize: Math.Point)
    {
        var clientRect = me.getClientRectPadding()

        var centerLayout = Array.createTemp'(*Wnd)()
        var oldSizes     = Array.createTemp'Math.Point()

        foreach c in me.childs
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

        foreach c in me.childs
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

        foreach c, i in me.childs
        {
            if !c.isVisible():
                continue
            c.applyLayout(oldSizes[i])
        }
    }

    // Get the child window at the given coordinate
    // Coordinate is expressed in the parent system
    mtd getWndAt(x, y: f32, getDisabled, getHidden = false)->#null *Wnd
    {
        if !me.position.contains(x, y):
            return null

        // Top level window is the last one painted, so it's the last
        // in the array.
        // So we must foreach in revert order
        for var i = cast(s32) me.childs.count - 1; i >= 0; i -= 1
        {
            let c = me.childs[i]
            if c.wndFlags.has(.Hidden) and !getHidden:
                continue
            if c.wndFlags.has(.Disabled) and !getDisabled:
                continue

            var pt = Math.Point{x - me.position.x, y - me.position.y}
            if !c.wndFlags.has(.NoScroll):
                pt += me.scrollPos
            let res = c.getWndAt(pt.x, pt.y)
            if res:
                return res
        }

        return me
    }

    // Returns true if 'child' is in the child hieararchy of 'me'
    mtd isParentOf(child: *Wnd)->bool
    {
        if !child:
            return false
        var p = child.parent
        while p
        {
            if p == me:
                return true
            p = p.parent
        }

        return false
    }

    // Returns true if 'child' is in the owner/child hieararchy of 'me'
    mtd isOwnerOf(child: *Wnd)->bool
    {
        if !child:
            return false
        var p = child.getOwner()
        while p
        {
            if p == me:
                return true
            p = p.getOwner()
        }

        return false
    }

    // Retrieve the child with the given id
    mtd getChildById(childId: string)->#null *Wnd
    {
        foreach c in me.childs
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
    mtd getParentById(parentId: string)->#null *Wnd
    {
        var p = me.parent
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
        me.surface.app.captureMouse(me)
    }

    mtd releaseMouse()
    {
        me.surface.app.releaseMouse()
    }

    mtd getMouseCapture()->*Wnd
    {
        return me.surface.app.getMouseCapture()
    }

    //////////////////////////////////////////////
    // Keyboard
    //////////////////////////////////////////////

    // Set the keyboard focus
    mtd setFocus()
    {
        if me.surface.app.keybFocusWnd == me:
            return
        if !me.isEnabled():
            return

        let last = me.surface.app.keybFocusWnd

        // Ask the window with the current focus if we can steal it
        if last
        {
            var evt = FocusEvent{kind: FocusEvent.Kind.CanKillFocus, other: me}
            last.sendEvent(&evt)
            if !evt.canChangeFocus:
                return
        }

        // Ask the new window if it wants the focus
        {
            var evt = FocusEvent{kind: FocusEvent.Kind.CanSetFocus, other: last}
            me.sendEvent(&evt)
            if !evt.canChangeFocus:
                return
        }

        if last
        {
            var evt = FocusEvent{kind: FocusEvent.Kind.KillFocus, other: me}
            last.sendEvent(&evt)
            last.invalidate()

            last.ownerNotifyEvent(.ChildFocusChanged)
        }

        me.surface.setFocus()

        var evt = FocusEvent{kind: FocusEvent.Kind.SetFocus, other: last}
        me.surface.app.keybFocusWnd = me
        me.sendEvent(&evt)
        me.invalidate()

        me.ownerNotifyEvent(.ChildFocusChanged)
    }

    // Get the window with the keyboard focus
    mtd getFocus()->*Wnd
    {
        return me.surface.app.keybFocusWnd
    }

    // Returns true if the window has the keyboard focus
    mtd hasFocus()->bool => me.surface.app.keybFocusWnd == me

    // Register a keyboard shortcut
    mtd registerKeyShortcut(mdf: Input.KeyModifiers, key: Input.Key, wndId: WndId)
    {
        foreach &c in me.keyShortcuts
        {
            if c.mdf == mdf and c.key == key
            {
                c.id = wndId
                return
            }
        }

        me.keyShortcuts.add({mdf, key, wndId})
    }

    // Get the id associated with a shortcut. null if none
    mtd getKeyShortcut(mdf: Input.KeyModifiers, key: Input.Key)->#null WndId
    {
        foreach c in me.keyShortcuts
        {
            if c.mdf == mdf and c.key == key:
                return c.id
        }

        let p = me.getOwner()
        if p:
            return p.getKeyShortcut(mdf, key)
        return null
    }

    // Get the name associated with a given id shortcut
    mtd getKeyShortcutNameFor(wndId: WndId)->String
    {
        var result: retval
        foreach c in me.keyShortcuts
        {
            if c.id == wndId
            {
                result = Input.getKeyName(c.mdf, c.key)
                return result
            }
        }

        let p = me.getOwner()
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
        return me.getApp().addTimer(time, me)
    }

    mtd postQuitEvent()
    {
        me.surface.app.postQuitEvent()
    }

    mtd postEvent(evt: IEvent)
    {
        let ptr = evt as Event
        ptr.target = me
        me.surface.app.postEvent(evt)
    }

    mtd postInvalidateEvent()
    {
        let evt = Event.create'InvalidateEvent()
        me.postEvent(evt)
    }

    mtd sendEvent(evt: IEvent)
    {
        let ptr = evt as Event
        ptr.target = me
        me.surface.app.sendEvent(evt)
    }

    mtd sendStateEvent(kind: StateEvent.Kind)
    {
        var evt = StateEvent{kind: kind}
        me.sendEvent(&evt)
    }

    mtd sendResizeEvent()
    {
        var evt = ResizeEvent{}
        evt.oldSize.x = me.position.width
        evt.oldSize.y = me.position.height
        me.sendEvent(&evt)
    }

    mtd postResizeEvent()
    {
        let evt = Event.create'ResizeEvent()
        evt.oldSize.x = me.position.width
        evt.oldSize.y = me.position.height
        me.postEvent(evt)
    }

    mtd sendCreateEvent()
    {
        if me.createEventDone:
            return
        me.createEventDone = true
        var evt = CreateEvent{}
        me.sendEvent(&evt)
    }

    mtd postCommandEvent(cmdId: WndId)
    {
        if cmdId == null:
            return
        let evt = Event.create'CommandEvent()
        evt.id     = cmdId
        evt.source = me
        me.postEvent(evt)
    }

    mtd processEvent(evt: IEvent)
    {
        var eat = false
        if me.hookOnEvent:
            eat = me.hookOnEvent(me, evt)
        if !eat:
            me.itf.onEvent(evt)
    }

    // Send a notification event
    mtd notifyEvent(kind: NotifyEvent.Kind)
    {
        var ntfy = NotifyEvent{}
        ntfy.ntfyKind = kind
        me.sendEvent(&ntfy)
    }

    // Send a notification event
    mtd ownerNotifyEvent(kind: NotifyEvent.Kind)
    {
        let p = me.getOwner()
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
        var evtState = me.sendComputeCommandStateEvent(me.id)
        if evtState.accepted
        {
            evtState.kind = .ApplyCommandState
            me.sendEvent(&evtState)
            return
        }

        foreach c in me.childs:
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
        evtState.source = me
        me.sendEvent(&evtState)

        // Is there a keyboard shortcut or a global hot key ?
        var sk = me.getKeyShortcutNameFor(cmdId)
        if !sk.length:
            sk = me.getApp().getHotKeyShortcutNameFor(cmdId)
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
        me.sendEvent(&evtState)

        // If it's not disabled, then we can send
        if !evtState.setFlags.has(.Disabled) or !evtState.disabled
        {
            var evtc = CommandEvent{}
            evtc.id = cmdId
            me.sendEvent(&evtc)
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
        cxt.wnd = me
        cxt.id  = evt.id

        foreach act in me.actions
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
        cxt.wnd = me
        cxt.id  = evt.id

        foreach act in me.actions
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
        evt.target   = me
        me.processEvent(&evt)

        if !evt.accepted
        {
            foreach c in me.childs:
                c.serializeState(ser)
        }
    }
}
