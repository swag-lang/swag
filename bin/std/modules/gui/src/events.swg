#global public
using Core

enum EventKind
{
    None
    Resize
    Paint
}

struct Event
{
    kind: EventKind = None
}

struct PaintEvent
{
    using base: Event
}

struct ResizeEvent
{
    using base: Event
    rect: Math.Rectangle
}