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
}

struct Cursor
{
    using native: NativeCursor
}