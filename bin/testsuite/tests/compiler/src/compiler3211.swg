#[Swag.EnumFlags]
enum KeyModifiers
{
    None
    Shift
    Control
    Alt
}

struct Event {}

enum Key
{
    R
}

struct KeyEvent
{
    using base:     Event
    modifiers:      KeyModifiers = .None
    key:            Key = undefined
}
