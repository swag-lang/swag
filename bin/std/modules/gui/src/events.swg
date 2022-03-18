#global public
using Core

enum EventKind
{
    None
    Create
    Destroy
    Resize
    Move
    PrePaint
    Paint
    PostPaint
    KeyPressed
    KeyReleased
    Rune
    MouseAccept
    MousePressed
    MouseReleased
    MouseDoubleClick
    MouseMove
    MouseEnter
    MouseLeave
    MouseWheel
    Quit
    SysCommand
    SetTheme
    Timer
    SetFocus
    KillFocus
    Command
    Frame
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

struct CommandEvent
{
    using base: Event{kind: EventKind.Command}
    id:         WndId
    source:     *Wnd
}

struct TimerEvent
{
    using base: Event{kind: EventKind.Timer}
    timer:      *Timer
}

struct FrameEvent
{
    using base: Event{kind: EventKind.Frame}
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
    modifiers:  Input.KeyModifiers = None
    key:        Input.Key = ?
    char:       rune
}

struct MouseEvent
{
    using base: Event
    button:     Input.MouseButton = ?
    surfacePos: Math.Point
    move:       Math.Point
    modifiers:  Input.KeyModifiers = None
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