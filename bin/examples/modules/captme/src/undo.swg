using Core

enum UndoKind
{
    MoveBaseForm
    MoveCapture
    OffsetSelection
    MoveToFront
    MoveToBack
    MoveFrontward
    MoveBackward
}

struct Undo
{
    kind:       UndoKind
    capture:    *Capture
    baseForm:   *BaseForm

    newSizeX:   s32
    newSizeY:   s32
    newPosX:    s32
    newPosY:    s32

    selection:  Array'uint
    oldForms:   Array'IForm
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
            if newPosX or newPosY
            {
                visit f: capture.forms
                    f.offset(newPosX, newPosY)
            }
            capture.width  += newSizeX
            capture.height += newSizeY

        case OffsetSelection:
            visit s: selection
            {
                f := capture.forms[s]
                f.offset(newSizeX, newSizeY)
            }

        case MoveToFront:
            selection.sort()
            oldForms = capture.forms
            dstIdx := cast(s32) (capture.forms.count - 1)
            for i := cast(s32) (selection.count - 1); i >= 0; i -= 1
            {
                srcIdx := selection[i]
                if srcIdx != dstIdx
                {
                    form := capture.forms[srcIdx]
                    capture.forms.removeAtOrdered(acast srcIdx)
                    capture.forms.insertAt(acast dstIdx, form)
                    capture.selection.remove(srcIdx)
                    capture.selection.add(cast(uint) dstIdx)
                }

                dstIdx -= 1
            }

        case MoveToBack:
            break
        case MoveFrontward:
            break
        case MoveBackward:
            break
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
            if newPosX or newPosY
            {
                visit f: capture.forms
                    f.offset(-newPosX, -newPosY)
            }
            capture.width  -= newSizeX
            capture.height -= newSizeY

        case OffsetSelection:
            visit s: selection
            {
                f := capture.forms[s]
                f.offset(-newSizeX, -newSizeY)
            }

        case MoveToFront, MoveToBack, MoveBackward, MoveFrontward:
            capture.selection = selection
            capture.forms     = oldForms
        }
    }
}