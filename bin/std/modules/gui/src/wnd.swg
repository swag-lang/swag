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