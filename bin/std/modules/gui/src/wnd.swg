#global public
using Core, Pixel

interface IWnd
{
    onEvent: func(self, *Event)
}

struct Wnd
{
    rect:           Math.Rectangle
    surface:        *Surface
    hookOnEvent:    func(*Wnd, *Event)
}

impl IWnd for Wnd
{
    mtd onEvent(event: *Event)
    {
        switch event.kind
        {
        case Resize:
            evt := cast(*ResizeEvent) event
            rect.width = evt.rect.width
            rect.height = evt.rect.height
        }
    }
}

impl Wnd
{
    mtd sendEvent(event: *Event)
    {
        if hookOnEvent
            hookOnEvent(self, event)
        else
            self.IWnd.onEvent(event)
    }
}