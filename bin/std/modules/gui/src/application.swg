#global public
using Core

struct Application
{
    renderer:       Pixel.RenderOgl
    theme:          Theme
    keyb:           Input.Keyboard
    mouse:          Input.Mouse
    mainSurface:    *Surface
    postedEvents:   Array'(*Event)
    surfaces:       Array'(*Surface)
    postDestroy:    Array'(*Wnd)
    cursorStack:    Array'(Cursor)
    mustQuit:       bool
    isActivated:    bool
    quitCode:       s32

    mouseEnterWnd:      *Wnd
    mouseCaptureWnd:    *Wnd
}

impl Application
{
    // Get application variables
    mtd getRenderer() => &renderer
    mtd getKeyboard() => &keyb

    // Creates a new surface
    mtd createSurface(x, y, width, height: s32, flags: SurfaceFlags = SurfaceFlags.Zero)->*Surface throw
    {
        surface := Memory.new'Surface()
        surface.app = self
        surface.position = @{x, y, width, height}

        try Surface.create(self, surface, x, y, width, height, flags, mainSurface)
        if !mainSurface
        {
            renderer.setCurrentContext(surface.rc)
            renderer.init()
            theme.setupDefault(&renderer)
            mainSurface = surface
        }

        surfaces.add(surface)

        surface.wnd.surface = surface
        surface.wnd.itf = cast(IWnd) surface.wnd

        evt := CreateEvent{}
        surface.wnd.sendEvent(&evt)

        return surface
    }

    // Remove and destroy the given surface
    // To cleanly close a surface, you should call surface.destroy()
    protected mtd destroySurface(surface: *Surface)
    {
        surfaces.remove(surface)
        if surface == mainSurface
            postQuitEvent()
        surface.destroyNative()
        Memory.delete(surface)
    }

    // Destroy a window
    // To cleanly close a window, you should call wnd.destroy()
    protected mtd destroyWnd(wnd: *Wnd)
    {
        if wnd == mouseEnterWnd
            mouseEnterWnd = null
        if wnd == mouseCaptureWnd
            mouseCaptureWnd = null

        visit e: postedEvents
        {
            if e.target == wnd
                e.target = null
        }

        if wnd.isAllocated
            Memory.delete(@dataof(wnd.itf), @kindof(wnd.itf))
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
        evt.quitCode = #scopefct.quitCode
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

    // Send and process an event
    protected mtd sendEventPrivate(event: *Event, stopAt: *Wnd)
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

    protected mtd sendMouseEvents()
    {
        mouse.update()

        mousePos := mouse.getPosition()

        // Wnd that will receive the message
        var targetWnd: *Wnd
        if mouseCaptureWnd
            targetWnd = mouseCaptureWnd
        else
            targetWnd = getWndAt(mousePos.x, mousePos.y)

        evt := MouseEvent{}

        // Mouse enter/leave
        if !targetWnd or targetWnd != mouseEnterWnd
        {
            // Enter
            if targetWnd
            {
                evt.kind = .MouseEnter
                evt.surfacePos = targetWnd.screenToSurface(mousePos)
                evt.pos = targetWnd.surfaceToLocal(evt.surfacePos)
                evt.target = targetWnd
                sendEventPrivate(&evt, mouseEnterWnd)
                targetWnd = evt.target
            }

            // Leave
            if mouseEnterWnd and targetWnd != mouseEnterWnd
            {
                evt.kind = .MouseLeave
                evt.surfacePos = mouseEnterWnd.screenToSurface(mousePos)
                evt.pos = mouseEnterWnd.surfaceToLocal(evt.surfacePos)
                mouseEnterWnd.sendEvent(&evt)
            }

            mouseEnterWnd = targetWnd
            if !targetWnd
                return
        }

        changeCursor()

        evt.surfacePos = targetWnd.screenToSurface(mousePos)
        evt.pos        = targetWnd.surfaceToLocal(evt.surfacePos)

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
                evt.kind = .MousePressed
                evt.button = cast(Input.MouseButton) i
                targetWnd.sendEvent(&evt)
            }
            else if lastP and !curP
            {
                evt.kind = .MouseReleased
                evt.button = cast(Input.MouseButton) i
                targetWnd.sendEvent(&evt)
            }
        }
    }

    protected mtd sendKeyboardEvents()
    {
        keyb.update()

        target := &mainSurface.wnd

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
                evt.target = target
                evt.modifiers = modifiers
                sendEvent(&evt)
            }
            else if lastP and !curP
            {
                evt := KeyEvent{kind: EventKind.KeyReleased}
                evt.key = cast(Input.Key) i
                evt.target = target
                evt.modifiers = modifiers
                sendEvent(&evt)
            }
        }
    }

    protected mtd sendPostedEvents()
    {
        for i := 0; i < postedEvents.count; i += 1
        {
            evt := postedEvents[i]
            sendEvent(evt)
            Memory.delete(evt, evt.type)
        }

        postedEvents.clear()
    }

    protected mtd sendDestroyEvents()
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

    // Run until exit
    #[Swag.Discardable]
    mtd run()->s32
    {
        while !mustQuit
        {
            Memory.freeTemp()

            sendSystemEvents()
            sendKeyboardEvents()
            sendMouseEvents()
            sendPostedEvents()
            sendDestroyEvents()

            visit s:
                surfaces s.paint()
        }

        return quitCode
    }
}