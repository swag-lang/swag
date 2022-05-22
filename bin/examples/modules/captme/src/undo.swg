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
    MovePoint
    CreateForm
    DeleteSelection
    SetAlpha
}

struct Undo
{
    kind:       UndoKind
    capture:    *Capture
    baseForm:   IForm

    newSizeX:   s32
    newSizeY:   s32
    newPosX:    s32
    newPosY:    s32

    selection:  Array'uint
    oldForms:   Array'IForm
    oldS32:     Array's32
    valS32:     s32
    minP:       s32
    maxP:       s32
    pointIdx:   u32
}

impl Undo
{
    mtd init()
    {
        switch kind
        {
        case AlignLeft, AlignCenter, AlignRight:
            selection = capture.selection
            minP = Swag.S32.Max
            maxP = Swag.S32.Min
            oldS32.reserve(selection.count)
            visit s: selection
            {
                f := capture.forms[s]
                r := f.boundRect()
                oldS32.add(cast(s32) r.x)
                minP = Math.min(minP, cast(s32) r.x)
                maxP = Math.max(maxP, cast(s32) (r.x + r.width))
            }

        case AlignTop, AlignMiddle, AlignBottom:
            selection = capture.selection
            minP = Swag.S32.Max
            maxP = Swag.S32.Min
            oldS32.reserve(selection.count)
            visit s: selection
            {
                f := capture.forms[s]
                r := f.boundRect()
                oldS32.add(cast(s32) r.x)
                minP = Math.min(minP, cast(s32) r.y)
                maxP = Math.max(maxP, cast(s32) (r.y + r.height))
            }

        case OffsetSelection:
            selection = capture.selection

        case SetAlpha:
            selection = capture.selection
            visit s: selection
            {
                b := cast(*BaseForm) @dataof(capture.forms[s])
                oldS32.add(cast(s32) b.alpha)
            }

        case MoveToFront, MoveToBack, MoveBackward, MoveForward:
            selection = capture.selection
            oldForms = capture.forms
            selection.sort()

        case DeleteSelection:
            selection = capture.selection
            oldForms = capture.forms
        }
    }

    mtd do()
    {
        #[Swag.Complete]
        switch kind
        {
        case SetAlpha:
            visit s: selection
            {
                b := cast(*BaseForm) @dataof(capture.forms[s])
                b.alpha = cast(u8) valS32
            }

        case AlignLeft:
            visit s: selection
            {
                f := capture.forms[s]
                r := f.boundRect()
                f.moveBaseForm(minP - cast(s32) r.x, 0, 0, 0)
            }

        case AlignRight:
            visit s: selection
            {
                f := capture.forms[s]
                r := f.boundRect()
                f.moveBaseForm((maxP - cast(s32) r.width) - cast(s32) r.x, 0, 0, 0)
            }

        case AlignCenter:
            visit s: selection
            {
                f := capture.forms[s]
                r := f.boundRect()
                f.moveBaseForm(((maxP + minP) / 2 - (cast(s32) r.width / 2)) - cast(s32) r.x, 0, 0, 0)
            }

        case AlignTop:
            visit s: selection
            {
                f := capture.forms[s]
                r := f.boundRect()
                f.moveBaseForm(0, minP - cast(s32) r.y, 0, 0)
            }

        case AlignBottom:
            visit s: selection
            {
                f := capture.forms[s]
                r := f.boundRect()
                f.moveBaseForm(0, (maxP - cast(s32) r.height) - cast(s32) r.y, 0, 0)
            }

        case AlignMiddle:
            visit s: selection
            {
                f := capture.forms[s]
                r := f.boundRect()
                f.moveBaseForm(0, ((maxP + minP) / 2 - (cast(s32) r.height / 2)) - cast(s32) r.y, 0, 0)
            }

        case MovePoint:
            baseForm.movePoint(pointIdx, newPosX, newPosY)

        case MoveBaseForm:
            baseForm.moveBaseForm(newPosX, newPosY, newSizeX, newSizeY)

        case MoveCapture:
            visit f: capture.forms
                f.moveForm(newPosX, newPosY)
            capture.width  += newSizeX
            capture.height += newSizeY

        case OffsetSelection:
            visit s: selection
            {
                f := capture.forms[s]
                f.moveForm(newPosX, newPosY)
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

        case DeleteSelection:
            visit s: selection
            {
                f := capture.forms[s]
                capture.forms.remove(f)
            }

            capture.selection.clear()
            capture.selectionHasChanged()

        case CreateForm:
            capture.forms.add(oldForms[0])
        }
    }

    mtd undo()
    {
        #[Swag.Complete]
        switch kind
        {
        case SetAlpha:
            visit s, i: selection
            {
                b := cast(*BaseForm) @dataof(capture.forms[s])
                b.alpha = cast(u8) oldS32[i]
            }

        case AlignLeft, AlignRight, AlignCenter, AlignTop, AlignBottom, AlignMiddle:
            visit s, i: selection
            {
                b := cast(*BaseForm) @dataof(capture.forms[s])
                b.x = oldS32[i]
            }

        case MovePoint:
            baseForm.movePoint(pointIdx, -newPosX, -newPosY)

        case MoveBaseForm:
            b := cast(*BaseForm) @dataof(baseForm)
            b.x -= newPosX
            b.y -= newPosY
            b.w -= newSizeX
            b.h -= newSizeY

        case MoveCapture:
            visit f: capture.forms
                f.moveForm(-newPosX, -newPosY)
            capture.width  -= newSizeX
            capture.height -= newSizeY

        case OffsetSelection:
            visit s: selection
            {
                f := capture.forms[s]
                f.moveForm(-newPosX, -newPosY)
            }

        case MoveToFront, MoveToBack, MoveForward, MoveBackward, DeleteSelection:
            capture.selection = selection
            capture.forms     = oldForms
            capture.selectionHasChanged()

        case CreateForm:
            capture.selection.remove(capture.forms.count - 1)
            capture.forms.removeBack()
            capture.selectionHasChanged()
        }
    }
}