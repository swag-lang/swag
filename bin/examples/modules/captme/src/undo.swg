using Core

enum UndoKind
{
    MoveBaseForm
    MoveCapture
    OffsetSelection
    MoveToFront
    MoveToBack
    MoveForward
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
    mtd init()
    {
        switch kind
        {
        case OffsetSelection:
            selection = capture.selection
        case MoveToFront, MoveToBack, MoveBackward, MoveForward:
            selection = capture.selection
            oldForms = capture.forms
            selection.sort()
        }
    }

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
            capture.hotForm = Swag.UInt.Max
            dstIdx := cast(s32) (capture.forms.count - 1)
            for i := cast(s32) (selection.count - 1); i >= 0; i -= 1
            {
                srcIdx := selection[i]
                if srcIdx != dstIdx
                {
                    form := capture.forms[srcIdx]
                    capture.forms.removeAtOrdered(srcIdx)
                    capture.forms.insertAt(acast dstIdx, form)
                    capture.selection.remove(srcIdx)
                    capture.selection.add(acast dstIdx)
                }

                dstIdx -= 1
            }

        case MoveToBack:
            capture.hotForm = Swag.UInt.Max
            dstIdx := 0
            for i := 0; i < selection.count; i += 1
            {
                srcIdx := selection[i]
                if srcIdx != dstIdx
                {
                    form := capture.forms[srcIdx]
                    capture.forms.removeAtOrdered(srcIdx)
                    capture.forms.insertAt(acast dstIdx, form)
                    capture.selection.remove(srcIdx)
                    capture.selection.add(acast dstIdx)
                }

                dstIdx += 1
            }

        case MoveForward:
            capture.hotForm = Swag.UInt.Max
            for i := cast(s32) (selection.count - 1); i >= 0; i -= 1
            {
                srcIdx := selection[i]
                if srcIdx != capture.forms.count - 1
                {
                    form := capture.forms[srcIdx]
                    capture.forms.removeAtOrdered(srcIdx)
                    capture.forms.insertAt(srcIdx + 1, form)
                    capture.selection.remove(srcIdx)
                    capture.selection.add(srcIdx + 1)
                }
            }

        case MoveBackward:
            capture.hotForm = Swag.UInt.Max
            for i := 0; i < selection.count; i += 1
            {
                srcIdx := selection[i]
                if srcIdx != 0
                {
                    form := capture.forms[srcIdx]
                    capture.forms.removeAtOrdered(srcIdx)
                    capture.forms.insertAt(srcIdx - 1, form)
                    capture.selection.remove(srcIdx)
                    capture.selection.add(srcIdx - 1)
                }
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

        case MoveToFront, MoveToBack, MoveForward, MoveBackward:
            capture.selection = selection
            capture.forms     = oldForms
        }
    }
}