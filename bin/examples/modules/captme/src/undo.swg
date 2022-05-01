using Core

enum UndoKind
{
    MoveCapture
    ResizeCapture
    OffsetSelection
}

struct Undo
{
    kind:       UndoKind
    capture:    *Capture

    newSizeX, newSizeY: s32
    selection:  Array'uint
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
        case OffsetSelection:
            visit s: selection
            {
                f := capture.forms[s]
                f.offset(newSizeX, newSizeY)
            }
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
        case OffsetSelection:
            visit s: selection
            {
                f := capture.forms[s]
                f.offset(-newSizeX, -newSizeY)
            }
        }
    }
}