#global public
using Core

enum EventKind
{
    None
    Create
    Resize
    Paint
    Keyboard
}

struct Event
{
    kind: EventKind = None
}

struct CreateEvent
{
    using base: Event
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

struct KeyboardEvent
{
    using base: Event
    pressed:    bool
    key:        Input.Key = ?
}