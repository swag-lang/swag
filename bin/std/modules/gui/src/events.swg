#global public
using Core

enum EventKind
{
    None
    Create
    Resize
    Paint
    KeyPressed
    KeyReleased
    Close
    Quit
}

struct Event
{
    kind: EventKind = None
    target: *Wnd
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

struct KeyEvent
{
    using base: Event
    key:        Input.Key = ?
}

struct QuitEvent
{
    using base: Event
    quitCode: s32
}