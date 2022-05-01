using Core

enum UndoKind
{
    ResizeCapture
}

struct Undo
{
    kind:       UndoKind
    capture:    *Capture
    newSize:    Math.Point
    oldSize:    Math.Point
}

impl Undo
{
    mtd do()
    {
        switch kind
        {
        case ResizeCapture:
            oldSize.x = cast(s32) capture.width
            oldSize.y = cast(s32) capture.height
            capture.width = cast(s32) newSize.x
            capture.height = cast(s32) newSize.y
        }
    }

    mtd undo()
    {
        switch kind
        {
        case ResizeCapture:
            capture.width = cast(s32) oldSize.x
            capture.height = cast(s32) oldSize.y
        }
    }
}