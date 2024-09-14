#global public
using Core

interface IEvent
{
}

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
    CanSetFocus
    CanKillFocus
    Command
    ComputeCommandState
    ApplyCommandState
    Frame
    SerializeState
    Show
    Hide
    Invalidate
    SysUser
    Notify
}

struct Event
{
    kind:         EventKind = None
    type:         typeinfo
    target:       *Wnd
    accepted:     bool
}

impl Event
{
    #[Swag.Overload]
    func(T) create()->*T
    {
        let res = Memory.new'T()
        res.type = #typeof(T)
        return res
    }

    #[Swag.Overload]
    func(T) create(kind: EventKind)->*T
    {
        let res = Memory.new'T()
        res.type = #typeof(T)
        res.kind = kind
        return res
    }
}

impl IEvent for Event
{
}


struct CommandEvent
{
    using base:     Event{kind: EventKind.Command}
    id:             WndId
    source:         *Wnd
}

impl IEvent for CommandEvent
{
}

struct CommandStateEvent
{
    #[Swag.EnumFlags]
    enum SetFlags
    {
        Zero     = 0
        Disabled
        Icon
        Shortcut
        Checked
        Name
        LongName
        ToolTip
        Hidden
    }
    using base:     Event{kind: EventKind.ComputeCommandState}
    id:             WndId
    source:         *Wnd

    icon:           Icon
    name:           String
    longName:       String
    shortcut:       String
    toolTip:        String
    setFlags:       SetFlags = Zero
    disabled:       bool
    checked:        bool
    hidden:         bool
}

impl IEvent for CommandStateEvent
{
}

struct TimerEvent
{
    using base:     Event{kind: EventKind.Timer}
    timer:          *Timer
}

impl IEvent for TimerEvent
{
}

struct FrameEvent
{
    using base:     Event{kind: EventKind.Frame}
    firstFrame:     bool
}

impl IEvent for FrameEvent
{
}

struct CreateEvent
{
    using base: Event{kind: EventKind.Create}
}

impl IEvent for CreateEvent
{
}

struct StateEvent
{
    using base: Event
}

impl IEvent for StateEvent
{
}

struct DestroyEvent
{
    using base: Event{kind: EventKind.Destroy}
}

impl IEvent for DestroyEvent
{
}

struct PaintEvent
{
    using base:     Event{kind: EventKind.Paint}
    bc:             *PaintContext
}

impl IEvent for PaintEvent
{
}

struct ResizeEvent
{
    using base:     Event{kind: EventKind.Resize}
    oldSize:        Math.Point
}

impl IEvent for ResizeEvent
{
}

struct MoveEvent
{
    using base:     Event{kind: EventKind.Move}
    oldPos:         Math.Point
}

impl IEvent for MoveEvent
{
}

struct KeyEvent
{
    using base:     Event
    modifiers:      Input.KeyModifiers = Zero
    key:            Input.Key = undefined
    char:           rune
}

impl IEvent for KeyEvent
{
}

struct MouseEvent
{
    using base:     Event
    button:         Input.MouseButton = undefined
    surfacePos:     Math.Point
    move:           Math.Point
    modifiers:      Input.KeyModifiers = Zero
}

impl IEvent for MouseEvent
{
}

struct QuitEvent
{
    using base:     Event{kind: EventKind.Quit}
    quitCode:       s32
}

impl IEvent for QuitEvent
{
}

struct FocusEvent
{
    using base:        Event
    other:             *Wnd
    canChangeFocus     = true
}

impl IEvent for FocusEvent
{
}

struct SysUserEvent
{
    using base:     Event{kind: EventKind.SysUser}
    userMsg:        u64
    param0:         u64
    param1:         u64
}

impl IEvent for SysUserEvent
{
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

impl IEvent for SysCommandEvent
{
}

struct SetThemeEvent
{
    using base:     Event{kind: EventKind.SetTheme}
    style:          *ThemeStyle
    parent:         *ThemeStyle
}

impl IEvent for SetThemeEvent
{
}

struct SerializeStateEvent
{
    using base:     Event{kind: EventKind.SerializeState}
    ser:            *Serialization.Serializer
}

impl IEvent for SerializeStateEvent
{
}

struct NotifyEvent
{
    using base: Event{kind: EventKind.Notify}

    enum Kind
    {
        ChildFocusChanged
    }

    ntfyKind:     Kind = undefined
    from:         *Wnd
}

impl IEvent for NotifyEvent
{
}