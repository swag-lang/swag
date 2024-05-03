#global public
using Core, Serialization

typealias FrameCallback = closure(*Application)

struct Application
{
    using native:           ApplicationNative
    sigFrame:               SigArray'closure(*Application)

    options:                Options
    appIcon:                Pixel.Image
    renderer:               Pixel.RenderOgl
    theme:                  *Theme
    style:                  ThemeStyle
    keyb:                   Input.Keyboard
    mouse:                  Input.Mouse
    mainSurface:            *Surface
    postedEvents:           Array'(*Event)
    surfaces:               Array'(*Surface)
    postDestroy:            Array'(*Wnd)
    hookEvents:             Array'(*Wnd)
    cursorStack:            Array'(Cursor)
    endModalExit:           WndId
    inModalLoop:            u32
    quitCode:               s32
    timing:                 Time.FrameTiming

    mustQuit:               bool
    endModal:               bool
    isActivated:            bool
    fadeDisabledSurface     = true

    mutexPostedEvents:      Sync.Mutex

    duringTimerEvents:      bool
    timers:                 Array'(*Timer)
    timersToAdd:            Array'(*Timer)
    timersToDelete:         Array'(*Timer)
    frameEvents:            Array'(*Wnd)
    modalSurfaces:          Array'(*Surface)
    toDelete:               Array'{ ptr: *void, type: const *Swag.TypeInfo }
    mouseEnterWnd:          *Wnd
    mouseCaptureWnd:        *Wnd
    keybFocusWnd:           *Wnd
    maxRunFrame:            u32
    configPath:             String
    hotKeys:                Array'KeyShortcut
}

internal impl Application
{
    // Send and process an event
    mtd sendEventPrivate(event: *Event, stopAt: *Wnd)
    {
        visit hook: hookEvents do
            hook.itf.onHookEvent(event)

        if event.kind == .Quit
        {
            let evt = cast(*QuitEvent) event
            mustQuit = true
            quitCode = evt.quitCode
            return
        }

        // Be sure we have a target
        if !event.target do
            return

        // Send event to the target
        // If the target does not accept the event, then pass it to the parent
        event.accepted = true
        event.target.processEvent(event)
        while !event.accepted
        {
            event.target = event.target.owner orelse event.target.parent
            if !event.target or event.target == stopAt do
                break
            event.accepted = true
            event.target.processEvent(event)
        }
    }

    func skipDisabled(wnd: *Wnd)->*Wnd
    {
        if !wnd do
            return null
        if wnd.surface.isDisabled do
            return null
        var scan = wnd
        while scan and !scan.isEnabled() do
            scan = scan.parent
        return scan
    }

    mtd sendMouseEvents()
    {
        mouse.update()

        var mousePos = mouse.getPosition()

        // Wnd that will receive the message
        var targetWnd: *Wnd
        if mouseCaptureWnd do
            targetWnd = mouseCaptureWnd
        else do
            targetWnd = getWndAtMouse()
        targetWnd = skipDisabled(targetWnd)

        // If no target window, but one target surface, then we will send
        // to the surfaceWnd (we probably are in the shadow area)
        if !targetWnd
        {
            let targetSurface = getSurfaceAt(mousePos.x, mousePos.y)
            if targetSurface
            {
                targetWnd = targetSurface.wnd
                targetWnd = skipDisabled(targetWnd)
            }
        }

        var evt = MouseEvent{}

        // Accept
        if targetWnd
        {
            evt.kind       = .MouseAccept
            evt.surfacePos = targetWnd.screenToSurface(mousePos)
            evt.target     = targetWnd
            sendEventPrivate(&evt, mouseEnterWnd)
            targetWnd = evt.target
        }

        // Mouse enter/leave
        if !targetWnd or targetWnd != mouseEnterWnd
        {
            // Enter
            if targetWnd
            {
                evt.kind       = .MouseEnter
                evt.surfacePos = targetWnd.screenToSurface(mousePos)
                evt.target     = targetWnd
                sendEventPrivate(&evt, mouseEnterWnd)

                if targetWnd.toolTip.length do
                    ToolTip.show(targetWnd, evt.surfacePos, targetWnd.toolTip)
            }

            // Leave
            if mouseEnterWnd and targetWnd != mouseEnterWnd
            {
                evt.kind       = .MouseLeave
                evt.surfacePos = mouseEnterWnd.screenToSurface(mousePos)
                evt.target     = mouseEnterWnd
                sendEventPrivate(&evt, mouseCaptureWnd)

                ToolTip.hide(mouseEnterWnd)
            }

            mouseEnterWnd = targetWnd
        }

        changeCursor(targetWnd)
        if !targetWnd do
            return

        evt.surfacePos = targetWnd.screenToSurface(mousePos)
        evt.modifiers  = keyb.getPressedModifiers()

        // Move
        var move = mouse.getMove()
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
            let lastP = mouse.previousState.pressed[i]
            let curP  = mouse.currentState.pressed[i]
            if !lastP and curP
            {
                if mouse.isButtonDoubleClick(cast(Input.MouseButton) i) do
                    evt.kind = .MouseDoubleClick
                else
                {
                    evt.kind = .MousePressed
                    if targetWnd.focusStrategy == .MousePressed do
                        targetWnd.setFocus()
                }

                evt.button = cast(Input.MouseButton) i
                targetWnd.sendEvent(&evt)
                ToolTip.hide()
            }
            elif lastP and !curP
            {
                evt.kind   = .MouseReleased
                evt.button = cast(Input.MouseButton) i
                targetWnd.sendEvent(&evt)
            }
        }
    }

    mtd sendMouseWheelEvent(z: s32)
    {
        var targetWnd = mouseEnterWnd
        targetWnd = skipDisabled(targetWnd)
        if !targetWnd do
            return

        var evt      = MouseEvent{kind: EventKind.MouseWheel}
        var mousePos = mouse.getPosition()
        evt.surfacePos = targetWnd.screenToSurface(mousePos)
        evt.move.y     = z
        evt.modifiers  = keyb.getPressedModifiers()
        targetWnd.sendEvent(&evt)
    }

    mtd sendKeyboardEvents()
    {
        keyb.update()

        var targetWnd = keybFocusWnd
        targetWnd = skipDisabled(targetWnd)
        if !targetWnd do
            return

        // Modifiers
        let modifiers = keyb.getPressedModifiers()

        // Detect changes
        loop i: keyb.previousState.pressed
        {
            let lastP = keyb.previousState.pressed[i]
            let curP  = keyb.currentState.pressed[i]
            if !lastP and curP
            {
                var evt = KeyEvent{kind: EventKind.KeyPressed}
                evt.key       = cast(Input.Key) i
                evt.modifiers = modifiers
                targetWnd.sendEvent(&evt)

                // Convert key to command (shortcuts)
                if !evt.accepted
                {
                    let id = targetWnd.getKeyShortcut(modifiers, cast(Input.Key) i)
                    if id != null do
                        targetWnd.sendCommandEvent(id)
                }
            }
            elif lastP and !curP
            {
                var evt = KeyEvent{kind: EventKind.KeyReleased}
                evt.key       = cast(Input.Key) i
                evt.modifiers = modifiers
                targetWnd.sendEvent(&evt)
            }
        }
    }

    mtd sendRuneEvent(key: Input.Key, r: rune)
    {
        var targetWnd = keybFocusWnd
        targetWnd = skipDisabled(targetWnd)
        if !targetWnd do
            return

        var evt = KeyEvent{kind: EventKind.Rune}
        evt.char      = r
        evt.key       = key
        evt.modifiers = keyb.getPressedModifiers()
        targetWnd.sendEvent(&evt)
    }

    mtd sendPostedEvents()
    {
        // Need to make a copy, in case the posted event triggers a modal loop
        mutexPostedEvents.lock()
        var copy = postedEvents
        postedEvents.clear()
        mutexPostedEvents.unlock()

        var needUpdateState = false
        for var i = 0; i < copy.count; i += 1
        {
            let evt = copy[i]
            if evt.kind == .Invalidate do
                evt.target.invalidate()
            else do
                sendEvent(evt)

            // Special event post process
            if evt.kind == .Command do
                needUpdateState = true

            toDelete.add({evt, evt.type})
        }

        if needUpdateState do
            updateCommandState()
    }

    mtd sendDestroyEvents()
    {
        if inModalLoop do
            return
        visit w: postDestroy do
            destroyWnd(w)
        postDestroy.clear()
    }

    // Update UI state of all surfaces
    mtd updateCommandState()
    {
        visit s: surfaces do
            s.wnd.updateCommandState()
    }

    // Remove and destroy the given surface
    // To cleanly close a surface, you should call surface.destroy()
    mtd destroySurface(surface: *Surface)
    {
        surfaces.remove(surface)
        if surface == mainSurface do
            postQuitEvent()

        if modalSurfaces.count and surface == modalSurfaces.back() do
            endModal = true

        surface.destroyNative()
        Memory.delete(surface)
    }

    mtd unregisterWnd(wnd: *Wnd)
    {
        if wnd == mouseEnterWnd do
            mouseEnterWnd = null
        if wnd == mouseCaptureWnd do
            mouseCaptureWnd = null
        if wnd == keybFocusWnd do
            keybFocusWnd = null

        // Be sure there's no timer for that window
        visit t: timers
        {
            if t.target == wnd and !t.toDelete
            {
                timersToDelete.add(t)
                t.toDelete = true
            }
        }

        // Remove from various lists
        frameEvents.remove(wnd)
        hookEvents.remove(wnd)

        // Be sure there's no posted event for that window
        visit e: postedEvents
        {
            if e.target == wnd do
                e.target = null
        }
    }

    mtd sendTimerEvents()
    {
        duringTimerEvents = true
        visit t: timers
        {
            if t.toDelete do
                continue

            if !t.stopwatch.isStarted do
                t.stopwatch.start()

            if t.stopwatch.elapsedMillisecondsNow() >= t.timeMs
            {
                var evt = TimerEvent{}
                evt.timer = t
                t.target.sendEvent(&evt)
                t.stopwatch.restart()
            }
        }

        duringTimerEvents = false

        // Add new registered timers
        visit t: timersToAdd do
            timers.add(t)
        timersToAdd.clear()

        // Remove some timers
        visit t: timersToDelete
        {
            t.toDelete = false
            deleteTimer(t)
        }

        timersToDelete.clear()
    }

    mtd sendFrameEvents()
    {
        visit t: frameEvents
        {
            var evt = FrameEvent{}
            t.sendEvent(&evt)
        }
    }

    mtd deletePointers()
    {
        if inModalLoop do
            return
        visit p: toDelete do
            Memory.delete(p.item0, p.item1)
        toDelete.clear()
    }

    public mtd runFrame()
    {
        // Auto exit
        if maxRunFrame and timing.frameCount > maxRunFrame do
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

        visit s: surfaces do
            s.paint()

        deletePointers()

        sigFrame.call(self)
    }

    public mtd doModalLoop(surface: *Surface, cb: closure(*Application) = null)->WndId
    {
        inModalLoop += 1
        defer inModalLoop -= 1

        visit s: surfaces
        {
            if s != surface do
                s.disable()
        }

        if surface
        {
            surface.enable()
            modalSurfaces.add(surface)
        }

        while !mustQuit and !endModal
        {
            runFrame()
            if cb do
                cb(self)
        }

        if surface do
            modalSurfaces.removeBack()

        if modalSurfaces.count
        {
            let back = modalSurfaces.back()
            back.enable()
            back.wnd.view.setFocus()
        }
        else
        {
            visit s, idx: surfaces
            {
                if s != surface
                {
                    s.enable()
                    s.wnd.view.setFocus()
                }
            }
        }

        if surface do
            surface.destroy()

        endModal = false
        return endModalExit
    }
}

impl Application
{
    mtd opDrop()
    {
        visit it: surfaces do
            it.destroy()
        sendDestroyEvents()
        visit it: postedEvents do
            toDelete.add({it, it.type})
        deletePointers()
        theme.freeDefault()
    }

    // Get application variables
    #[Swag.Inline]
    {
        mtd getRenderer() => &renderer
        mtd getKeyboard() => &keyb
        mtd getMouse()    => &mouse
        mtd const getDt() => timing.dt
    }

    // Create a main surface, and run
    func runSurface(x, y, width, height: s32, title: string = null, view: *Wnd = null, borders = true, canSize = true, hook: HookEvent = null, init: func(*Application) = null)
    {
        var app: Application

        var flags = SurfaceFlags.OverlappedWindow
        if !canSize do
            flags &= ~.Sizeable
        if !borders do
            flags &= ~.Border
        let surface = assume app.createSurface(x, y, width, height, flags, view, hook)
        surface.setTitle(title)
        if init do
            init(&app)
        surface.show()
        app.run()
    }

    // Creates a new surface
    mtd createSurface(x, y, width, height: s32, flags = SurfaceFlags.OverlappedWindow, view: *Wnd = null, hook: HookEvent = null)->*Surface throw
    {
        let surface = Memory.new'Surface()
        surface.app      = self
        surface.position = {x, y, width, height}

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
            .wnd.create(view, hook)
            .wnd.surface = surface
            .wnd.itf     = cast(IWnd) surface.wnd
            .wnd.computeStyle()
            .wnd.view.computeStyle()
            .wnd.sendResizeEvent()
        }

        return surface
    }

    // Associate a configuration file
    mtd setConfigPath(path: string)
    {
        configPath = path
    }

    // Associate a configuration file
    mtd setAppIcon(img: Pixel.Image)
    {
        appIcon = img
    }

    internal mtd serializeState(ser: *Serializer) throw
    {
        ser.beginSection(.Unordered)
        visit s: surfaces do
            s.serializeState(ser)
        ser.endSection()
    }

    // Save the application state
    mtd saveState() throw
    {
        if configPath.isEmpty() do
            return

        var buf:  ConcatBuffer
        var save: Encoder'Write.TagBin
        var ser:  Serializer
        ser.startWrite(save, &buf)
        serializeState(&ser)
        ser.end()

        File.writeAllBytes(configPath, buf)
    }

    // Load the application state
    mtd loadState() throw
    {
        if configPath.isEmpty() do
            return

        var buf = File.readAllBytes(configPath)

        var load: Decoder'Read.TagBin
        var ser:  Serializer
        ser.startRead(load, buf.toSlice())
        serializeState(&ser)
        ser.end()
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
        var evt = DestroyEvent{}
        wnd.sendEvent(&evt)

        unregisterWnd(wnd)

        // Remove from parent
        if wnd.parent and !wnd.parent.isPendingDestroy
        {
            let idx = Slice.findLinear(wnd.parent.childs.toSlice(), wnd)
            Debug.assert(idx != Swag.U64.Max)
            wnd.parent.childs.removeAtOrdered(idx)
            wnd.parent.invalidate()
        }

        if wnd.isAllocated
        {
            toDelete.add({wnd, wnd.type})
        }
    }

    // Returns the window under the given point
    mtd getWndAtMouse()->*Wnd
    {
        var mousePos      = mouse.getPosition()
        let targetSurface = getSurfaceAt(mousePos.x, mousePos.y)
        if !targetSurface do
            return null
        targetSurface.screenToSurface(&mousePos)
        let targetWnd = targetSurface.wnd.getWndAt(mousePos.x, mousePos.y)
        return targetWnd
    }

    // Set activated state
    mtd setActivated(activated: bool)
    {
        if isActivated == activated do
            return
        isActivated = activated
        visit s: modalSurfaces do
            s.bringToTop()
        invalidate()
    }

    // Force all surfaces to be painted
    mtd invalidate()
    {
        visit s: surfaces do
            s.invalidate()
    }

    // Ask to exit the application
    mtd postQuitEvent(quitCode: s32 = 0)
    {
        let evt = Event.create'QuitEvent()
        evt.quitCode = #self.quitCode
        postEvent(evt)
    }

    // Post a new event (thread safe)
    mtd postEvent(event: *Event)
    {
        Sync.scopedLock(&mutexPostedEvents)
        postedEvents.add(event)
    }

    mtd sendEvent(event: *Event)
    {
        sendEventPrivate(event, null)
    }

    // Register a new timer for the given 'target'
    mtd addTimer(time: Time.Duration, target: *Wnd)->*Timer
    {
        let timer = Memory.new'Timer()
        timer.timeMs = time.toMs()
        timer.target = target
        if duringTimerEvents do
            timersToAdd.add(timer)
        else do
            timers.add(timer)
        return timer
    }

    // Delete and unregister timer
    mtd deleteTimer(timer: *Timer)
    {
        if duringTimerEvents
        {
            Debug.assert(!timer.toDelete)
            timer.toDelete = true
            timersToDelete.add(timer)
        }
        else
        {
            Debug.assert(!timer.toDelete)
            Memory.delete(timer)
            timers.remove(timer)
        }
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

    internal mtd changeCursor(wnd: *Wnd)
    {
        if cursorStack.count do
            Cursor.apply(cursorStack.back())
        elif wnd and wnd.cursor.native.cursor != null do
            Cursor.apply(wnd.cursor)
        else do
            Cursor.apply(Cursor.from(CursorShape.Arrow))
    }

    // Run until exit
    #[Swag.Discardable]
    mtd run()->s32
    {
        visit s: surfaces
        {
            var evt = FrameEvent{}
            evt.firstFrame = true
            s.wnd.view.sendEvent(&evt)
        }

        while !mustQuit do
            runFrame()

        return quitCode
    }

    // End current modal loop
    mtd exitModal(exitId: string = "")
    {
        endModal     = true
        endModalExit = exitId
    }

    mtd getFileIcon(name: string, small = true)->Icon      => getFileDirectoryIcon(name, false, small)
    mtd getDirectoryIcon(name: string, small = true)->Icon => getFileDirectoryIcon(name, true, small)

    // Register a global os key action
    #[Swag.Overload]
    mtd registerHotKey(mdf: Input.KeyModifiers, key: Input.Key, wndId: WndId, target: *Wnd) throw
    {
        visit &c: hotKeys
        {
            if c.mdf == mdf and c.key == key
            {
                c.id     = wndId
                c.target = target
                return
            }
        }

        hotKeys.add({mdf, key, wndId, target})
        registerHotKey(mdf, key)
    }

    // Get the id associated with a shortcut. null if none
    mtd getHotKeyShortcut(mdf: Input.KeyModifiers, key: Input.Key)->{ id: WndId, wnd: *Wnd }
    {
        visit c: hotKeys
        {
            if c.mdf == mdf and c.key == key do
                return {c.id, c.target}
        }

        return {null, null}
    }

    // Get the name associated with a given id shortcut
    mtd getHotKeyShortcutNameFor(wndId: WndId)->String
    {
        var result: retval
        visit c: hotKeys
        {
            if c.id == wndId
            {
                result = Input.getKeyName(c.mdf, c.key)
                return result
            }
        }

        return result
    }
}
