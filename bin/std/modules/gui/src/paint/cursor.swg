#global public

#[Swag.EnumIndex]
enum CursorShape
{
    Arrow
    SizeWE
    SizeNS
    SizeNWSE
    SizeNESW
    SizeAll
    Cross
    Help
    Hand
    IBeam
    No
}

struct Cursor
{
    using native: NativeCursor
}