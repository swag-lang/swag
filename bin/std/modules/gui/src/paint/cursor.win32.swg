#global if #os == Swag.TargetOs.Windows
using Win32

public struct NativeCursor
{
    cursor: HCURSOR
}

impl Cursor
{
    public func from(shape: CursorShape)->Cursor
    {
        var result: retval

        #[Swag.Complete]
        switch shape
        {
        case Arrow:
            result.cursor = LoadCursorA(null, MAKEINTRESOURCEA(IDC_ARROW))
        case SizeWE:
            result.cursor = LoadCursorA(null, MAKEINTRESOURCEA(IDC_SIZEWE))
        }

        return result
    }
}