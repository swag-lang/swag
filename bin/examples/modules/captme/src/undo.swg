using Core

enum UndoKind
{
    MoveBaseForm
    MoveCapture
    OffsetSelection
}

struct Undo
{
    kind:       UndoKind
    capture:    *Capture
    baseForm:   *BaseForm

    newSizeX, newSizeY: s32
    newPosX,  newPosY: s32
    selection:  Array'uint
}

impl Undo
{
    mtd do()
    {
        #[Swag.Complete]
        switch kind
        {
        case MoveBaseForm:
            baseForm.x += newPosX
            baseForm.y += newPosY
            baseForm.w += newSizeX
            baseForm.h += newSizeY
        case MoveCapture:
            visit f: capture.forms
                f.offset(newPosX, newPosY)
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
        case MoveBaseForm:
            baseForm.x -= newPosX
            baseForm.y -= newPosY
            baseForm.w -= newSizeX
            baseForm.h -= newSizeY
        case MoveCapture:
            visit f: capture.forms
                f.offset(-newPosX, -newPosY)
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