#global public
using Core, Pixel

interface IWnd
{
    onEvent: func(self, *Event)
}

struct Wnd
{
    rect: Math.Rectangle
    hookOnEvent: func(*IWnd, *Event)
}

impl IWnd for Wnd
{
    mtd onEvent(event: *Event)
    {
        if hookOnEvent
        {
            hookOnEvent(self, event)
        }
    }
}