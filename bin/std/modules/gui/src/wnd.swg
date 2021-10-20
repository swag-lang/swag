#global public
using Core, Pixel

interface IWnd
{
    onCreateEvent:  func(self, *CreateEvent)
    onCloseEvent:   func(self, *CloseEvent)
    onEvent:        func(self, *Event)
    onResizeEvent:  func(self, *ResizeEvent)
    onPaintEvent:   func(self, *PaintEvent)
    onKeyEvent:     func(self, *KeyEvent)
}

struct Wnd
{
    itf:        IWnd
    surface:    *Surface
    rect:       Math.Rectangle
    parent:     *Wnd
    childs:     Array'(*Wnd)

    hookOnEvent:        func(*Wnd, *Event)
    hookOnCreateEvent:  func(*Wnd, *CreateEvent)
    hookOnCloseEvent:   func(*Wnd, *CloseEvent)
    hookOnResizeEvent:  func(*Wnd, *ResizeEvent)
    hookOnPaintEvent:   func(*Wnd, *PaintEvent)
    hookOnKeyEvent:     func(*Wnd, *KeyEvent)
}

impl IWnd for Wnd
{
    mtd onEvent(evt: *Event)
    {
        switch evt.kind
        {
        case Create:
            if hookOnCreateEvent
                hookOnCreateEvent(self, cast(*CreateEvent) evt)
            else
                itf.onCreateEvent(cast(*CreateEvent) evt)

        case Close:
            if hookOnCloseEvent
                hookOnCloseEvent(self, cast(*CloseEvent) evt)
            else
                itf.onCloseEvent(cast(*CloseEvent) evt)

        case Resize:
            if hookOnResizeEvent
                hookOnResizeEvent(self, cast(*ResizeEvent) evt)
            else
                itf.onResizeEvent(cast(*ResizeEvent) evt)

        case Paint:
            if hookOnPaintEvent
                hookOnPaintEvent(self, cast(*PaintEvent) evt)
            else
                itf.onPaintEvent(cast(*PaintEvent) evt)

        case KeyPressed, KeyReleased:
            if hookOnKeyEvent
                hookOnKeyEvent(self, cast(*KeyEvent) evt)
            else
                itf.onKeyEvent(cast(*KeyEvent) evt)
        }
    }

    mtd onResizeEvent(evt: *ResizeEvent)
    {
        rect.width = evt.rect.width
        rect.height = evt.rect.height
    }

    mtd onPaintEvent(evt: *PaintEvent)
    {
        visit c: childs
            c.itf.onPaintEvent(evt)
    }

    mtd onKeyEvent(evt: *KeyEvent)
    {
    }

    mtd onCreateEvent(evt: *CreateEvent)
    {
    }

    mtd onCloseEvent(evt: *CloseEvent)
    {
    }
}

impl Wnd
{
    mtd postQuitEvent()
    {
        surface.app.postQuitEvent()
    }

    mtd sendEvent(event: *Event)
    {
        if hookOnEvent
            hookOnEvent(self, event)
        else
            itf.onEvent(event)
    }
}