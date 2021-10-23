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
    mustQuit:       bool
    quitCode:       s32
    mouseEnterWnd:  *Wnd
}

impl Application
{
    // Get application variables
    mtd getRenderer() => &renderer
    mtd getKeyboard() => &keyb

    // Creates a new surface
    mtd createSurface(title: string, x, y, width, height: s32, flags: SurfaceFlags = SurfaceFlags.OverlappedWindow)->*Surface throw
    {
        surface := try Surface.create(self, title, x, y, width, height, flags)

        if !mainSurface
        {
            theme.setup(&renderer)
            mainSurface = surface
        }

        surfaces.add(surface)

        surface.wnd.surface = surface
        surface.wnd.itf = cast(IWnd) surface.wnd
        surface.wnd.colorBackground.themeColor = &theme.colorWndBackground

        evt := CreateEvent{kind: EventKind.Create}
        surface.wnd.sendEvent(&evt)

        return surface
    }

    // Returns the surface under the given point
    mtd getSurfaceAt(x, y: f32)->*Surface
    {
        visit s: surfaces
        {
            if s.pos.contains(x, y)
                return s
        }

        return null
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

    // Ask to exit the application
    mtd postQuitEvent(quitCode: s32 = 0)
    {
        evt := Event.alloc'QuitEvent(EventKind.Quit)
        evt.quitCode = #scopefct.quitCode
        postEvent(evt)
    }

    // Post a new event
    mtd postEvent(event: *Event)
    {
        postedEvents.add(event)
    }

    // Send and process an event
    mtd sendEvent(event: *Event)
    {
        if event.kind == .Quit
        {
            evt := cast(*QuitEvent) event
            mustQuit = true
            quitCode = evt.quitCode
            return
        }

        Debug.assert(event.target != null)
        event.target.processEvent(event)
    }

    protected mtd sendMouseEvents()
    {
        mouse.update()

        mousePos  := mouse.getPosition()
        targetWnd := getWndAt(mousePos.x, mousePos.y)

        // Mouse leave
        if !targetWnd or targetWnd != mouseEnterWnd
        {
            if mouseEnterWnd
            {
                evt := MouseEvent{kind: EventKind.MouseLeave}
                evt.target = mouseEnterWnd
                evt.surfacePos = mousePos
                sendEvent(&evt)
            }

            mouseEnterWnd = targetWnd
            if !targetWnd
                return

            evt := MouseEvent{kind: EventKind.MouseEnter}
            evt.target = mouseEnterWnd
            evt.surfacePos = mousePos
            sendEvent(&evt)
        }

        // Move
        move := mouse.getMove()
        if Math.abs(move.x) >= 0.5 or Math.abs(move.y) >= 0.5
        {
            evt := MouseEvent{kind: EventKind.MouseMove}
            evt.target = targetWnd
            evt.surfacePos = mousePos
            sendEvent(&evt)
        }

        // Detect changes
        loop i: mouse.previousState.pressed
        {
            lastP := mouse.previousState.pressed[i]
            curP  := mouse.currentState.pressed[i]
            if !lastP and curP
            {
                evt := MouseEvent{kind: EventKind.MousePressed}
                evt.button = cast(Input.MouseButton) i
                evt.target = targetWnd
                evt.surfacePos = mousePos
                sendEvent(&evt)
            }
            else if lastP and !curP
            {
                evt := MouseEvent{kind: EventKind.MouseReleased}
                evt.button = cast(Input.MouseButton) i
                evt.target = targetWnd
                evt.surfacePos = mousePos
                sendEvent(&evt)
            }
        }
    }

    protected mtd sendKeyboardEvents()
    {
        keyb.update()

        target := &mainSurface.wnd

        // Modifiers
        var modifiers: KeyModifiers = .None
        if keyb.isKeyPressed(Input.Key.LShift)   modifiers |= .Shift
        if keyb.isKeyPressed(Input.Key.RShift)   modifiers |= .Shift
        if keyb.isKeyPressed(Input.Key.LControl) modifiers |= .Control
        if keyb.isKeyPressed(Input.Key.RControl) modifiers |= .Control
        if keyb.isKeyPressed(Input.Key.LMenu)    modifiers |= .Alt
        if keyb.isKeyPressed(Input.Key.RMenu)    modifiers |= .Alt

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
        visit evt: postedEvents
        {
            sendEvent(evt)
            Memory.delete(evt, evt.type)
        }

        postedEvents.clear()
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
        }

        return quitCode
    }
}