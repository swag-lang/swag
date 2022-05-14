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
    AlignLeft
    AlignCenter
    AlignRight
    AlignTop
    AlignMiddle
    AlignBottom
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
    oldPos:     Array's32
    minX:       s32
    maxX:       s32
}

impl Undo
{
    mtd init()
    {
        switch kind
        {
        case AlignLeft, AlignCenter, AlignRight:
            selection = capture.selection
            minX = Swag.S32.Max
            maxX = Swag.S32.Min
            oldPos.reserve(selection.count)
            visit s: selection
            {
                b := cast(*BaseForm) @dataof(capture.forms[s])
                oldPos.add(b.x)
                minX = Math.min(minX, b.x)
                maxX = Math.max(maxX, b.x + b.w)
            }

        case AlignTop, AlignMiddle, AlignBottom:
            selection = capture.selection
            minX = Swag.S32.Max
            maxX = Swag.S32.Min
            oldPos.reserve(selection.count)
            visit s: selection
            {
                b := cast(*BaseForm) @dataof(capture.forms[s])
                oldPos.add(b.x)
                minX = Math.min(minX, b.y)
                maxX = Math.max(maxX, b.y + b.h)
            }

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
        case AlignLeft:
            visit s: selection
            {
                b := cast(*BaseForm) @dataof(capture.forms[s])
                b.x = minX
            }

        case AlignRight:
            visit s: selection
            {
                b := cast(*BaseForm) @dataof(capture.forms[s])
                b.x = maxX - b.w
            }

        case AlignCenter:
            visit s: selection
            {
                b := cast(*BaseForm) @dataof(capture.forms[s])
                b.x = (maxX + minX) / 2 - (b.w / 2)
            }

        case AlignTop:
            visit s: selection
            {
                b := cast(*BaseForm) @dataof(capture.forms[s])
                b.y = minX
            }

        case AlignBottom:
            visit s: selection
            {
                b := cast(*BaseForm) @dataof(capture.forms[s])
                b.y = maxX - b.h
            }

        case AlignMiddle:
            visit s: selection
            {
                b := cast(*BaseForm) @dataof(capture.forms[s])
                b.y = (maxX + minX) / 2 - (b.h / 2)
            }

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
        case AlignLeft, AlignRight, AlignCenter, AlignTop, AlignBottom, AlignMiddle:
            visit s, i: selection
            {
                b := cast(*BaseForm) @dataof(capture.forms[s])
                b.x = oldPos[i]
            }

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