#global public
using Core

struct Application
{
    renderer:       Pixel.RenderOgl
    keyb:           Input.Keyboard
    mainSurface:    *Surface
    postedEvents:   Array'(*Event)
    mustQuit:       bool
    quitCode:       s32
}

impl Application
{
    // Get application variables
    mtd getRenderer() => &renderer
    mtd getKeyboard() => &keyb

    // Creates a new surface
    mtd createSurface(title: string, x, y, width, height: s32, flags: SurfaceFlags, onEvent: func(*Wnd, *Event) = null)->*Surface throw
    {
        surface := try Surface.create(self, title, x, y, width, height, flags, onEvent)
        mainSurface = surface
        evt := CreateEvent{kind: EventKind.Create}
        evt.target = &surface.wnd
        sendEvent(&evt)
        return surface
    }

    // Ask to exit the application
    mtd postQuitEvent(quitCode: s32 = 0)
    {
        evt := Event.alloc'QuitEvent(EventKind.Quit)
        evt.quitCode = #scopefct.quitCode
        postEvent(evt)
    }

    mtd postEvent(event: *Event)
    {
        postedEvents.add(event)
    }

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
        event.target.sendEvent(event)
    }

    protected mtd sendKeyboardEvents()
    {
        keyb.update()

        target := &mainSurface.wnd

        loop i: keyb.previousState.pressed
        {
            lastP := keyb.previousState.pressed[i]
            curP  := keyb.currentState.pressed[i]
            if !lastP and curP
            {
                evt := KeyEvent{kind: EventKind.KeyPressed}
                evt.key = cast(Input.Key) i
                evt.target = target
                sendEvent(&evt)
            }
            else if lastP and !curP
            {
                evt := KeyEvent{kind: EventKind.KeyReleased}
                evt.key = cast(Input.Key) i
                evt.target = target
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
            sendPostedEvents()

            mainSurface.invalidate()
        }

        return quitCode
    }
}