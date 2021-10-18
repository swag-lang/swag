#global public
using Core, Pixel

interface IWnd
{
    onEvent: func(self, *Event)
}

struct Wnd
{
    rect: Math.Rectangle
    hookOnEvent: func(*Wnd, *Event)
}

impl IWnd for Wnd
{
    mtd onEvent(event: *Event)
    {
    }
}