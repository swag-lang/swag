#global public

enum EventKind
{
    None
    Resize
    Paint
}

struct Event
{
    kind: EventKind = None
}

struct PaintEvent
{
    using base: Event
}

struct ResizeEvent
{
    using base: Event
}