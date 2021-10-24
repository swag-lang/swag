#global public
using Core

enum EventKind
{
    None
    Create
    Resize
    Move
    Paint
    KeyPressed
    KeyReleased
    MousePressed
    MouseReleased
    MouseMove
    MouseEnter
    MouseLeave
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
    using base: Event{kind: EventKind.Create}
}

struct CloseEvent
{
    using base: Event{kind: EventKind.Close}
}

struct PaintEvent
{
    using base: Event{kind: EventKind.Paint}
    bc: *PaintContext
}

struct ResizeEvent
{
    using base: Event{kind: EventKind.Resize}
    oldSize: Math.Point
}

struct MoveEvent
{
    using base: Event{kind: EventKind.Move}
    oldPos: Math.Point
}

#[Swag.EnumFlags]
enum KeyModifiers
{
    None = 0
    Shift
    Control
    Alt
}

struct KeyEvent
{
    using base: Event
    modifiers:  KeyModifiers = .None
    key:        Input.Key = ?
}

struct MouseEvent
{
    using base: Event
    button:     Input.MouseButton = ?
    surfacePos: Math.Point
    pos:        Math.Point
}

struct QuitEvent
{
    using base: Event
    quitCode: s32
}