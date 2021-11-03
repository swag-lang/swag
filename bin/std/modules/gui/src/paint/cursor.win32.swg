#global if #os == Swag.TargetOs.Windows
using Win32

public struct NativeCursor
{
    cursor: HCURSOR
}

private var DefaultCursors: [@countof(CursorShape)] Cursor
#init
{
    using CursorShape
    DefaultCursors[Arrow].cursor    = LoadCursorA(null, MAKEINTRESOURCEA(IDC_ARROW))
    DefaultCursors[SizeWE].cursor   = LoadCursorA(null, MAKEINTRESOURCEA(IDC_SIZEWE))
    DefaultCursors[SizeNS].cursor   = LoadCursorA(null, MAKEINTRESOURCEA(IDC_SIZENS))
    DefaultCursors[SizeNWSE].cursor = LoadCursorA(null, MAKEINTRESOURCEA(IDC_SIZENWSE))
    DefaultCursors[SizeNESW].cursor = LoadCursorA(null, MAKEINTRESOURCEA(IDC_SIZENESW))
    DefaultCursors[SizeAll].cursor  = LoadCursorA(null, MAKEINTRESOURCEA(IDC_SIZEALL))
    DefaultCursors[Cross].cursor    = LoadCursorA(null, MAKEINTRESOURCEA(IDC_CROSS))
    DefaultCursors[Help].cursor     = LoadCursorA(null, MAKEINTRESOURCEA(IDC_HELP))
}

impl Cursor
{
    public func from(shape: CursorShape)->Cursor
    {
        return DefaultCursors[shape]
    }
}