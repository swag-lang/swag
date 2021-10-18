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
    mtd getRenderer() => &renderer

    // Creates a new surface
    mtd createSurface(title: string, x, y, width, height: s32, flags: SurfaceFlags, onEvent: func(*Wnd, *Event) = null)->*Surface throw
    {
        surface := try Surface.create(self, title, x, y, width, height, flags, onEvent)
        mainSurface = surface
        evt := CreateEvent{kind: EventKind.Create}
        surface.wnd.sendEvent(evt)
        return surface
    }

    protected mtd sendKeyboardEvents()
    {
        keyb.update()
        loop i: keyb.previousState.pressed
        {
            if !keyb.previousState.pressed[i] and keyb.currentState.pressed[i]
            {
                evt := KeyboardEvent{kind: EventKind.Keyboard, pressed: true}
                evt.key = cast(Input.Key) i
                mainSurface.wnd.sendEvent(evt)
            }
        }
    }

    // Run until exit
    mtd run()
    {
        loop
        {
            Memory.freeTemp()

            if !Surface.messageLoop()
                return

            sendKeyboardEvents()
            mainSurface.invalidate()
        }
    }
}