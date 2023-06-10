#global public
using Core, Pixel

alias WndId = string
alias HookEvent = func(*Wnd, *Event)->bool

enum DockStyle
{
    None
    Top
    Left
    Right
    Bottom
    Center
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
    mdf:    Input.KeyModifiers = Zero
    key:    Input.Key = None
    id:     WndId
    target: *Wnd
}

interface IWnd
{
    mtd onEvent(evt: *Event)
    mtd onHookEvent(evt: *Event)
    mtd onCreateEvent(evt: *CreateEvent)
    mtd onDestroyEvent(evt: *DestroyEvent)
    mtd onStateEvent(evt: *StateEvent)
    mtd onResizeEvent(evt: *ResizeEvent)
    mtd onPrePaintEvent(evt: *PaintEvent)
    mtd onPaintEvent(evt: *PaintEvent)
    mtd onPostPaintEvent(evt: *PaintEvent)
    mtd onKeyEvent(evt: *KeyEvent)
    mtd onMouseEvent(evt: *MouseEvent)
    mtd onSysCommandEvent(evt: *SysCommandEvent)
    mtd onSysUserEvent(evt: *SysUserEvent)
    mtd onSetThemeEvent(evt: *SetThemeEvent)
    mtd onTimerEvent(evt: *TimerEvent)
    mtd onFocusEvent(evt: *FocusEvent)
    mtd onCommandEvent(evt: *CommandEvent)
    mtd onComputeStateEvent(evt: *CommandStateEvent)
    mtd onApplyStateEvent(evt: *CommandStateEvent)
    mtd onFrameEvent(evt: *FrameEvent)
    mtd onSerializeStateEvent(evt: *SerializeStateEvent)
}

#[Swag.EnumFlags]
enum WndFlags
{
    Zero = 0
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
    itf:        IWnd
    type:       typeinfo
    surface:    *Surface
    style:      ThemeStyle
    wndFlags    = WndFlags.ClipChildren
    userData:   *void

    position:       Math.Rectangle
    minSize:        Math.Point
    maxSize:        Math.Point
    scrollPos:      Math.Point

    invalidatePadding:  Math.Vector4

    parent:     *Wnd
    owner:      *Wnd
    childs:     Array'(*Wnd)

    name:       RichString
    id:         WndId
    cursor:     Cursor
    toolTip:    String

    keyShortcuts:   Array'KeyShortcut
    actions:        Array'IActionUI

    margin:         Math.Vector4
    padding:        Math.Vector4
    dockStyle:      DockStyle = None
    anchorStyle:    AnchorStyle = None

    focusStrategy: FocusStategy = MousePressed

    backgroundStyle:  BackgroundStyle = Transparent
    isAllocated:      bool
    isPendingDestroy: bool
    createEventDone:  bool

    hookOnEvent: HookEvent
}

impl IWnd for Wnd
{
    mtd onEvent(evt: *Event)
    {
        switch evt.kind
        {
        case Create:
            itf.onCreateEvent(cast(*CreateEvent) evt)
        case Destroy:
            itf.onDestroyEvent(cast(*DestroyEvent) evt)
        case Show, Hide:
            itf.onStateEvent(cast(*StateEvent) evt)
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
        case SysUser:
            itf.onSysUserEvent(cast(*SysUserEvent) evt)
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
            if executeAction(cast(*CommandEvent) evt)
                return
            itf.onCommandEvent(cast(*CommandEvent) evt)
        case ComputeCommandState:
            if updateAction(cast(*CommandStateEvent) evt)
                return
            itf.onComputeStateEvent(cast(*CommandStateEvent) evt)
        case ApplyCommandState:
            itf.onApplyStateEvent(cast(*CommandStateEvent) evt)
        case Frame:
            itf.onFrameEvent(cast(*FrameEvent) evt)
        case SerializeState:
            itf.onSerializeStateEvent(cast(*SerializeStateEvent) evt)
        }
    }

    mtd onCreateEvent(evt: *CreateEvent)                    {}
    mtd onDestroyEvent(evt: *DestroyEvent)                  {}
    mtd onStateEvent(evt: *StateEvent)                      {}
    mtd onResizeEvent(evt: *ResizeEvent)                    {}
    mtd onPrePaintEvent(evt: *PaintEvent)                   {}
    mtd onPaintEvent(evt: *PaintEvent)                      {}
    mtd onPostPaintEvent(evt: *PaintEvent)                  {}
    mtd onSetThemeEvent(evt: *SetThemeEvent)                { invalidate(); sendResizeEvent(); }
    mtd onTimerEvent(evt: *TimerEvent)                      {}
    mtd onHookEvent(evt: *Event)                            {}
    mtd onSerializeStateEvent(evt: *SerializeStateEvent)    {}
    mtd onFrameEvent(evt: *FrameEvent)                      {}

    mtd onSysCommandEvent(evt: *SysCommandEvent)        { evt.accepted = false; }
    mtd onSysUserEvent(evt: *SysUserEvent)              { evt.accepted = false; }
    mtd onFocusEvent(evt: *FocusEvent)                  { evt.accepted = false; }
    mtd onKeyEvent(evt: *KeyEvent)                      { evt.accepted = false; }
    mtd onMouseEvent(evt: *MouseEvent)                  { evt.accepted = false; }
    mtd onCommandEvent(evt: *CommandEvent)              { evt.accepted = false; }
    mtd onComputeStateEvent(evt: *CommandStateEvent)    { evt.accepted = false; }
    mtd onApplyStateEvent(evt: *CommandStateEvent)      { evt.accepted = false; }
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
        mtd getTopWnd()         => surface.wnd
        mtd getTheme()          => style.refr.theme
        mtd getThemeMetrics()   => &style.refr.theme.metrics
        mtd getThemeColors()    => &style.refr.theme.colors
        mtd getThemeRects()     => &style.refr.theme.rects
        mtd getTiming()         => &surface.app.timing
        mtd getFont()           => style.getFont()
    }

    #[Swag.Inline]
    func(T) create(parent: *Wnd = null, id: WndId = null, hook: HookEvent = null)->*T
    {
        return create(parent, "", {}, id: id, hook: hook)
    }

    #[Swag.Inline]
    func(T) create(parent: *Wnd, position: Math.Rectangle, id: WndId = null, hook: HookEvent = null)->*T
    {
        return create(parent, "", position, id: id, hook: hook)
    }

    func(T) create(parent: *Wnd, name: string, position: Math.Rectangle, id: WndId = null, hook: HookEvent = null)->*T
    {
        res := Memory.new'T()
        res.isAllocated = true
        res.itf = cast(IWnd) dref res
        res.type = T
        res.name = name
        res.id = id
        res.position = position
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
        surface.app.destroyWnd(self)
    }

    // Destroy the window
    mtd destroy()
    {
        wndFlags.add(.Hidden)
        if isPendingDestroy
            return
        isPendingDestroy = true
        visit c: childs
            c.destroy()
        surface.app.postDestroy.add(self)
        invalidate()

        if parent and !parent.isPendingDestroy
            parent.applyLayout()
    }

    private mtd firstTopMostIndex()->u64
    {
        visit c, i: childs
        {
            if c.wndFlags.has(.TopMost)
                return i
        }

        return Swag.U64.Max
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

            // Add the window at the back, or before the first topmost window
            if !who.childs.count
                who.childs.add(self)
            else
            {
                idxToInsert := who.childs.count
                if !(wndFlags & .TopMost)
                {
                    idxToInsert = who.firstTopMostIndex()
                    if idxToInsert == Swag.U64.Max
                        idxToInsert = who.childs.count
                }

                who.childs.insertAt(idxToInsert, self)
            }

            who.invalidate()
        }

        computeStyle()
        sendCreateEvent()
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

        if !wndFlags.has(.Hidden)
            return
        wndFlags.remove(.Hidden)
        parent.invalidate()
    }

    // Hide the window
    mtd hide()
    {
        if wndFlags.has(.Hidden)
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
            if !wndFlags.has(.Disabled)
                return
            wndFlags.remove(.Disabled)
        }
        else
        {
            if wndFlags.has(.Disabled)
                return
            wndFlags.add(.Disabled)

            // Check global state
            if getApp().keybFocusWnd == self
                getTopView().setFocus()
        }

        invalidate()
    }

    // Disable the window
    mtd disable()
    {
        enable(false)
    }

    // Returns true if window, and all its parents, are visible
    mtd isVisible(onlyMe = false)->bool
    {
        check := self
        while check
        {
            if check.wndFlags.has(.Hidden)
                return false
            if onlyMe
                return true
            check = check.parent
        }

        return true
    }

    mtd isVisibleState()->bool => wndFlags.has(.Hidden) ? false : true

    // Returns true if window, and all its parents, are enabled
    mtd isEnabled()->bool
    {
        check := self
        while check
        {
            if check.wndFlags.has(.Disabled)
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
        painter := bc.painter
        if painter.isEmptyClippingRect()
            return

        // Recompute style if necessary
        computeStyle()

        // Transmit disabled state to childs
        lastDisabled := bc.isDisabled
        if wndFlags.has(.Disabled)
            bc.isDisabled = true

        // Background
        theme := getTheme()
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
        curTrs := painter.getTransform()
        evt := PaintEvent{}
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
        if wndFlags.has(.ClipChildren)
            painter.pushClippingRect(getClientRect())

        if !painter.isEmptyClippingRect()
        {
            // Me
            if !wndFlags.has(.NoScroll)
                painter.translateTransform(-scrollPos.x, -scrollPos.y)
            evt.kind = .Paint
            painter.pushState()
            sendEvent(&evt)
            painter.popState()
            painter.setTransform(curTrs)

            // Childs
            visit c: childs
            {
                if c.wndFlags.has(.Hidden)
                    continue

                painter.pushState()
                if !c.wndFlags.has(.NoScroll)
                    painter.translateTransform(-scrollPos.x, -scrollPos.y)
                c.paint(bc)
                painter.popState()
            }
        }

        // Restore clipping
        if wndFlags.has(.ClipChildren)
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
        style.dirty = false
    }

    // Force the window to be repainted
    mtd invalidate()
    {
        rect := getSurfaceRect()
        rect.applyPadding(invalidatePadding)
        rect.applyPadding({-32, -32, -32, -32})
        if surface
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
        pos := position
        if pos.x + pos.width > parent.position.width - marginW
        {
            if simX
                pos.x -= (pos.width + marginH)
            else
                pos.x = parent.position.width - pos.width - marginW
        }

        if pos.y + pos.height > parent.position.height - marginH
        {
            if simY
                pos.y -= (pos.height + marginH)
            else
                pos.y = parent.position.height - pos.height - marginH
        }

        if pos != position
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
        r := getSurfaceRect()
        return {pos.x - r.x, pos.y - r.y}
    }

    // Convert a local coordinate to a surface coordinate
    mtd localToSurface(pos: Math.Point)->Math.Point
    {
        r := getSurfaceRect()
        return {pos.x + r.x, pos.y + r.y}
    }

    // Convert a local coordinate to a surface coordinate
    mtd localToSurface(pos: Math.Rectangle)->Math.Rectangle
    {
        r := getSurfaceRect()
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

    // Get the wnd position relative to a given parent
    mtd getRectIn(in: *Wnd)->Math.Rectangle
    {
        var result: retval = position
        p := parent
        while p and p != in
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
            if left != Swag.F32.Inf
                .x = left
            if top    != Swag.F32.Inf
                .y = top
            if right  != Swag.F32.Inf
                .z = right
            if bottom != Swag.F32.Inf
                .w = bottom
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
            if left   != Swag.F32.Inf
                .x = left
            if top    != Swag.F32.Inf
                .y = top
            if right  != Swag.F32.Inf
                .z = right
            if bottom != Swag.F32.Inf
                .w = bottom
        }

        sendResizeEvent()
    }

    mtd setAllPaddings(value: f32)          = setPadding(value, value, value, value)
    mtd setAllPaddings(value: Math.Vector4) = setPadding(value.x, value.y, value.z, value.w)

    // Force the window to be the first painted (before all siblings)
    mtd bringToFront()
    {
        Debug.assert(parent != null)
        if parent.childs.front() == self
            return

        (idx, found) := Slice.findLinear(parent.childs.toSlice(), self)
        Debug.assert(found)

        parent.childs.removeAtOrdered(idx)
        parent.childs.insertAt(0, self)
    }

    // Force the window to be the last painted (on top of siblings)
    mtd bringToTop()
    {
        Debug.assert(parent != null)
        if parent.childs.back() == self
            return

        (idx, found) := Slice.findLinear(parent.childs.toSlice(), self)
        Debug.assert(found)

        // Put the window at the back, or before the first topmost window
        idxToReplace := parent.childs.count - 1
        if !wndFlags.has(.TopMost)
        {
            idxToReplace = parent.firstTopMostIndex()
            if idxToReplace == Swag.U64.Max
                return
            idxToReplace -= 1
        }

        swap(parent.childs.buffer + idx, parent.childs.buffer + idxToReplace)
    }

    // Move and size the wnd
    mtd setPosition(rect: Math.Rectangle, computeLayout = true)
    {
        move(rect.x, rect.y)
        resize(rect.width, rect.height, computeLayout)
    }

    // Move and size the wnd
    mtd setPosition(x, y, w, h: f32, computeLayout = true)
    {
        move(x, y)
        resize(w, h, computeLayout)
    }

    // Move the wnd
    mtd move(x, y: f32)
    {
        if position.x == x and position.y == y
            return
        evt := MoveEvent{}
        evt.oldPos = {position.x, position.y}
        position.x = Math.round(x)
        position.y = Math.round(y)
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
    mtd resize(width, height: f32, computeLayout = true)
    {
        if position.width == width and position.height == height
            return

        evt := ResizeEvent{}
        evt.oldSize = {position.width, position.height}
        position.width = Math.round(width)
        position.height = Math.round(height)
        clampSize(&position)

        sendEvent(&evt)

        if computeLayout
            applyLayout(evt.oldSize)
    }

    // Apply the current layout (childs constraints)
    mtd applyLayout()
    {
        applyLayout({position.width, position.height})
    }

    mtd applyLayout(oldSize: Math.Point)
    {
        clientRect := getClientRectPadding()

        centerLayout := Array.createTemp'(*Wnd)()
        oldSizes     := Array.createTemp'Math.Point()

        visit c: childs
        {
            if c.anchorStyle == .None
                continue

            pos := c.position
            if c.anchorStyle & .X and oldSize.x > 0
            {
                oldM := oldSize.x - c.position.x
                pos.x = position.width - oldM
            }

            if c.anchorStyle & .Y and oldSize.y > 0
            {
                oldM := oldSize.y - c.position.y
                pos.y = position.height - oldM
            }

            if c.anchorStyle & .Width and oldSize.x > 0
            {
                if position.width - oldSize.x
                    pos.width += position.width - oldSize.x
            }

            if c.anchorStyle & .Height and oldSize.y > 0
            {
                pos.height += position.height - oldSize.y
            }

            c.setPosition(pos, false)
        }

        visit c: childs
        {
            oldSizes.add({c.position.width, c.position.height})
            if !c.isVisibleState()
                continue
            switch c.dockStyle
            {
            case Left:
                pos := c.position
                pos.x = clientRect.x + c.margin.x
                pos.y = clientRect.y + c.margin.y
                pos.setBottom(clientRect.bottom() - c.margin.w)
                if c.maxSize.y > 0 and c.maxSize.y < pos.height and !(c.anchorStyle & .Y)
                    pos.y = clientRect.vertCenter() - c.maxSize.y * 0.5
                c.setPosition(pos, false)
                diff := c.margin.x + pos.width + c.margin.z
                clientRect.width -= diff
                clientRect.x += diff

            case Right:
                pos := c.position
                pos.x = clientRect.right() - pos.width - c.margin.z
                pos.y = clientRect.y + c.margin.y
                pos.setBottom(clientRect.bottom() - c.margin.w)
                if c.maxSize.y > 0 and c.maxSize.y < pos.height and !(c.anchorStyle & .Y)
                    pos.y = clientRect.vertCenter() - c.maxSize.y * 0.5
                c.setPosition(pos, false)
                diff := c.margin.x + pos.width + c.margin.z
                clientRect.width -= diff

            case Top:
                pos := c.position
                pos.x = clientRect.x + c.margin.x
                pos.y = clientRect.y + c.margin.y
                pos.setRight(clientRect.right() - c.margin.z)
                if c.maxSize.x > 0 and c.maxSize.x < pos.width and !(c.anchorStyle & .X)
                    pos.x = clientRect.horzCenter() - c.maxSize.x * 0.5
                c.setPosition(pos, false)
                diff := c.margin.y + pos.height + c.margin.w
                clientRect.y += diff
                clientRect.height -= diff

            case Bottom:
                pos := c.position
                pos.x = clientRect.x + c.margin.x
                pos.y = clientRect.bottom() - c.margin.w - pos.height
                pos.setRight(clientRect.right() - c.margin.z)
                if c.maxSize.x > 0 and c.maxSize.x < pos.width and !(c.anchorStyle & .X)
                    pos.x = clientRect.horzCenter() - c.maxSize.x * 0.5
                c.setPosition(pos, false)
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
            c.setPosition(pos, false)
        }

        visit c, i: childs
        {
            if !c.isVisible()
                continue
            c.applyLayout(oldSizes[i])
        }
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
            if c.wndFlags.has(.Hidden) and !getHidden
                continue
            if c.wndFlags.has(.Disabled) and !getDisabled
                continue


            pt := Math.Point{x - position.x, y - position.y}
            if !c.wndFlags.has(.NoScroll)
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

        surface.setFocus()
        evt := FocusEvent{kind: EventKind.SetFocus}
        evt.other = surface.app.keybFocusWnd
        surface.app.keybFocusWnd = self
        sendEvent(&evt)
        invalidate()
    }

    // Get the window with the keyboard focus
    mtd getFocus()->*Wnd
    {
        return surface.app.keybFocusWnd
    }

    // Returns true if the window has the keyboard focus
    mtd hasFocus() => surface.app.keybFocusWnd == self

    // Register a keyboard shortcut
    mtd registerKeyShortcut(mdf: Input.KeyModifiers, key: Input.Key, wndId: WndId)
    {
        visit &c: keyShortcuts
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
    mtd getKeyShortcut(mdf: Input.KeyModifiers, key: Input.Key)->WndId
    {
        visit c: keyShortcuts
        {
            if c.mdf == mdf and c.key == key
                return c.id
        }

        if owner
            return owner.getKeyShortcut(mdf, key)
        if parent
            return parent.getKeyShortcut(mdf, key)
        return null
    }

    // Get the name associated with a given id shortcut
    mtd getKeyShortcutNameFor(wndId: WndId)->String
    {
        var result: retval
        visit c: keyShortcuts
        {
            if c.id == wndId
            {
                result = Input.getKeyName(c.mdf, c.key)
                return result
            }
        }

        if owner
            return owner.getKeyShortcutNameFor(wndId)
        if parent
            return parent.getKeyShortcutNameFor(wndId)
        return result
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

    mtd postInvalidateEvent()
    {
        evt := Event.create'Event()
        evt.kind = .Invalidate
        postEvent(evt)
    }

    mtd sendEvent(evt: *Event)
    {
        evt.target = self
        surface.app.sendEvent(evt)
    }

    mtd sendStateEvent(kind: EventKind)
    {
        evt := StateEvent{kind: kind}
        sendEvent(&evt)
    }

    mtd sendResizeEvent()
    {
        evt := ResizeEvent{}
        evt.oldSize.x = position.width
        evt.oldSize.y = position.height
        sendEvent(&evt)
    }

    mtd postResizeEvent()
    {
        evt := Event.create'ResizeEvent()
        evt.oldSize.x = position.width
        evt.oldSize.y = position.height
        postEvent(evt)
    }

    mtd sendCreateEvent()
    {
        if createEventDone
            return
        createEventDone = true
        evt := CreateEvent{}
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

    //////////////////////////////////////////////
    // Commands
    //////////////////////////////////////////////

    // Main function to update command state of various windows/widgets.
    // This will update the state of this window, and all of its childs
    mtd updateCommandState()
    {
        evtState := sendComputeCommandStateEvent(id)
        if evtState.accepted
        {
            evtState.kind = .ApplyCommandState
            sendEvent(&evtState)
            return
        }

        visit c: childs
            c.updateCommandState()
    }

    // Send the command state event with the given id to the window.
    // Returns the updated command state.
    mtd sendComputeCommandStateEvent(cmdId: WndId)->CommandStateEvent
    {
        if cmdId == null
            return {}

        var evtState: retval

        // User will fill the state
        evtState.kind = .ComputeCommandState
        evtState.id = cmdId
        evtState.source = self
        sendEvent(&evtState)

        // Is there a keyboard shortcut or a global hot key ?
        sk := getKeyShortcutNameFor(cmdId)
        if !sk.length
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
        if cmdId == null
            return

        // We first need to be sure that the target can deal with the event
        evtState := CommandStateEvent{}
        evtState.kind = .ComputeCommandState
        evtState.id = cmdId
        sendEvent(&evtState)

        // If it's not disabled, then we can send
        if !evtState.setFlags.has(.Disabled) or !evtState.disabled
        {
            evtc := CommandEvent{}
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
        itfAct := @mkinterface(null, @typeof(T), IActionUI)
        actions.add(itfAct)
    }

    private mtd updateAction(evt: *CommandStateEvent)->bool
    {
        var cxt: ActionContext
        cxt.wnd = self
        cxt.id = evt.id

        visit act: actions
        {
            if act.accept(cxt)
            {
                evt.accepted = true
                if act.update(cxt, evt)
                    return true
            }
        }

        return false
    }

    private mtd executeAction(evt: *CommandEvent)->bool
    {
        var cxt: ActionContext
        cxt.wnd = self
        cxt.id = evt.id

        visit act: actions
        {
            if act.accept(cxt)
            {
                evt.accepted = true
                if act.execute(cxt)
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
        evt.ser = ser
        evt.target = self
        processEvent(&evt)

        if !evt.accepted
        {
            visit c: childs
                c.serializeState(ser)
        }
    }
}