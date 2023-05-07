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
    CommandUI
    CommandState
    Frame
    SerializeState
    Show
    Hide
    Invalidate
    SysUser
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

struct CommandUIEvent
{
    #[Swag.EnumFlags]
    enum SetFlags
    {
        Zero = 0
        Disabled
        Icon
        Shortcut
        Checked
        Name
        LongName
        ToolTip
        Hidden
    }

    using base: Event{kind: EventKind.CommandUI}
    id:         WndId
    source:     *Wnd

    icon:       Icon
    name:       String
    longName:   String
    shortcut:   String
    toolTip:    String
    setFlags:   SetFlags = Zero
    disabled:   bool
    checked:    bool
    hidden:     bool
}

struct TimerEvent
{
    using base: Event{kind: EventKind.Timer}
    timer:      *Timer
}

struct FrameEvent
{
    using base: Event{kind: EventKind.Frame}
    firstFrame: bool
}

struct CreateEvent
{
    using base: Event{kind: EventKind.Create}
}

struct StateEvent
{
    using base: Event
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
    key:        Input.Key = undefined
    char:       rune
}

struct MouseEvent
{
    using base: Event
    button:     Input.MouseButton = undefined
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

struct SysUserEvent
{
    using base: Event{kind: EventKind.SysUser}
    userMsg:    u64
    param0:     u64
    param1:     u64
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
        User
    }

    sysKind: Kind = undefined
}

struct SetThemeEvent
{
    using base: Event{kind: EventKind.SetTheme}
    style:  *ThemeStyle
    parent: *ThemeStyle
}

struct SerializeStateEvent
{
    using base: Event{kind: EventKind.SerializeState}
    ser: *Serialization.Serializer
}