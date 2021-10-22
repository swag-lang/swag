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
    MousePressed
    MouseReleased
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

struct CloseEvent
{
    using base: Event
}

struct PaintEvent
{
    using base: Event
    painter:    *Pixel.Painter
    renderer:   *Pixel.RenderOgl
}

struct ResizeEvent
{
    using base: Event
    rect: Math.Rectangle
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
}

struct QuitEvent
{
    using base: Event
    quitCode: s32
}