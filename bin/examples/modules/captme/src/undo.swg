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
    SetTypedValue
    SetFormFromModel
    SetText
    SetFont
    SetFontStyle
    Flatten
    FlattenAll
    RestoreOrg
    SetImage
    SetImageInside
}

struct Undo
{
    kind:       UndoKind
    capture:    *Capture
    baseForm:   IForm
    serial:     u32

    selection:      Array'u64

    toFreeForms:    Array'IForm
    oldForms:       Array'IForm
    oldS32:         Array's32
    oldU32:         Array'u32
    oldForm:        Array'*BaseForm
    oldText:        Array'String
    oldImage:       Array'Image
    oldImg:         Image
    oldImagePos:    Math.Point

    newSizeX:       s32
    newSizeY:       s32
    newPosX:        s32
    newPosY:        s32
    newImg:         Image
    newImagePos:    Math.Point

    minP:       s32
    maxP:       s32
    pointIdx:   u32
    valU32:     u32
    valRef:     const *Swag.TypeValue
    mdlType:    typeinfo
    mdlValue:   *BaseForm
    text:       String
    fontStyle:  FontFamilyStyle
}

impl Undo
{
    mtd opDrop()
    {
        Capture.freeForms(toFreeForms)

        if kind == .SetFormFromModel
        {
            Memory.delete(mdlValue, mdlType)
            visit f: oldForm
                Memory.delete(f, mdlType)
        }
    }

    mtd init()
    {
        switch kind
        {
        case RestoreOrg:
            oldImagePos = capture.backImagePos
            oldImg = capture.backImg
            oldForms = capture.forms
            oldS32.add(capture.width)
            oldS32.add(capture.height)

        case Flatten:
            selection = capture.selection
            oldImg = capture.backImg
            oldImagePos = capture.backImagePos
            oldForms = capture.forms

        case FlattenAll:
            oldImg = capture.backImg
            oldImagePos = capture.backImagePos
            oldForms = capture.forms

        case SetImage:
            oldImg = capture.backImg
            oldImagePos = capture.backImagePos

        case SetImageInside:
            selection = capture.selection
            visit s: selection
            {
                f := capture.forms[s]
                if @kindof(f) != FormImage
                    continue
                b := cast(*FormImage) @dataof(f)
                if b.kind != .Image
                    continue
                oldImage.add(b.image)
            }

        case SetText:
            selection = capture.selection
            visit s: selection
            {
                f := capture.forms[s]
                if @kindof(f) != FormText
                    continue
                b := cast(*FormText) @dataof(f)
                oldText.add(b.text)
            }

        case SetFont:
            selection = capture.selection
            visit s: selection
            {
                f := capture.forms[s]
                if @kindof(f) != FormText
                    continue
                b := cast(*FormText) @dataof(f)
                oldText.add(b.fontName)
                oldU32.add(cast(u32) b.fontStyle)
            }

        case SetFontStyle:
            selection = capture.selection
            visit s: selection
            {
                f := capture.forms[s]
                if @kindof(f) != FormText
                    continue
                b := cast(*FormText) @dataof(f)
                oldU32.add(cast(u32) b.fontStyle)
            }

        case SetFormFromModel:
            selection = capture.selection
            visit s: selection
            {
                f := capture.forms[s]
                if @kindof(f) != mdlType
                    continue
                save := cast(*BaseForm) Memory.new(mdlType)
                f.copy(save, cast(*BaseForm) @dataof(f))
                oldForm.add(save)
            }

        case AlignLeft, AlignCenter, AlignRight:
            selection = capture.selection
            minP = Swag.S32.Max
            maxP = Swag.S32.Min
            oldS32.reserve(selection.count)
            visit s: selection
            {
                f := capture.forms[s]
                r := f.boundRect(false)
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
                r := f.boundRect(false)
                oldS32.add(cast(s32) r.x)
                minP = Math.min(minP, cast(s32) r.y)
                maxP = Math.max(maxP, cast(s32) (r.y + r.height))
            }

        case OffsetSelection:
            selection = capture.selection

        case SetTypedValue:
            selection = capture.selection
            visit s: selection
            {
                f := capture.forms[s]
                if mdlType and @kindof(f) != mdlType
                    continue
                var val: u32
                Reflection.getFieldValue(&val, valRef, @dataof(f), true)
                oldU32.add(val)
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
        mtd removeSelection()
        {
            toFreeForms.clear()
            selection.sort()
            offset := 0
            visit s: selection
            {
                f := capture.forms[s - offset]
                offset += 1
                capture.forms.removeOrdered(f)
                toFreeForms.add(f)
            }

            capture.selection.clear()
            capture.selectionHasChanged()
        }

        #[Swag.Complete]
        switch kind
        {
        case RestoreOrg:
            with capture
            {
                .backImg = .backImgOrg
                .width = .widthOrg
                .height = .heightOrg
                .selection.clear()
                .forms.clear()
                .dirtyBackImg = true
                .backImagePos.clear()
                .selectionHasChanged()
            }

        case Flatten:
            with capture
            {
                .backImg = .toImage(onlySel: true)
                .backImagePos.clear()
                .dirtyBackImg = true
            }
            removeSelection()

        case FlattenAll:
            with capture
            {
                .backImg = capture.toImage()
                .backImagePos.clear()
                .selection.clear()
                .forms.clear()
                .dirtyBackImg = true
                .selectionHasChanged()
            }

        case SetImage:
            capture.backImg = newImg
            capture.backImagePos = newImagePos
            capture.dirtyBackImg = true

        case SetImageInside:
            visit s, i: selection
            {
                f := capture.forms[s]
                if @kindof(f) != FormImage
                    continue
                b := cast(*FormImage) @dataof(f)
                if b.kind != .Image
                    continue
                b.image = oldImg
                b.dirty = true
            }

        case SetText:
            visit s, i: selection
            {
                f := capture.forms[s]
                if @kindof(f) != FormText
                    continue
                b := cast(*FormText) @dataof(f)
                b.text = text
                b.isDirty = true
            }

        case SetFont:
            visit s, i: selection
            {
                f := capture.forms[s]
                if @kindof(f) != FormText
                    continue
                b := cast(*FormText) @dataof(f)
                b.fontName  = text
                b.fontStyle = fontStyle
                b.isDirtyFont = true
            }

        case SetFontStyle:
            visit s, i: selection
            {
                f := capture.forms[s]
                if @kindof(f) != FormText
                    continue
                b := cast(*FormText) @dataof(f)
                b.fontStyle = fontStyle
                b.isDirtyFont = true
            }

        case SetFormFromModel:
            visit s: selection
            {
                f := capture.forms[s]
                if @kindof(f) != mdlType
                    continue
                bf := cast(*BaseForm) @dataof(f)
                if bf.subKind != mdlValue.subKind
                    continue
                f.copy(cast(*BaseForm) @dataof(f), mdlValue)
            }

        case SetTypedValue:
            visit s: selection
            {
                f := capture.forms[s]
                if mdlType and @kindof(f) != mdlType
                    continue
                Reflection.setFieldValue(@dataof(f), valRef, &valU32, true)
            }

            capture.updateModelFromSelection()

        case AlignLeft:
            visit s: selection
            {
                f := capture.forms[s]
                r := f.boundRect(false)
                f.moveBaseForm(minP - cast(s32) r.x, 0, 0, 0)
            }

        case AlignRight:
            visit s: selection
            {
                f := capture.forms[s]
                r := f.boundRect(false)
                f.moveBaseForm((maxP - cast(s32) r.width) - cast(s32) r.x, 0, 0, 0)
            }

        case AlignCenter:
            visit s: selection
            {
                f := capture.forms[s]
                r := f.boundRect(false)
                f.moveBaseForm(((maxP + minP) / 2 - (cast(s32) r.width / 2)) - cast(s32) r.x, 0, 0, 0)
            }

        case AlignTop:
            visit s: selection
            {
                f := capture.forms[s]
                r := f.boundRect(false)
                f.moveBaseForm(0, minP - cast(s32) r.y, 0, 0)
            }

        case AlignBottom:
            visit s: selection
            {
                f := capture.forms[s]
                r := f.boundRect(false)
                f.moveBaseForm(0, (maxP - cast(s32) r.height) - cast(s32) r.y, 0, 0)
            }

        case AlignMiddle:
            visit s: selection
            {
                f := capture.forms[s]
                r := f.boundRect(false)
                f.moveBaseForm(0, ((maxP + minP) / 2 - (cast(s32) r.height / 2)) - cast(s32) r.y, 0, 0)
            }

        case MovePoint:
            baseForm.movePoint(pointIdx, newPosX, newPosY)

        case MoveBaseForm:
            baseForm.moveBaseForm(newPosX, newPosY, newSizeX, newSizeY)

        case MoveCapture:
            capture.backImagePos.x += newPosX
            capture.backImagePos.y += newPosY
            visit f: capture.forms
                f.moveForm(newPosX, newPosY)
            capture.width  += newSizeX
            capture.height += newSizeY
            capture.width = Math.clamp(capture.width, Capture.MinSize, Capture.MaxSize)
            capture.height = Math.clamp(capture.height, Capture.MinSize, Capture.MaxSize)

        case OffsetSelection:
            visit s: selection
            {
                f := capture.forms[s]
                f.moveForm(newPosX, newPosY)
            }

        case MoveToFront:
            capture.hotForm = Swag.U64.Max
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
            capture.hotForm = Swag.U64.Max
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
            capture.hotForm = Swag.U64.Max
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
            capture.hotForm = Swag.U64.Max
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
            removeSelection()

        case CreateForm:
            toFreeForms.clear()
            visit f: oldForms
                capture.forms.add(f)
        }

        capture.hasChanged()
    }

    mtd undo()
    {
        #[Swag.Complete]
        switch kind
        {
        case RestoreOrg:
            with capture
            {
                .width = oldS32[0]
                .height = oldS32[1]
                .backImg = oldImg
                .forms   = oldForms
                .selection.clear()
                .dirtyBackImg = true
                .backImagePos = oldImagePos
                .selectionHasChanged()
            }

        case Flatten, FlattenAll:
            with capture
            {
                .backImg = oldImg
                .backImagePos = oldImagePos
                .forms = oldForms
                .selection.clear()
                .dirtyBackImg = true
                .selectionHasChanged()
            }

        case SetImage:
            capture.backImg = oldImg
            capture.backImagePos = oldImagePos
            capture.dirtyBackImg = true

        case SetImageInside:
            visit s, i: selection
            {
                f := capture.forms[s]
                if @kindof(f) != FormImage
                    continue
                b := cast(*FormImage) @dataof(f)
                if b.kind != .Image
                    continue
                b.image = oldImage[i]
                b.dirty = true
            }

        case SetText:
            visit s, i: selection
            {
                f := capture.forms[s]
                if @kindof(f) != FormText
                    continue
                b := cast(*FormText) @dataof(f)
                b.text = oldText[i]
                b.isDirty = true
            }

        case SetFont:
            visit s, i: selection
            {
                f := capture.forms[s]
                if @kindof(f) != FormText
                    continue
                b := cast(*FormText) @dataof(f)
                b.fontName = oldText[i]
                b.fontStyle = acast oldU32[i]
                b.isDirtyFont = true
            }

        case SetFontStyle:
            visit s, i: selection
            {
                f := capture.forms[s]
                if @kindof(f) != FormText
                    continue
                b := cast(*FormText) @dataof(f)
                b.fontStyle = acast oldU32[i]
                b.isDirtyFont = true
            }

        case SetFormFromModel:
            visit s, i: selection
            {
                f := capture.forms[s]
                if @kindof(f) != mdlType
                    continue
                bf := cast(*BaseForm) @dataof(f)
                if bf.subKind != mdlValue.subKind
                    continue
                f.copy(cast(*BaseForm) @dataof(f), oldForm[i])
            }

        case SetTypedValue:
            visit s, i: selection
            {
                f := capture.forms[s]
                if mdlType and @kindof(f) != mdlType
                    continue
                val := oldU32[i]
                Reflection.setFieldValue(@dataof(f), valRef, &val, true)
            }
            capture.updateModelFromSelection()

        case AlignLeft, AlignRight, AlignCenter, AlignTop, AlignBottom, AlignMiddle:
            visit s, i: selection
            {
                b := cast(*BaseForm) @dataof(capture.forms[s])
                b.x = oldS32[i]
            }

        case MovePoint:
            baseForm.movePoint(pointIdx, -newPosX, -newPosY)

        case MoveBaseForm:
            baseForm.moveBaseForm(-newPosX, -newPosY, -newSizeX, -newSizeY)

        case MoveCapture:
            capture.backImagePos.x += -newPosX
            capture.backImagePos.y += -newPosY
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
            toFreeForms.clear()
            capture.selection = selection
            capture.forms     = oldForms
            capture.selectionHasChanged()

        case CreateForm:
            toFreeForms = oldForms
            loop oldForms
            {
                capture.selection.remove(capture.forms.count - 1)
                capture.forms.removeBack()
            }

            capture.selectionHasChanged()
        }

        capture.hasChanged()
    }
}