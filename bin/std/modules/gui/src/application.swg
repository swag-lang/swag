#global public
using Core

struct Application
{
    renderer:       Pixel.RenderOgl
    keyb:           Input.Keyboard
    mainSurface:    *Surface
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
        surface.wnd.sendEvent(evt)
        return surface
    }

    protected mtd sendEvent(event: *Event)
    {
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
                sendEvent(evt)
            }
            else if lastP and !curP
            {
                evt := KeyEvent{kind: EventKind.KeyReleased}
                evt.key = cast(Input.Key) i
                evt.target = target
                sendEvent(evt)
            }
        }
    }

    // Run until exit
    mtd run()
    {
        loop
        {
            Memory.freeTemp()

            if !sendSystemEvents()
                return
            sendKeyboardEvents()

            mainSurface.invalidate()
        }
    }
}