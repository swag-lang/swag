using Core

enum UndoKind
{
    MoveCapture
    ResizeCapture
}

struct Undo
{
    kind:       UndoKind
    capture:    *Capture

    newSizeX, newSizeY: s32
}

impl Undo
{
    mtd do()
    {
        #[Swag.Complete]
        switch kind
        {
        case MoveCapture:
            visit f: capture.forms
                f.offset(newSizeX, newSizeY)
            fallthrough
        case ResizeCapture:
            capture.width  += newSizeX
            capture.height += newSizeY
        }
    }

    mtd undo()
    {
        #[Swag.Complete]
        switch kind
        {
        case MoveCapture:
            visit f: capture.forms
                f.offset(-newSizeX, -newSizeY)
            fallthrough
        case ResizeCapture:
            capture.width  -= newSizeX
            capture.height -= newSizeY
        }
    }
}