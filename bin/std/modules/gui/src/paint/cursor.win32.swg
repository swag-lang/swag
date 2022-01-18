#global if #os == Swag.TargetOs.Windows
using Win32

public struct NativeCursor
{
    cursor: HCURSOR
}

impl NativeCursor
{
    var defaultCursors: [@countof(CursorShape)] Cursor
}

#init
{
    using NativeCursor, CursorShape
    defaultCursors[Arrow].cursor    = LoadCursorA(null, MAKEINTRESOURCEA(IDC_ARROW))
    defaultCursors[SizeWE].cursor   = LoadCursorA(null, MAKEINTRESOURCEA(IDC_SIZEWE))
    defaultCursors[SizeNS].cursor   = LoadCursorA(null, MAKEINTRESOURCEA(IDC_SIZENS))
    defaultCursors[SizeNWSE].cursor = LoadCursorA(null, MAKEINTRESOURCEA(IDC_SIZENWSE))
    defaultCursors[SizeNESW].cursor = LoadCursorA(null, MAKEINTRESOURCEA(IDC_SIZENESW))
    defaultCursors[SizeAll].cursor  = LoadCursorA(null, MAKEINTRESOURCEA(IDC_SIZEALL))
    defaultCursors[Cross].cursor    = LoadCursorA(null, MAKEINTRESOURCEA(IDC_CROSS))
    defaultCursors[Help].cursor     = LoadCursorA(null, MAKEINTRESOURCEA(IDC_HELP))
}

impl Cursor
{
    public func from(shape: CursorShape)->Cursor
    {
        return NativeCursor.defaultCursors[shape]
    }


    public mtd clear()
    {
        cursor = null
    }
}