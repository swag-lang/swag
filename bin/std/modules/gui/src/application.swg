#global public
using Core

alias FrameCallback = closure(*Application)

struct Application
{
    renderer:       Pixel.RenderOgl
    theme:          *Theme
    style:          ThemeStyle
    keyb:           Input.Keyboard
    mouse:          Input.Mouse
    mainSurface:    *Surface
    postedEvents:   Array'(*Event)
    surfaces:       Array'(*Surface)
    postDestroy:    Array'(*Wnd)
    hookEvents:     Array'(*Wnd)
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
    maxRunFrame:        u32
}

private impl Application
{
    // Send and process an event
    mtd sendEventPrivate(event: *Event, stopAt: *Wnd)
    {
        visit hook: hookEvents
            hook.itf.onHookEvent(event)

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
            event.target = event.target.owner orelse event.target.parent
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
        if wnd.surface.isDisabled
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

        // If no target window, but one target surface, then we will send
        // to the surfaceWnd (we probably are in the shadow area)
        if !targetWnd
        {
            targetSurface := getSurfaceAt(mousePos.x, mousePos.y)
            if targetSurface
            {
                targetWnd = targetSurface.wnd
                targetWnd = skipDisabled(targetWnd)
            }
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

                if targetWnd.toolTip.length
                    ToolTip.show(targetWnd, evt.surfacePos, targetWnd.toolTip)
            }

            // Leave
            if mouseEnterWnd and targetWnd != mouseEnterWnd
            {
                evt.kind = .MouseLeave
                evt.surfacePos = mouseEnterWnd.screenToSurface(mousePos)
                evt.target = mouseEnterWnd
                sendEventPrivate(&evt, mouseCaptureWnd)

                ToolTip.hide(mouseEnterWnd)
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
            ToolTip.resetTimer(evt.surfacePos)
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
                ToolTip.hide()
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
        if !targetWnd
            return

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
                evt.modifiers = modifiers
                targetWnd.sendEvent(&evt)

                if !evt.accepted
                {
                    id := targetWnd.getKeyShortcut(modifiers, cast(Input.Key) i)
                    if id != null
                    {
                        evtui := CommandUIEvent{}
                        evtui.id = id
                        targetWnd.sendEvent(&evtui)
                        if !evtui.disabled
                        {
                            evtc := CommandEvent{}
                            evtc.id = id
                            targetWnd.sendEvent(&evtc)
                        }
                    }
                }
            }
            elif lastP and !curP
            {
                evt := KeyEvent{kind: EventKind.KeyReleased}
                evt.key = cast(Input.Key) i
                evt.modifiers = modifiers
                targetWnd.sendEvent(&evt)
            }
        }
    }

    mtd sendRuneEvent(key: Input.Key, r: rune)
    {
        targetWnd := keybFocusWnd
        targetWnd = skipDisabled(targetWnd)
        if !targetWnd
            return

        evt := KeyEvent{kind: EventKind.Rune}
        evt.char = r
        evt.key = key
        evt.modifiers = keyb.getPressedModifiers()
        targetWnd.sendEvent(&evt)
    }

    mtd sendPostedEvents()
    {
        // Need to make a copy, in case the posted event triggers a modal loop
        copy := postedEvents
        postedEvents.clear()

        for i := 0; i < copy.count; i += 1
        {
            evt := copy[i]
            sendEvent(evt)

            // Special event post process
            if evt.kind == .Command
            {
                pevt := cast(*CommandEvent) evt
                pevt.source.updateCommandState()
            }

            Memory.delete(evt, evt.type)
        }
    }

    mtd sendDestroyEvents()
    {
        visit wnd: postDestroy
            destroyWnd(wnd)
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

        // Remove from various lists
        frameEvents.remove(wnd)
        hookEvents.remove(wnd)

        // Be sure there's no posted event for that window
        visit e: postedEvents
        {
            if e.target == wnd
                e.target = null
        }
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
        // Auto exit
        if maxRunFrame and timing.frameCount > maxRunFrame
            Env.exit(0)

        timing.update()
        Memory.freeTemp()

        ToolTip.update(getDt())
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
            surface.enable()
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
        {
            back := modalSurfaces.back()
            back.enable()
            back.wnd.view.setFocus()
        }

        endModal = false
        return endModalExit
    }
}

impl Application
{
    // Get application variables
    #[Swag.Inline]
    {
        mtdc getRenderer() => &renderer
        mtdc getKeyboard() => &keyb
        mtdc getMouse()    => &mouse
        mtdc getDt()       => timing.dt
    }

    // Create a main surface, and run
    func runSurface(x, y, width, height: s32, title: string = null, view: *Wnd = null, borders = true, canSize = true, hook: HookEvent = null, init: func(*Application) = null)
    {
        var app: Application

        flags := SurfaceFlags.OverlappedWindow
        if !canSize flags &= ~.Sizeable
        if !borders flags &= ~.Border
        surface := assume app.createSurface(x, y, width, height, flags, view, hook)
        surface.setTitle(title)
        if init init(&app)
        surface.show()
        app.run()
    }

    // Creates a new surface
    mtd createSurface(x, y, width, height: s32, flags = SurfaceFlags.OverlappedWindow, view: *Wnd = null, hook: HookEvent = null)->*Surface throw
    {
        surface := Memory.new'Surface()
        surface.app = self
        surface.position = @{x, y, width, height}

        if !mainSurface
        {
            theme = Memory.new'Theme()
            style.set(theme)
        }

        try Surface.create(surface, x, y, width, height, flags, mainSurface)

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
            wnd.create(view, hook)
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

    // Destroy a window
    // To cleanly close a window, you should call wnd.destroy()
    mtd destroyWnd(wnd: *Wnd)
    {
        evt := DestroyEvent{}
        wnd.sendEvent(&evt)

        unregisterWnd(wnd)

        // Remove from parent
        if wnd.parent and !wnd.parent.isPendingDestroy
        {
            (idx, ok) := Slice.findLinear(wnd.parent.childs.toSlice(), wnd)
            Debug.assert(ok)
            wnd.parent.childs.removeAtOrdered(idx)
            wnd.parent.invalidate()
        }

        if wnd.isAllocated
            Memory.delete(wnd, wnd.type)
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
        Debug.assert(!frameEvents.contains(target))
        frameEvents.add(target)
    }

    // Unregister a window that receives a FrameEvent each running loop
    mtd unregisterFrameEvent(target: *Wnd)
    {
        frameEvents.remove(target)
    }

    // Register a window to receive hook events
    mtd registerHookEvents(target: *Wnd)
    {
        Debug.assert(!hookEvents.contains(target))
        hookEvents.add(target)
    }

    // Unregister a window to receive hook events
    mtd unregisterHookEvents(target: *Wnd)
    {
        hookEvents.remove(target)
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
    mtd exitModal(exitId: string = "")
    {
        endModal = true
        endModalExit = exitId
    }
}