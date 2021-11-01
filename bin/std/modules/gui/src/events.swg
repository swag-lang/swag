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
    Quit
    SysCommand
}

struct Event
{
    kind:       EventKind = None
    type:       typeinfo
    target:     *Wnd
    accepted:   bool
}

impl Event
{
    func(T) create()->*T
    {
        res := Memory.new'T()
        res.type = @typeof(T)
        return res
    }

    func(T) create(kind: EventKind)->*T
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

struct KeyEvent
{
    using base: Event
    modifiers:  Input.KeyModifiers = .None
    key:        Input.Key = ?
}

struct MouseEvent
{
    using base: Event
    button:     Input.MouseButton = ?
    surfacePos: Math.Point
    pos:        Math.Point
    move:       Math.Point
}

struct QuitEvent
{
    using base: Event{kind: EventKind.Quit}
    quitCode: s32
}

enum SysCommandKind
{
    Maximize
    Minimize
    Restore
    Close
}

struct SysCommandEvent
{
    using base: Event{kind: EventKind.SysCommand}
    sysKind: SysCommandKind = ?
}