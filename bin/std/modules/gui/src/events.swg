#global public
using Core

enum EventKind
{
    None
    Create
    Destroy
    Resize
    Move
    Paint
    KeyPressed
    KeyReleased
    Char
    MouseAccept
    MousePressed
    MouseReleased
    MouseDoubleClick
    MouseMove
    MouseEnter
    MouseLeave
    Quit
    SysCommand
    SetTheme
    Timer
    SetFocus
    KillFocus
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

struct TimerEvent
{
    using base: Event{kind: EventKind.Timer}
    timer:      *Timer
}

struct CreateEvent
{
    using base: Event{kind: EventKind.Create}
}

struct DestroyEvent
{
    using base: Event{kind: EventKind.Destroy}
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
    char:       rune
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

struct FocusEvent
{
    using base: Event
    other:      *Wnd
}

struct SysCommandEvent
{
    using base: Event{kind: EventKind.SysCommand}

    enum Kind
    {
        Minimize
        Maximize
        Restore
        Close
    }

    sysKind: Kind = ?
}

struct SetThemeEvent
{
    using base: Event{kind: EventKind.SetTheme}
    style:  *Style
    parent: *Style
}