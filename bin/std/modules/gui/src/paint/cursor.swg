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
    Wait
}

struct Cursor
{
    using native: NativeCursor
}

impl Cursor
{
    // Force the wait cursor
    func wait()
    {
        apply(Cursor.from(CursorShape.Wait))
    }

    // Returns true if the cursor is defined
    mtd isValid()->bool
    {
        return .native.cursor != null
    }

    // Destroy the cursor
    mtd destroy()
    {
        .native.cursor = null
    }
}
