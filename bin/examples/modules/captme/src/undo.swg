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
    kind:            UndoKind
    capture:         *Capture
    baseForm:        IForm
    serial:          u32

    selection:       Array'u64

    toFreeForms:     Array'IForm
    oldForms:        Array'IForm
    oldS32:          Array's32
    oldU32:          Array'u32
    oldForm:         Array'*BaseForm
    oldText:         Array'String
    oldImage:        Array'Image
    oldImg:          Image
    oldImagePos:     Math.Point

    newSizeX:        s32
    newSizeY:        s32
    newPosX:         s32
    newPosY:         s32
    newImg:          Image
    newImagePos:     Math.Point

    minP:            s32
    maxP:            s32
    pointIdx:        u32
    valU32:          u32
    valRef:          const *Swag.TypeValue
    mdlType:         typeinfo
    mdlValue:        *BaseForm
    text:            String
    fontStyle:       FontFamilyStyle
}

impl Undo
{
    mtd opDrop()
    {
        Capture.freeForms(.toFreeForms)

        if .kind == .SetFormFromModel
        {
            Memory.delete(.mdlValue, .mdlType)
            foreach f in .oldForm do
                Memory.delete(f, .mdlType)
        }
    }

    mtd acceptsType(type: typeinfo)->bool
    {
        if .mdlType == null do
            return true
        if .mdlType == type do
            return true
        if .mdlType == FormLine and type == FormArrow do
            return true
        return false
    }

    mtd init()
    {
        switch .kind
        {
        case RestoreOrg:
            .oldImagePos = .capture.backImagePos
            .oldImg      = .capture.backImg
            .oldForms    = .capture.forms
            .oldS32.add(.capture.width)
            .oldS32.add(.capture.height)

        case Flatten:
            .selection   = .capture.selection
            .oldImg      = .capture.backImg
            .oldImagePos = .capture.backImagePos
            .oldForms    = .capture.forms

        case FlattenAll:
            .oldImg      = .capture.backImg
            .oldImagePos = .capture.backImagePos
            .oldForms    = .capture.forms

        case SetImage:
            .oldImg      = .capture.backImg
            .oldImagePos = .capture.backImagePos

        case SetImageInside:
            .selection = .capture.selection
            foreach s in .selection
            {
                let f = .capture.forms[s]
                if @kindof(f) != FormImage do
                    continue
                let b = cast(*FormImage) @dataof(f)
                if b.kind != .Image do
                    continue
                .oldImage.add(b.image)
            }

        case SetText:
            .selection = .capture.selection
            foreach s in .selection
            {
                let f = .capture.forms[s]
                if @kindof(f) != FormText do
                    continue
                let b = cast(*FormText) @dataof(f)
                .oldText.add(b.text)
            }

        case SetFont:
            .selection = .capture.selection
            foreach s in .selection
            {
                let f = .capture.forms[s]
                if @kindof(f) != FormText do
                    continue
                let b = cast(*FormText) @dataof(f)
                .oldText.add(b.fontName)
                .oldU32.add(cast(u32) b.fontStyle)
            }

        case SetFontStyle:
            .selection = .capture.selection
            foreach s in .selection
            {
                let f = .capture.forms[s]
                if @kindof(f) != FormText do
                    continue
                let b = cast(*FormText) @dataof(f)
                .oldU32.add(cast(u32) b.fontStyle)
            }

        case SetFormFromModel:
            .selection = .capture.selection
            foreach s in .selection
            {
                let f = .capture.forms[s]
                if !.acceptsType(@kindof(f)) do
                    continue
                let save = cast(*BaseForm) Memory.new(.mdlType)
                f.copy(save, cast(*BaseForm) @dataof(f))
                .oldForm.add(save)
            }

        case AlignLeft, AlignCenter, AlignRight:
            .selection = .capture.selection
            .minP      = Swag.S32.Max
            .maxP      = Swag.S32.Min
            .oldS32.reserve(.selection.count)
            foreach s in .selection
            {
                let f = .capture.forms[s]
                let r = f.boundRect()
                .oldS32.add(cast(s32) r.x)
                .minP = Math.min(.minP, cast(s32) r.x)
                .maxP = Math.max(.maxP, cast(s32) (r.x + r.width))
            }

        case AlignTop, AlignMiddle, AlignBottom:
            .selection = .capture.selection
            .minP      = Swag.S32.Max
            .maxP      = Swag.S32.Min
            .oldS32.reserve(.selection.count)
            foreach s in .selection
            {
                let f = .capture.forms[s]
                let r = f.boundRect()
                .oldS32.add(cast(s32) r.x)
                .minP = Math.min(.minP, cast(s32) r.y)
                .maxP = Math.max(.maxP, cast(s32) (r.y + r.height))
            }

        case OffsetSelection:
            .selection = .capture.selection

        case SetTypedValue:
            .selection = .capture.selection
            foreach s in .selection
            {
                let f = .capture.forms[s]
                if !.acceptsType(@kindof(f)) do
                    continue
                var val: u32
                Reflection.getFieldValue(&val, .valRef, @dataof(f), true)
                .oldU32.add(val)
            }

        case MoveToFront, MoveToBack, MoveBackward, MoveForward:
            .selection = .capture.selection
            .oldForms  = .capture.forms
            .selection.sort()

        case DeleteSelection:
            .selection = .capture.selection
            .oldForms  = .capture.forms
        }
    }

    mtd doIt()
    {
        mtd removeSelection()
        {
            .toFreeForms.clear()
            .selection.sort()
            var offset = 0
            foreach s in .selection
            {
                let f = .capture.forms[s - offset]
                offset += 1
                .capture.forms.removeOrdered(f)
                .toFreeForms.add(f)
            }

            .capture.selection.clear()
            .capture.selectionHasChanged()
        }

        #[Swag.Complete]
        switch .kind
        {
        case RestoreOrg:
            with me.capture
            {
                .backImg = .backImgOriginal
                .width   = .widthOrg
                .height  = .heightOrg
                .selection.clear()
                .forms.clear()
                .dirtyBackImg = true
                .backImagePos.clear()
                .selectionHasChanged()
            }

        case Flatten:
            with me.capture
            {
                .backImg = .toImage(onlySel: true)
                .backImagePos.clear()
                .dirtyBackImg = true
            }
            .removeSelection()

        case FlattenAll:
            with me.capture
            {
                .backImg = me.capture.toImage()
                .backImagePos.clear()
                .selection.clear()
                .forms.clear()
                .dirtyBackImg = true
                .selectionHasChanged()
            }

        case SetImage:
            .capture.backImg      = .newImg
            .capture.backImagePos = .newImagePos
            .capture.dirtyBackImg = true

        case SetImageInside:
            foreach s, i in .selection
            {
                let f = .capture.forms[s]
                if @kindof(f) != FormImage do
                    continue
                let b = cast(*FormImage) @dataof(f)
                if b.kind != .Image do
                    continue
                b.image = .oldImg
                b.dirty = true
            }

        case SetText:
            foreach s, i in .selection
            {
                let f = .capture.forms[s]
                if @kindof(f) != FormText do
                    continue
                let b = cast(*FormText) @dataof(f)
                b.text    = .text
                b.isDirty = true
            }

        case SetFont:
            foreach s, i in .selection
            {
                let f = .capture.forms[s]
                if @kindof(f) != FormText do
                    continue
                let b = cast(*FormText) @dataof(f)
                b.fontName    = .text
                b.fontStyle   = .fontStyle
                b.isDirtyFont = true
            }

        case SetFontStyle:
            foreach s, i in .selection
            {
                let f = .capture.forms[s]
                if @kindof(f) != FormText do
                    continue
                let b = cast(*FormText) @dataof(f)
                b.fontStyle   = .fontStyle
                b.isDirtyFont = true
            }

        case SetFormFromModel:
            foreach s in .selection
            {
                let f = .capture.forms[s]
                if !.acceptsType(@kindof(f)) do
                    continue
                let bf = cast(*BaseForm) @dataof(f)
                if bf.subKind != .mdlValue.subKind do
                    continue
                f.copy(cast(*BaseForm) @dataof(f), .mdlValue)
            }

        case SetTypedValue:
            foreach s in .selection
            {
                let f = .capture.forms[s]
                if !.acceptsType(@kindof(f)) do
                    continue
                Reflection.setFieldValue(@dataof(f), .valRef, &.valU32, true)
            }

            .capture.updateModelFromSelection()

        case AlignLeft:
            foreach s in .selection
            {
                let f = .capture.forms[s]
                let r = f.boundRect()
                f.moveBaseForm(.minP - cast(s32) r.x, 0, 0, 0)
            }

        case AlignRight:
            foreach s in .selection
            {
                let f = .capture.forms[s]
                let r = f.boundRect()
                f.moveBaseForm((.maxP - cast(s32) r.width) - cast(s32) r.x, 0, 0, 0)
            }

        case AlignCenter:
            foreach s in .selection
            {
                let f = .capture.forms[s]
                let r = f.boundRect()
                f.moveBaseForm(((.maxP + .minP) / 2 - (cast(s32) r.width / 2)) - cast(s32) r.x, 0, 0, 0)
            }

        case AlignTop:
            foreach s in .selection
            {
                let f = .capture.forms[s]
                let r = f.boundRect()
                f.moveBaseForm(0, .minP - cast(s32) r.y, 0, 0)
            }

        case AlignBottom:
            foreach s in .selection
            {
                let f = .capture.forms[s]
                let r = f.boundRect()
                f.moveBaseForm(0, (.maxP - cast(s32) r.height) - cast(s32) r.y, 0, 0)
            }

        case AlignMiddle:
            foreach s in .selection
            {
                let f = .capture.forms[s]
                let r = f.boundRect()
                f.moveBaseForm(0, ((.maxP + .minP) / 2 - (cast(s32) r.height / 2)) - cast(s32) r.y, 0, 0)
            }

        case MovePoint:
            .baseForm.movePoint(.pointIdx, .newPosX, .newPosY)

        case MoveBaseForm:
            .baseForm.moveBaseForm(.newPosX, .newPosY, .newSizeX, .newSizeY)

        case MoveCapture:
            .capture.backImagePos.x += .newPosX
            .capture.backImagePos.y += .newPosY
            foreach f in .capture.forms do
                f.moveForm(.newPosX, .newPosY)
            .capture.width += .newSizeX
            .capture.height += .newSizeY
            .capture.width  = Math.clamp(.capture.width, Capture.MinSize, Capture.MaxSize)
            .capture.height = Math.clamp(.capture.height, Capture.MinSize, Capture.MaxSize)

        case OffsetSelection:
            foreach s in .selection
            {
                let f = .capture.forms[s]
                f.moveForm(.newPosX, .newPosY)
            }

        case MoveToFront:
            .capture.hotForm = Swag.U64.Max
            var dstIdx = cast(s32) (.capture.forms.count - 1)
            for #reverse i in .selection
            {
                let srcIdx = .selection[i]
                if srcIdx != dstIdx
                {
                    let form = .capture.forms[srcIdx]
                    .capture.forms.removeAtOrdered(srcIdx)
                    .capture.forms.insertAt(cast() dstIdx, form)
                    .capture.selection.remove(srcIdx)
                    .capture.selection.add(cast() dstIdx)
                }

                dstIdx -= 1
            }

        case MoveToBack:
            .capture.hotForm = Swag.U64.Max
            var dstIdx = 0
            for i in .selection
            {
                let srcIdx = .selection[i]
                if srcIdx != dstIdx
                {
                    let form = .capture.forms[srcIdx]
                    .capture.forms.removeAtOrdered(srcIdx)
                    .capture.forms.insertAt(cast() dstIdx, form)
                    .capture.selection.remove(srcIdx)
                    .capture.selection.add(cast() dstIdx)
                }

                dstIdx += 1
            }

        case MoveForward:
            .capture.hotForm = Swag.U64.Max
            for #reverse i in .selection
            {
                let srcIdx = .selection[i]
                if srcIdx != .capture.forms.count - 1
                {
                    let form = .capture.forms[srcIdx]
                    .capture.forms.removeAtOrdered(srcIdx)
                    .capture.forms.insertAt(srcIdx + 1, form)
                    .capture.selection.remove(srcIdx)
                    .capture.selection.add(srcIdx + 1)
                }
            }

        case MoveBackward:
            .capture.hotForm = Swag.U64.Max
            for i in .selection
            {
                let srcIdx = .selection[i]
                if srcIdx != 0
                {
                    let form = .capture.forms[srcIdx]
                    .capture.forms.removeAtOrdered(srcIdx)
                    .capture.forms.insertAt(srcIdx - 1, form)
                    .capture.selection.remove(srcIdx)
                    .capture.selection.add(srcIdx - 1)
                }
            }

        case DeleteSelection:
            .removeSelection()

        case CreateForm:
            .toFreeForms.clear()
            foreach f in .oldForms do
                .capture.forms.add(f)
        }

        .capture.hasChanged()
    }

    mtd undoIt()
    {
        #[Swag.Complete]
        switch .kind
        {
        case RestoreOrg:
            with me.capture
            {
                .width   = me.oldS32[0]
                .height  = me.oldS32[1]
                .backImg = me.oldImg
                .forms   = me.oldForms
                .selection.clear()
                .dirtyBackImg = true
                .backImagePos = me.oldImagePos
                .selectionHasChanged()
            }

        case Flatten, FlattenAll:
            with me.capture
            {
                .backImg      = me.oldImg
                .backImagePos = me.oldImagePos
                .forms        = me.oldForms
                .selection.clear()
                .dirtyBackImg = true
                .selectionHasChanged()
            }

        case SetImage:
            .capture.backImg      = .oldImg
            .capture.backImagePos = .oldImagePos
            .capture.dirtyBackImg = true

        case SetImageInside:
            foreach s, i in .selection
            {
                let f = .capture.forms[s]
                if @kindof(f) != FormImage do
                    continue
                let b = cast(*FormImage) @dataof(f)
                if b.kind != .Image do
                    continue
                b.image = .oldImage[i]
                b.dirty = true
            }

        case SetText:
            foreach s, i in .selection
            {
                let f = .capture.forms[s]
                if @kindof(f) != FormText do
                    continue
                let b = cast(*FormText) @dataof(f)
                b.text    = .oldText[i]
                b.isDirty = true
            }

        case SetFont:
            foreach s, i in .selection
            {
                let f = .capture.forms[s]
                if @kindof(f) != FormText do
                    continue
                let b = cast(*FormText) @dataof(f)
                b.fontName    = .oldText[i]
                b.fontStyle   = cast() .oldU32[i]
                b.isDirtyFont = true
            }

        case SetFontStyle:
            foreach s, i in .selection
            {
                let f = .capture.forms[s]
                if @kindof(f) != FormText do
                    continue
                let b = cast(*FormText) @dataof(f)
                b.fontStyle   = cast() .oldU32[i]
                b.isDirtyFont = true
            }

        case SetFormFromModel:
            foreach s, i in .selection
            {
                let f = .capture.forms[s]
                if !.acceptsType(@kindof(f)) do
                    continue
                let bf = cast(*BaseForm) @dataof(f)
                if bf.subKind != .mdlValue.subKind do
                    continue
                f.copy(cast(*BaseForm) @dataof(f), .oldForm[i])
            }

        case SetTypedValue:
            foreach s, i in .selection
            {
                let f = .capture.forms[s]
                if !.acceptsType(@kindof(f)) do
                    continue
                var val = .oldU32[i]
                Reflection.setFieldValue(@dataof(f), .valRef, &val, true)
            }
            .capture.updateModelFromSelection()

        case AlignLeft, AlignRight, AlignCenter, AlignTop, AlignBottom, AlignMiddle:
            foreach s, i in .selection
            {
                let b = cast(*BaseForm) @dataof(.capture.forms[s])
                b.x = .oldS32[i]
            }

        case MovePoint:
            .baseForm.movePoint(.pointIdx, -.newPosX, -.newPosY)

        case MoveBaseForm:
            .baseForm.moveBaseForm(-.newPosX, -.newPosY, -.newSizeX, -.newSizeY)

        case MoveCapture:
            .capture.backImagePos.x += -.newPosX
            .capture.backImagePos.y += -.newPosY
            foreach f in .capture.forms do
                f.moveForm(-.newPosX, -.newPosY)
            .capture.width -= .newSizeX
            .capture.height -= .newSizeY

        case OffsetSelection:
            foreach s in .selection
            {
                let f = .capture.forms[s]
                f.moveForm(-.newPosX, -.newPosY)
            }

        case MoveToFront, MoveToBack, MoveForward, MoveBackward, DeleteSelection:
            .toFreeForms.clear()
            .capture.selection = .selection
            .capture.forms     = .oldForms
            .capture.selectionHasChanged()

        case CreateForm:
            .toFreeForms = .oldForms
            for .oldForms
            {
                .capture.selection.remove(.capture.forms.count - 1)
                .capture.forms.removeBack()
            }

            .capture.selectionHasChanged()
        }

        .capture.hasChanged()
    }
}
