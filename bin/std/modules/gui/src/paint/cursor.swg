#global public

#[Swag.EnumIndex]
enum CursorShape
{
    Arrow
    SizeWE
    SizeNS
    SizeNWSE
    SizeNESW
}

struct Cursor
{
    using native: NativeCursor
}