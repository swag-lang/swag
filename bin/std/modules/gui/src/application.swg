#global public
using Core

alias FrameCallback = closure(*Application)

struct Application
{
    renderer:       Pixel.RenderOgl
    theme:          *Theme
    style:          Style
    keyb:           Input.Keyboard
    mouse:          Input.Mouse
    mainSurface:    *Surface
    postedEvents:   Array'(*Event)
    surfaces:       Array'(*Surface)
    postDestroy:    Array'(*Wnd)
    cursorStack:    Array'(Cursor)
    mustQuit:       bool
    endModal:       bool
    endModalExit:   WndId
    isActivated:    bool
    quitCode:       s32
    timing:         Time.FrameTiming

    timers:             Array'(*Timer)
    frameEvents:        Array'(*Wnd)
    modalSurfaces:      Array'(*Surface)
    mouseEnterWnd:      *Wnd
    mouseCaptureWnd:    *Wnd
    keybFocusWnd:       *Wnd
}

private impl Application
{
    // Send and process an event
    mtd sendEventPrivate(event: *Event, stopAt: *Wnd)
    {
        if event.kind == .Quit
        {
            evt := cast(*QuitEvent) event
            mustQuit = true
            quitCode = evt.quitCode
            return
        }

        // Be sure we have a target
        if !event.target
            return

        // Send event to the target
        // If the target does not accept the event, then pass it to the parent
        event.accepted = true
        event.target.processEvent(event)
        while !event.accepted
        {
            event.target = event.target.parent
            if !event.target or event.target == stopAt
                break
            event.accepted = true
            event.target.processEvent(event)
        }
    }

    func skipDisabled(wnd: *Wnd)->*Wnd
    {
        if !wnd
            return null
        scan := wnd
        while scan and !scan.isEnabled()
            scan = scan.parent
        return scan
    }

    mtd sendMouseEvents()
    {
        mouse.update()

        mousePos := mouse.getPosition()

        // Wnd that will receive the message
        var targetWnd: *Wnd
        if mouseCaptureWnd
            targetWnd = mouseCaptureWnd
        else
            targetWnd = getWndAt(mousePos.x, mousePos.y)
        targetWnd = skipDisabled(targetWnd)

        // If not target window, but one target surface, then we will send
        // to the surfaceWnd (we probably are in the shadow area)
        if !targetWnd
        {
            targetSurface := getSurfaceAt(mousePos.x, mousePos.y)
            if targetSurface
                targetWnd = targetSurface.wnd
        }

        evt := MouseEvent{}

        // Accept
        if targetWnd
        {
            evt.kind = .MouseAccept
            evt.surfacePos = targetWnd.screenToSurface(mousePos)
            evt.target = targetWnd
            sendEventPrivate(&evt, mouseEnterWnd)
            targetWnd = evt.target
        }

        // Mouse enter/leave
        if !targetWnd or targetWnd != mouseEnterWnd
        {
            // Enter
            if targetWnd
            {
                evt.kind = .MouseEnter
                evt.surfacePos = targetWnd.screenToSurface(mousePos)
                evt.target = targetWnd
                sendEventPrivate(&evt, mouseEnterWnd)
            }

            // Leave
            if mouseEnterWnd and targetWnd != mouseEnterWnd
            {
                evt.kind = .MouseLeave
                evt.surfacePos = mouseEnterWnd.screenToSurface(mousePos)
                evt.target = mouseEnterWnd
                sendEventPrivate(&evt, mouseCaptureWnd)
            }

            mouseEnterWnd = targetWnd
        }

        changeCursor(targetWnd)
        if !targetWnd
            return

        evt.surfacePos = targetWnd.screenToSurface(mousePos)
        evt.modifiers  = keyb.getPressedModifiers()

        // Move
        move := mouse.getMove()
        if Math.abs(move.x) >= 0.5 or Math.abs(move.y) >= 0.5
        {
            evt.kind = EventKind.MouseMove
            evt.move = move
            targetWnd.sendEvent(&evt)
        }

        // Detect changes
        loop i: mouse.previousState.pressed
        {
            lastP := mouse.previousState.pressed[i]
            curP  := mouse.currentState.pressed[i]
            if !lastP and curP
            {
                if mouse.isButtonDoubleClick(cast(Input.MouseButton) i)
                    evt.kind = .MouseDoubleClick
                else
                {
                    evt.kind = .MousePressed
                    if targetWnd.focusStrategy == .MousePressed
                        targetWnd.setFocus()
                }

                evt.button = cast(Input.MouseButton) i
                targetWnd.sendEvent(&evt)
            }
            elif lastP and !curP
            {
                evt.kind = .MouseReleased
                evt.button = cast(Input.MouseButton) i
                targetWnd.sendEvent(&evt)
            }
        }
    }

    mtd sendMouseWheelEvent(z: s32)
    {
        targetWnd := mouseEnterWnd
        targetWnd = skipDisabled(targetWnd)
        if !targetWnd
            return

        evt := MouseEvent{kind: EventKind.MouseWheel}
        mousePos := mouse.getPosition()
        evt.surfacePos = targetWnd.screenToSurface(mousePos)
        evt.move.y     = z
        evt.modifiers  = keyb.getPressedModifiers()
        targetWnd.sendEvent(&evt)
    }

    mtd sendKeyboardEvents()
    {
        keyb.update()

        targetWnd := keybFocusWnd
        targetWnd = skipDisabled(targetWnd)

        // Modifiers
        modifiers := keyb.getPressedModifiers()

        // Detect changes
        loop i: keyb.previousState.pressed
        {
            lastP := keyb.previousState.pressed[i]
            curP  := keyb.currentState.pressed[i]
            if !lastP and curP
            {
                evt := KeyEvent{kind: EventKind.KeyPressed}
                evt.key = cast(Input.Key) i
                evt.target = targetWnd
                evt.modifiers = modifiers
                sendEvent(&evt)
            }
            elif lastP and !curP
            {
                evt := KeyEvent{kind: EventKind.KeyReleased}
                evt.key = cast(Input.Key) i
                evt.target = targetWnd
                evt.modifiers = modifiers
                sendEvent(&evt)
            }
        }
    }

    mtd sendCharEvent(r: rune)
    {
        targetWnd := keybFocusWnd
        targetWnd = skipDisabled(targetWnd)
        if !targetWnd
            return
        evt := KeyEvent{kind: EventKind.Char}
        evt.char = r
        evt.modifiers = keyb.getPressedModifiers()
        targetWnd.sendEvent(&evt)
    }

    mtd sendPostedEvents()
    {
        for i := 0; i < postedEvents.count; i += 1
        {
            evt := postedEvents[i]
            sendEvent(evt)
            Memory.delete(evt, evt.type)
        }

        postedEvents.clear()
    }

    mtd sendDestroyEvents()
    {
        for i := 0; i < postDestroy.count; i += 1
        {
            evt := DestroyEvent{}
            wnd := postDestroy[i]
            wnd.sendEvent(&evt)
            destroyWnd(wnd)
        }

        postDestroy.clear()
    }

    // Remove and destroy the given surface
    // To cleanly close a surface, you should call surface.destroy()
    mtd destroySurface(surface: *Surface)
    {
        surfaces.remove(surface)
        if surface == mainSurface
            postQuitEvent()

        if modalSurfaces.count and surface == modalSurfaces.back()
            endModal = true

        surface.destroyNative()
        Memory.delete(surface)
    }

    mtd unregisterWnd(wnd: *Wnd)
    {
        if wnd == mouseEnterWnd
            mouseEnterWnd = null
        if wnd == mouseCaptureWnd
            mouseCaptureWnd = null
        if wnd == keybFocusWnd
            keybFocusWnd = null

        // Be sure there's no timer for that window
        for i := 0; i < timers.count; i += 1
        {
            if timers[i].target == wnd
            {
                deleteTimer(timers[i])
                i -= 1
            }
        }

        // Be sure there's no frame event
        frameEvents.remove(wnd)

        // Be sure there's no posted event for that window
        visit e: postedEvents
        {
            if e.target == wnd
                e.target = null
        }
    }

    // Destroy a window
    // To cleanly close a window, you should call wnd.destroy()
    mtd destroyWnd(wnd: *Wnd)
    {
        unregisterWnd(wnd)

        // Remove from parent
        if wnd.parent and !wnd.parent.isPendingDestroy
        {
            (idx, ok) := Slice.findLinear(wnd.parent.childs.toSlice(), wnd)
            Debug.assert(ok)
            wnd.parent.childs.removeAt(idx)
        }

        if wnd.isAllocated
            Memory.delete(@dataof(wnd.itf), @kindof(wnd.itf))
    }

    mtd sendTimerEvents()
    {
        visit t: timers
        {
            if !t.stopwatch.isStarted
                t.stopwatch.start()

            if t.stopwatch.elapsedMillisecondsNow() >= t.timeMs
            {
                evt := TimerEvent{}
                evt.timer = t
                t.target.sendEvent(&evt)
                t.stopwatch.restart()
            }
        }
    }

    mtd sendFrameEvents()
    {
        visit t: frameEvents
        {
            evt := FrameEvent{}
            t.sendEvent(&evt)
        }
    }

    mtd runFrame()
    {
        timing.update()
        Memory.freeTemp()

        sendFrameEvents()
        sendTimerEvents()
        sendSystemEvents()
        sendKeyboardEvents()
        sendMouseEvents()
        sendPostedEvents()
        sendDestroyEvents()

        visit s:
            surfaces s.paint()
    }

    mtd doModalLoop(surface: *Surface, cb: closure(self) = null)->WndId
    {
        visit s: surfaces
            s.disable()

        if surface
        {
            modalSurfaces.add(surface)
        }

        while !mustQuit and !endModal
        {
            runFrame()
            if cb
                cb(self)
        }

        if surface
        {
            surface.destroy()
            modalSurfaces.removeBack()
        }

        visit s: surfaces
            s.enable()

        if modalSurfaces.count
            modalSurfaces.back().enable()

        endModal = false
        return endModalExit
    }
}

impl Application
{
    // Get application variables
    mtd getRenderer() => &renderer
    mtd getKeyboard() => &keyb

    // Creates a new surface
    mtd createSurface(x, y, width, height: s32, flags: SurfaceFlags = SurfaceFlags.Zero, view: *Wnd = null)->*Surface throw
    {
        surface := Memory.new'Surface()
        surface.app = self
        surface.position = @{x, y, width, height}

        if !mainSurface
        {
            theme = Memory.new'Theme()
            style.set(theme)
        }

        try Surface.create(surface, view, x, y, width, height, flags, mainSurface)

        if !mainSurface
        {
            renderer.setCurrentContext(surface.rc)
            renderer.init()
            theme.setupDefault(&renderer)
            mainSurface = surface
        }

        surfaces.add(surface)

        with surface
        {
            wnd.create(view)
            wnd.surface = surface
            wnd.itf = cast(IWnd) surface.wnd
            wnd.computeStyle()
            wnd.view.computeStyle()
            wnd.sendResizeEvent()
        }

        return surface
    }

    // Return the captured wnd for mouse
    mtd getMouseCapture()->*Wnd
    {
        return mouseCaptureWnd
    }

    // Returns the window under the given point
    mtd getWndAt(x, y: f32)->*Wnd
    {
        mousePos := mouse.getPosition()
        targetSurface := getSurfaceAt(mousePos.x, mousePos.y)
        if !targetSurface
            return null
        targetSurface.screenToSurface(&mousePos)
        targetWnd := targetSurface.wnd.getWndAt(mousePos.x, mousePos.y)
        return targetWnd
    }

    // Set activated state
    mtd setActivated(activated: bool)
    {
        if isActivated == activated
            return
        isActivated = activated
        visit s: modalSurfaces
            s.bringToTop()
        invalidate()
    }

    // Force all surfaces to be painted
    mtd invalidate()
    {
        visit s: surfaces
            s.invalidate()
    }

    // Ask to exit the application
    mtd postQuitEvent(quitCode: s32 = 0)
    {
        evt := Event.create'QuitEvent()
        evt.quitCode = #self.quitCode
        postEvent(evt)
    }

    // Post a new event
    mtd postEvent(event: *Event)
    {
        postedEvents.add(event)
    }

    mtd sendEvent(event: *Event)
    {
        sendEventPrivate(event, null)
    }

    // Register a new timer for the given 'target'
    mtd addTimer(time: Time.Duration, target: *Wnd)->*Timer
    {
        timer := Memory.new'Timer()
        timer.timeMs = time.toMs()
        timer.target = target
        timers.add(timer)
        return timer
    }

    // Delete and unregister timer
    mtd deleteTimer(timer: *Timer)
    {
        Memory.delete(timer)
        timers.remove(timer)
    }

    // Register a window to receive a FrameEvent each running loop
    mtd registerFrameEvent(target: *Wnd)
    {
        frameEvents.add(target)
    }

    // Unregister a window that receives a FrameEvent each running loop
    mtd unregisterFrameEvent(target: *Wnd)
    {
        frameEvents.remove(target)
    }

    // Push a new mouse cursor
    mtd pushCursor(cursor: Cursor)
    {
        cursorStack.add(cursor)
    }

    // Pop a new mouse cursor
    mtd popCursor()
    {
        cursorStack.removeBack()
    }

    // Run until exit
    #[Swag.Discardable]
    mtd run()->s32
    {
        while !mustQuit
        {
            runFrame()
        }

        return quitCode
    }

    // End current modal loop
    mtd exitModal(exitId: string)
    {
        endModal = true
        endModalExit = exitId
    }
}