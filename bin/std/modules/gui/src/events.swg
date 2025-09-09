#global public
using Core

interface IEvent
{
}

struct Event
{
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
}

impl IEvent for Event
{
}

struct CommandEvent
{
    using base:     Event
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
        Zero          = 0
        Disabled
        Icon
        Shortcut
        Checked
        Name
        LongName
        ToolTip
        Hidden
        RenderingHint
    }

    #[Swag.EnumFlags]
    enum RenderingHint
    {
        Zero           = 0
        ExclusiveGroup
    }

    enum Kind
    {
        ComputeCommandState
        ApplyCommandState
    }

    using base:        Event
    kind:              Kind
    id:                WndId
    source:            *Wnd

    icon:              Icon
    name:              String
    longName:          String
    shortcut:          String
    toolTip:           String
    setFlags:          SetFlags
    renderingHint:     RenderingHint
    disabled:          bool
    checked:           bool
    hidden:            bool
}

impl IEvent for CommandStateEvent
{
}

struct TimerEvent
{
    using base:     Event
    timer:          *Timer
}

impl IEvent for TimerEvent
{
}

struct FrameEvent
{
    using base:     Event
    firstFrame:     bool
}

impl IEvent for FrameEvent
{
}

struct CreateEvent
{
    using base: Event
}

impl IEvent for CreateEvent
{
}

struct StateEvent
{
    enum Kind
    {
        Show
        Hide
    }

    using base:     Event
    kind:           Kind
}

impl IEvent for StateEvent
{
}

struct DestroyEvent
{
    using base: Event
}

impl IEvent for DestroyEvent
{
}

struct PaintEvent
{
    enum Kind
    {
        PrePaint
        Paint
        PostPaint
    }

    using base:     Event
    kind:           Kind = .Paint
    bc:             *PaintContext
}

impl IEvent for PaintEvent
{
}

struct ResizeEvent
{
    using base:     Event
    oldSize:        Math.Point
}

impl IEvent for ResizeEvent
{
}

struct MoveEvent
{
    using base:     Event
    oldPos:         Math.Point
}

impl IEvent for MoveEvent
{
}

struct KeyEvent
{
    enum Kind
    {
        KeyPressed
        KeyReleased
        Rune
    }

    using base:     Event
    kind:           Kind
    modifiers:      Input.KeyModifiers = Zero
    key:            Input.Key = undefined
    char:           rune
}

impl IEvent for KeyEvent
{
}

struct MouseEvent
{
    enum Kind
    {
        MouseAccept
        MousePressed
        MouseReleased
        MouseDoubleClick
        MouseMove
        MouseEnter
        MouseLeave
        MouseWheel
        MouseSetCursor
    }

    using base:     Event
    kind:           Kind
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
    using base:     Event
    quitCode:       s32
}

impl IEvent for QuitEvent
{
}

struct FocusEvent
{
    enum Kind
    {
        SetFocus
        KillFocus
        CanSetFocus
        CanKillFocus
    }

    using base:        Event
    kind:              Kind
    other:             *Wnd
    canChangeFocus     = true
}

impl IEvent for FocusEvent
{
}

struct SysUserEvent
{
    using base:     Event
    userMsg:        u64
    param0:         u64
    param1:         u64
}

impl IEvent for SysUserEvent
{
}

struct SysCommandEvent
{
    enum Kind
    {
        Minimize
        Maximize
        Restore
        Close
        User
    }

    using base:     Event
    kind:           Kind
}

impl IEvent for SysCommandEvent
{
}

struct ThemeEvent
{
    using base:     Event
    style:          *ThemeStyle
    parent:         *ThemeStyle
}

impl IEvent for ThemeEvent
{
}

struct SerializeStateEvent
{
    using base:     Event
    ser:            *Serialization.Serializer
}

impl IEvent for SerializeStateEvent
{
}

struct NotifyEvent
{
    using base: Event

    enum Kind
    {
        ChildFocusChanged
        ThemeChanged
    }

    ntfyKind:     Kind = undefined
    from:         *Wnd
}

impl IEvent for NotifyEvent
{
}

struct InvalidateEvent
{
    using base: Event
}

impl IEvent for InvalidateEvent
{
}
