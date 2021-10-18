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
    kind:   EventKind = None
    type:   typeinfo
    target: *Wnd
}

impl Event
{
    func(T) alloc(kind: EventKind)->*T
    {
        res := Memory.new'T()
        res.type = @typeof(T)
        res.kind = kind
        return res
    }
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

#[Swag.EnumFlags]
enum KeyModifiers
{
    None
    Shift
    Control
    Alt
}

struct KeyEvent
{
    using base: Event
    modifiers:  KeyModifiers = ?//.None
    key:        Input.Key = ?
}

struct QuitEvent
{
    using base: Event
    quitCode: s32
}