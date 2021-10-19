#global public
using Core, Pixel

interface IWnd
{
    onEvent:        func(self, *Event)
    onResizeEvent:  func(self, *ResizeEvent)
    onPaintEvent:   func(self, *PaintEvent)
    onKeyEvent:     func(self, *KeyEvent)
}

struct Wnd
{
    rect:     Math.Rectangle
    surface:  *Surface

    hookOnEvent:        func(*Wnd, *Event)
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
        case Resize:
            if hookOnResizeEvent
                hookOnResizeEvent(self, cast(*ResizeEvent) evt)
            else
                onResizeEvent(cast(*ResizeEvent) evt)

        case Paint:
            if hookOnPaintEvent
                hookOnPaintEvent(self, cast(*PaintEvent) evt)
            else
                onPaintEvent(cast(*PaintEvent) evt)

        case KeyPressed, KeyReleased:
            if hookOnKeyEvent
                hookOnKeyEvent(self, cast(*KeyEvent) evt)
            else
                onKeyEvent(cast(*KeyEvent) evt)
        }
    }

    mtd onResizeEvent(evt: *ResizeEvent)
    {
        rect.width = evt.rect.width
        rect.height = evt.rect.height
    }

    mtd onPaintEvent(evt: *PaintEvent)
    {
    }

    mtd onKeyEvent(evt: *KeyEvent)
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