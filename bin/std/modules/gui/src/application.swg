#global public
using Core, Serialization

alias FrameCallback = func||(*Application)

struct Application
{
    using native:           ApplicationNative
    sigFrame:               SigArray'func||(*Application)

    options:                ApplicationOptions
    appIcon:                Pixel.Image
    renderer:               Pixel.RenderOgl
    theme:                  *Theme
    style:                  ThemeStyle
    keyb:                   Input.Keyboard
    mouse:                  Input.Mouse
    mainSurface:            *Surface
    postedEvents:           Array'(IEvent)
    surfaces:               Array'(*Surface)
    postDestroy:            Array'(*Wnd)
    hookOnEvent:            Array'(*Wnd)
    frameCursor:            Cursor
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
    dirtyTheme:             u32
}

internal impl Application
{
    // Send and process an event
    mtd sendEventPrivate(evt: IEvent, stopAt: #null *Wnd)
    {
        // Global hook events
        foreach hook in me.hookOnEvent:
            hook.itf.onHookEvent(evt)

        if let ptr = evt as QuitEvent
        {
            me.mustQuit = true
            me.quitCode = ptr.quitCode
            return
        }

        let event = cast(*Event) evt

        // Be sure we have a target
        if !event.target:
            return

        // Parent hook events
        var scan = event.target.getOwner()
        while scan
        {
            if scan.hookOnChildEvent and scan.hookOnChildEvent(scan, evt):
                return
            scan = scan.getOwner()
        }

        // Send event to the target
        // If the target does not accept the event, then pass it to the parent
        event.accepted = true
        event.target.processEvent(evt)
        while !event.accepted
        {
            event.target = event.target.owner orelse event.target.parent
            if !event.target or event.target == stopAt:
                break
            event.accepted = true
            event.target.processEvent(evt)
        }
    }

    func skipDisabled(wnd: #null *Wnd)->#null *Wnd
    {
        if !wnd:
            return null
        if wnd.surface.isDisabled:
            return null
        var scan = wnd
        while scan and !scan.isEnabled():
            scan = scan.parent
        return scan
    }

    mtd sendMouseEvents()
    {
        me.mouse.update()

        var mousePos = me.mouse.getPosition()

        // Wnd that will receive the message
        var targetWnd: *Wnd
        if me.mouseCaptureWnd:
            targetWnd = me.mouseCaptureWnd
        else:
            targetWnd = me.getWndAtMouse()
        targetWnd = skipDisabled(targetWnd)

        // If no target window, but one target surface, then we will send
        // to the surfaceWnd (we probably are in the shadow area)
        if !targetWnd
        {
            let targetSurface = me.getSurfaceAt(mousePos.x, mousePos.y)
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
            me.sendEventPrivate(&evt, me.mouseEnterWnd)
            targetWnd = evt.target
        }

        // Mouse enter/leave
        if !targetWnd or targetWnd != me.mouseEnterWnd
        {
            // Enter
            if targetWnd
            {
                evt.kind       = .MouseEnter
                evt.surfacePos = targetWnd.screenToSurface(mousePos)
                evt.target     = targetWnd
                me.sendEventPrivate(&evt, me.mouseEnterWnd)

                if targetWnd.toolTip.length:
                    ToolTip.show(targetWnd, evt.surfacePos, targetWnd.toolTip)
            }

            // Leave
            if me.mouseEnterWnd and targetWnd != me.mouseEnterWnd
            {
                evt.kind       = .MouseLeave
                evt.surfacePos = me.mouseEnterWnd.screenToSurface(mousePos)
                evt.target     = me.mouseEnterWnd
                me.sendEventPrivate(&evt, me.mouseCaptureWnd)

                ToolTip.hide(me.mouseEnterWnd)
            }

            me.mouseEnterWnd = targetWnd
        }

        evt.modifiers = me.keyb.getPressedModifiers()

        // Move
        if targetWnd
        {
            var move = me.mouse.getMove()
            evt.surfacePos = targetWnd.screenToSurface(mousePos)
            evt.kind       = .MouseMove
            evt.move       = move
            targetWnd.sendEvent(&evt)
            if Math.abs(move.x) >= 0.5 or Math.abs(move.y) >= 0.5:
                ToolTip.resetTimer(evt.surfacePos)
        }

        if me.frameCursor.isValid():
            Cursor.apply(me.frameCursor)
        elif targetWnd and targetWnd.cursor.isValid():
            Cursor.apply(targetWnd.cursor)
        else:
            Cursor.apply(Cursor.from(CursorShape.Arrow))
        me.frameCursor.destroy()
        if !targetWnd:
            return

        // Detect changes
        for i in me.mouse.previousState.pressed
        {
            let lastP = me.mouse.previousState.pressed[i]
            let curP  = me.mouse.currentState.pressed[i]
            if !lastP and curP
            {
                if me.mouse.isButtonDoubleClick(cast(Input.MouseButton) i):
                    evt.kind = .MouseDoubleClick
                else
                {
                    evt.kind = .MousePressed
                    if targetWnd.focusStrategy == .MousePressed:
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
        var targetWnd = me.mouseEnterWnd
        targetWnd = skipDisabled(targetWnd)
        if !targetWnd:
            return

        var evt      = MouseEvent{kind: .MouseWheel}
        var mousePos = me.mouse.getPosition()
        evt.surfacePos = targetWnd.screenToSurface(mousePos)
        evt.move.y     = z
        evt.modifiers  = me.keyb.getPressedModifiers()
        targetWnd.sendEvent(&evt)
    }

    mtd sendKeyboardEvents()
    {
        me.keyb.update()

        var targetWnd = me.keybFocusWnd
        targetWnd = skipDisabled(targetWnd)
        if !targetWnd:
            return

        // Modifiers
        let modifiers = me.keyb.getPressedModifiers()

        // Detect changes
        for i in me.keyb.previousState.pressed
        {
            let lastP = me.keyb.previousState.pressed[i]
            let curP  = me.keyb.currentState.pressed[i]
            if !lastP and curP
            {
                var evt = KeyEvent{kind: .KeyPressed}
                evt.key       = cast(Input.Key) i
                evt.modifiers = modifiers
                targetWnd.sendEvent(&evt)

                // Convert key to command (shortcuts)
                if !evt.accepted
                {
                    let id = targetWnd.getKeyShortcut(modifiers, cast(Input.Key) i)
                    if id != null:
                        targetWnd.sendCommandEvent(id)
                }
            }
            elif lastP and !curP
            {
                var evt = KeyEvent{kind: .KeyReleased}
                evt.key       = cast(Input.Key) i
                evt.modifiers = modifiers
                targetWnd.sendEvent(&evt)
            }
        }
    }

    mtd sendRuneEvent(key: Input.Key, r: rune)
    {
        var targetWnd = me.keybFocusWnd
        targetWnd = skipDisabled(targetWnd)
        if !targetWnd:
            return

        var evt = KeyEvent{kind: .Rune}
        evt.char      = r
        evt.key       = key
        evt.modifiers = me.keyb.getPressedModifiers()
        targetWnd.sendEvent(&evt)
    }

    mtd sendPostedEvents()
    {
        // Need to make a copy, in case the posted event triggers a modal for
        me.mutexPostedEvents.lock()
        var copy = me.postedEvents
        me.postedEvents.clear()
        me.mutexPostedEvents.unlock()

        var needUpdateState = false
        foreach it in copy
        {
            let evt = it as Event

            if it is InvalidateEvent:
                evt.target.invalidate()
            else:
                me.sendEvent(it)

            // Special event post process
            if it is CommandEvent:
                needUpdateState = true

            me.toDelete.add({evt, evt.type})
        }

        if needUpdateState:
            me.updateCommandState()
    }

    mtd sendDestroyEvents()
    {
        if me.inModalLoop:
            return
        foreach w in me.postDestroy:
            me.destroyWnd(w)
        me.postDestroy.clear()
    }

    // Update UI state of all surfaces
    mtd updateCommandState()
    {
        foreach s in me.surfaces:
            s.wnd.updateCommandState()
    }

    // Remove and destroy the given surface
    // To cleanly close a surface, you should call surface.destroy()
    mtd destroySurface(surface: *Surface)
    {
        me.surfaces.remove(surface)
        if surface == me.mainSurface:
            me.postQuitEvent()

        if me.modalSurfaces.count and surface == me.modalSurfaces.back():
            me.endModal = true

        surface.destroyNative()
        Memory.delete(surface)
    }

    mtd unregisterWnd(wnd: *Wnd)
    {
        if wnd == me.mouseEnterWnd or wnd.isOwnerOf(me.mouseEnterWnd):
            me.mouseEnterWnd = null
        if wnd == me.mouseCaptureWnd or wnd.isOwnerOf(me.mouseCaptureWnd):
            me.mouseCaptureWnd = null
        if wnd == me.keybFocusWnd or wnd.isOwnerOf(me.keybFocusWnd):
            me.keybFocusWnd = null

        // Be sure there's no timer for that window
        foreach t in me.timers
        {
            if t.target == wnd and !t.toDelete
            {
                me.timersToDelete.add(t)
                t.toDelete = true
            }
        }

        // Remove from various lists
        me.frameEvents.remove(wnd)
        me.hookOnEvent.remove(wnd)

        // Be sure there's no posted event for that window
        foreach e in me.postedEvents
        {
            let ptr = e as Event
            if ptr.target == wnd:
                ptr.target = null
        }
    }

    mtd sendTimerEvents()
    {
        me.duringTimerEvents = true
        foreach t in me.timers
        {
            if t.toDelete:
                continue

            if !t.stopwatch.isStarted:
                t.stopwatch.start()

            if t.stopwatch.elapsedMillisecondsNow() >= t.timeMs
            {
                var evt = TimerEvent{}
                evt.timer = t
                t.target.sendEvent(&evt)
                t.stopwatch.restart()
            }
        }

        me.duringTimerEvents = false

        // Add new registered timers
        foreach t in me.timersToAdd:
            me.timers.add(t)
        me.timersToAdd.clear()

        // Remove some timers
        foreach t in me.timersToDelete
        {
            t.toDelete = false
            me.deleteTimer(t)
        }

        me.timersToDelete.clear()
    }

    mtd sendFrameEvents()
    {
        foreach t in me.frameEvents
        {
            var evt = FrameEvent{}
            t.sendEvent(&evt)
        }
    }

    mtd deletePointers()
    {
        if me.inModalLoop:
            return
        foreach p in me.toDelete:
            Memory.delete(p.item0, p.item1)
        me.toDelete.clear()
    }

    public mtd runFrame()
    {
        // Auto exit
        if me.maxRunFrame and me.timing.frameCount > me.maxRunFrame:
            Env.exit(0)

        me.timing.update()
        Memory.freeTemp()

        ToolTip.update(me.getDt())
        me.sendFrameEvents()
        me.sendTimerEvents()
        me.sendSystemEvents()
        me.sendKeyboardEvents()
        me.sendMouseEvents()
        me.sendPostedEvents()
        me.sendDestroyEvents()

        foreach s in me.surfaces:
            s.paint()

        me.deletePointers()

        me.sigFrame.call(me)
    }

    public mtd doModalLoop(surface: #null *Surface, cb: func||(*Application) = null)->WndId
    {
        me.inModalLoop += 1
        defer me.inModalLoop -= 1

        foreach s in me.surfaces
        {
            if s != surface:
                s.disable()
        }

        if surface
        {
            surface.enable()
            me.modalSurfaces.add(surface)
        }

        while !me.mustQuit and !me.endModal
        {
            me.runFrame()
            if cb:
                cb(me)
        }

        if surface:
            me.modalSurfaces.removeBack()

        if me.modalSurfaces.count
        {
            let back = me.modalSurfaces.back()
            back.enable()
            back.wnd.view.setFocus()
        }
        else
        {
            foreach s, idx in me.surfaces
            {
                if s != surface
                {
                    s.enable()
                    s.wnd.view.setFocus()
                }
            }
        }

        if surface:
            surface.destroy()

        me.endModal = false
        return me.endModalExit
    }
}

impl Application
{
    mtd opDrop()
    {
        foreach it in me.surfaces:
            it.destroy()
        me.sendDestroyEvents()

        foreach it in me.postedEvents
        {
            let ptr = it as Event
            me.toDelete.add({ptr, ptr.type})
        }

        me.deletePointers()
        me.theme.freeDefault()
    }

    // Get application variables
    #[Swag.Inline]
    {
        mtd getRenderer() => &me.renderer
        mtd getKeyboard() => &me.keyb
        mtd getMouse()    => &me.mouse
        mtd const getDt() => me.timing.dt
    }

    // Create a main surface, and run
    func runSurface(x, y, width, height: s32, title: string = null, view: *Wnd = null, borders = true, canSize = true, hook: HookEvent = null, init: func(*Application) = null)
    {
        var app: Application

        var flags = SurfaceFlags.OverlappedWindow
        if !canSize:
            flags &= ~.Sizeable
        if !borders:
            flags &= ~.Border
        let surface = assume app.createSurface(x, y, width, height, flags, view, hook)
        surface.setTitle(title)
        if init:
            init(&app)
        surface.show()
        app.run()
    }

    // Creates a new surface
    mtd createSurface(x, y, width, height: s32, flags = SurfaceFlags.OverlappedWindow, view: *Wnd = null, hook: HookEvent = null, surfaceID: string = null)->*Surface throw
    {
        let surface = Memory.new'Surface()
        surface.app       = me
        surface.position  = {x, y, width, height}
        surface.surfaceID = surfaceID

        if !me.mainSurface
        {
            me.theme = Memory.new'Theme()
            me.style.set(me.theme)
        }

        try Surface.create(surface, x, y, width, height, flags, me.mainSurface)

        if !me.mainSurface
        {
            me.renderer.setCurrentContext(surface.rc)
            me.renderer.init()
            me.theme.setupDefault(&me.renderer)
            me.mainSurface = surface
        }

        me.surfaces.add(surface)

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
        me.configPath = path
    }

    // Associate a configuration file
    mtd setAppIcon(img: Pixel.Image)
    {
        me.appIcon = img
    }

    // Call it when the theme has changed
    mtd themeHasChanged()
    {
        func updateTheme(wnd: *Wnd)
        {
            wnd.style.dirty = true
            wnd.computeStyle()
            wnd.invalidate()
            foreach c in wnd.childs:
                updateTheme(c)

            var evt = NotifyEvent{}
            evt.accepted = false
            evt.ntfyKind = .ThemeChanged
            wnd.sendEvent(&evt)
        }

        me.dirtyTheme += 1
        foreach s in me.surfaces:
            s.wnd.updateTheme()
    }

    internal mtd serializeState(ser: *Serializer) throw
    {
        ser.beginSection(.Unordered)
        foreach s in me.surfaces:
            s.serializeState(ser)
        ser.endSection()
    }

    // Save the application state
    mtd saveState() throw
    {
        if me.configPath.isEmpty():
            return

        var buf:  ConcatBuffer
        var save: Encoder'Write.TagBin
        var ser:  Serializer
        ser.startWrite(save, &buf)
        me.serializeState(&ser)
        ser.end()

        File.writeAllBytes(me.configPath, buf)
    }

    // Load the application state
    mtd loadState() throw
    {
        if me.configPath.isEmpty():
            return

        var buf = File.readAllBytes(me.configPath)

        var load: Decoder'Read.TagBin
        var ser:  Serializer
        ser.startRead(load, buf.toSlice())
        me.serializeState(&ser)
        ser.end()
    }

    // Return the captured wnd for mouse
    mtd getMouseCapture()->*Wnd
    {
        return me.mouseCaptureWnd
    }

    // Destroy a window
    // To cleanly close a window, you should call wnd.destroy()
    mtd destroyWnd(wnd: *Wnd)
    {
        var evt = DestroyEvent{}
        wnd.sendEvent(&evt)

        me.unregisterWnd(wnd)

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
            me.toDelete.add({wnd, wnd.type})
        }
    }

    // Returns the window under the given point
    mtd getWndAtMouse()->#null *Wnd
    {
        var mousePos      = me.mouse.getPosition()
        let targetSurface = me.getSurfaceAt(mousePos.x, mousePos.y)
        if !targetSurface:
            return null
        targetSurface.screenToSurface(&mousePos)
        let targetWnd = targetSurface.wnd.getWndAt(mousePos.x, mousePos.y)
        return targetWnd
    }

    // Set activated state
    mtd setActivated(activated: bool)
    {
        if me.isActivated == activated:
            return
        me.isActivated = activated
        foreach s in me.modalSurfaces:
            s.bringToTop()
        me.invalidate()
    }

    // Force all surfaces to be painted
    mtd invalidate()
    {
        foreach s in me.surfaces:
            s.invalidate()
    }

    // Ask to exit the application
    mtd postQuitEvent(quitCode: s32 = 0)
    {
        let evt = Event.create'QuitEvent()
        evt.quitCode = quitCode
        me.postEvent(evt)
    }

    // Post a new event (thread safe)
    mtd postEvent(event: IEvent)
    {
        Sync.scopedLock(&me.mutexPostedEvents)
        me.postedEvents.add(event)
    }

    mtd sendEvent(event: IEvent)
    {
        me.sendEventPrivate(event, null)
    }

    // Register a new timer for the given 'target'
    mtd addTimer(time: Time.Duration, target: *Wnd)->*Timer
    {
        let timer = Memory.new'Timer()
        timer.timeMs = time.toMs()
        timer.target = target
        if me.duringTimerEvents:
            me.timersToAdd.add(timer)
        else:
            me.timers.add(timer)
        return timer
    }

    // Delete and unregister timer
    mtd deleteTimer(timer: *Timer)
    {
        if me.duringTimerEvents
        {
            Debug.assert(!timer.toDelete)
            timer.toDelete = true
            me.timersToDelete.add(timer)
        }
        else
        {
            Debug.assert(!timer.toDelete)
            Memory.delete(timer)
            me.timers.remove(timer)
        }
    }

    // Register a window to receive a FrameEvent each running for
    mtd registerFrameEvent(target: *Wnd)
    {
        Debug.assert(!me.frameEvents.contains(target))
        me.frameEvents.add(target)
    }

    // Unregister a window that receives a FrameEvent each running for
    mtd unregisterFrameEvent(target: *Wnd)
    {
        me.frameEvents.remove(target)
    }

    // Register a window to receive hook events
    mtd registerHookEvents(target: *Wnd)
    {
        Debug.assert(!me.hookOnEvent.contains(target))
        me.hookOnEvent.add(target)
    }

    // Unregister a window to receive hook events
    mtd unregisterHookEvents(target: *Wnd)
    {
        me.hookOnEvent.remove(target)
    }

    // Push a new mouse cursor
    mtd setFrameCursor(cursor: Cursor)
    {
        me.frameCursor = cursor
    }

    // Run until exit
    #[Swag.Discardable]
    mtd run()->s32
    {
        foreach s in me.surfaces
        {
            var evt = FrameEvent{}
            evt.firstFrame = true
            s.wnd.view.sendEvent(&evt)
        }

        while !me.mustQuit:
            me.runFrame()

        return me.quitCode
    }

    // End current modal for
    mtd exitModal(exitId: string = "")
    {
        me.endModal     = true
        me.endModalExit = exitId
    }

    mtd getFileIcon(name: string, small = true)->Icon      => me.getFileDirectoryIcon(name, false, small)
    mtd getDirectoryIcon(name: string, small = true)->Icon => me.getFileDirectoryIcon(name, true, small)

    // Enable or disable the processing of hot keys
    mtd enableHotKeys(state = true)
    {
        me.hotKeysDisabled = !state
    }

    // Disable the processing of hot keys
    mtd disableHotKeys()
    {
        me.hotKeysDisabled = true
    }

    // Register a global os key action
    #[Swag.Overload]
    mtd registerHotKey(mdf: Input.KeyModifiers, key: Input.Key, wndId: WndId, target: *Wnd) throw
    {
        foreach &c in me.hotKeys
        {
            if c.mdf == mdf and c.key == key
            {
                c.id     = wndId
                c.target = target
                return
            }
        }

        me.hotKeys.add({mdf, key, wndId, target})
        registerHotKey(mdf, key)
    }

    // Get the id associated with a shortcut. null if none
    mtd getHotKeyShortcut(mdf: Input.KeyModifiers, key: Input.Key)->{ id: WndId, wnd: *Wnd }
    {
        foreach c in me.hotKeys
        {
            if c.mdf == mdf and c.key == key:
                return {c.id, c.target}
        }

        return {null, null}
    }

    // Get the name associated with a given id shortcut
    mtd getHotKeyShortcutNameFor(wndId: WndId)->String
    {
        var result: retval
        foreach c in me.hotKeys
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
