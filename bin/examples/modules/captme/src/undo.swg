#global marked
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
    valRef:          const *Swag.TypedValue
    mdlType:         typeinfo
    mdlValue:        *BaseForm
    text:            String
    fontStyle:       FontFamilyStyle
}

impl Undo
{
    mtd opDrop()
    {
        Capture.freeForms(me.toFreeForms)

        if me.kind == .SetFormFromModel
        {
            Memory.delete(me.mdlValue, me.mdlType)
            foreach f in me.oldForm:
                Memory.delete(f, me.mdlType)
        }
    }

    mtd acceptsType(type: typeinfo)->bool
    {
        if me.mdlType == null:
            return true
        if me.mdlType == type:
            return true
        if me.mdlType == FormLine and type == FormArrow:
            return true
        return false
    }

    mtd init()
    {
        switch me.kind
        {
        case RestoreOrg:
            me.oldImagePos = me.capture.backImagePos
            me.oldImg      = me.capture.backImg
            me.oldForms    = me.capture.forms
            me.oldS32.add(me.capture.width)
            me.oldS32.add(me.capture.height)

        case Flatten:
            me.selection   = me.capture.selection
            me.oldImg      = me.capture.backImg
            me.oldImagePos = me.capture.backImagePos
            me.oldForms    = me.capture.forms

        case FlattenAll:
            me.oldImg      = me.capture.backImg
            me.oldImagePos = me.capture.backImagePos
            me.oldForms    = me.capture.forms

        case SetImage:
            me.oldImg      = me.capture.backImg
            me.oldImagePos = me.capture.backImagePos

        case SetImageInside:
            me.selection = me.capture.selection
            foreach s in me.selection
            {
                let f = me.capture.forms[s]
                if @kindof(f) != FormImage:
                    continue
                let b = cast(*FormImage) @dataof(f)
                if b.kind != .Image:
                    continue
                me.oldImage.add(b.image)
            }

        case SetText:
            me.selection = me.capture.selection
            foreach s in me.selection
            {
                let f = me.capture.forms[s]
                if @kindof(f) != FormText:
                    continue
                let b = cast(*FormText) @dataof(f)
                me.oldText.add(b.text)
            }

        case SetFont:
            me.selection = me.capture.selection
            foreach s in me.selection
            {
                let f = me.capture.forms[s]
                if @kindof(f) != FormText:
                    continue
                let b = cast(*FormText) @dataof(f)
                me.oldText.add(b.fontName)
                me.oldU32.add(cast(u32) b.fontStyle)
            }

        case SetFontStyle:
            me.selection = me.capture.selection
            foreach s in me.selection
            {
                let f = me.capture.forms[s]
                if @kindof(f) != FormText:
                    continue
                let b = cast(*FormText) @dataof(f)
                me.oldU32.add(cast(u32) b.fontStyle)
            }

        case SetFormFromModel:
            me.selection = me.capture.selection
            foreach s in me.selection
            {
                let f = me.capture.forms[s]
                if !me.acceptsType(@kindof(f)):
                    continue
                let save = cast(*BaseForm) Memory.new(me.mdlType)
                f.copy(save, cast(*BaseForm) @dataof(f))
                me.oldForm.add(save)
            }

        case AlignLeft, AlignCenter, AlignRight:
            me.selection = me.capture.selection
            me.minP      = Swag.S32.Max
            me.maxP      = Swag.S32.Min
            me.oldS32.reserve(me.selection.count)
            foreach s in me.selection
            {
                let f = me.capture.forms[s]
                let r = f.boundRect()
                me.oldS32.add(cast(s32) r.x)
                me.minP = Math.min(me.minP, cast(s32) r.x)
                me.maxP = Math.max(me.maxP, cast(s32) (r.x + r.width))
            }

        case AlignTop, AlignMiddle, AlignBottom:
            me.selection = me.capture.selection
            me.minP      = Swag.S32.Max
            me.maxP      = Swag.S32.Min
            me.oldS32.reserve(me.selection.count)
            foreach s in me.selection
            {
                let f = me.capture.forms[s]
                let r = f.boundRect()
                me.oldS32.add(cast(s32) r.x)
                me.minP = Math.min(me.minP, cast(s32) r.y)
                me.maxP = Math.max(me.maxP, cast(s32) (r.y + r.height))
            }

        case OffsetSelection:
            me.selection = me.capture.selection

        case SetTypedValue:
            me.selection = me.capture.selection
            foreach s in me.selection
            {
                let f = me.capture.forms[s]
                if !me.acceptsType(@kindof(f)):
                    continue
                var val: u32
                Reflection.getFieldValue(&val, me.valRef, @dataof(f), true)
                me.oldU32.add(val)
            }

        case MoveToFront, MoveToBack, MoveBackward, MoveForward:
            me.selection = me.capture.selection
            me.oldForms  = me.capture.forms
            me.selection.sort()

        case DeleteSelection:
            me.selection = me.capture.selection
            me.oldForms  = me.capture.forms
        }
    }

    mtd doIt()
    {
        mtd removeSelection()
        {
            me.toFreeForms.clear()
            me.selection.sort()
            var offset = 0
            foreach s in me.selection
            {
                let f = me.capture.forms[s - offset]
                offset += 1
                me.capture.forms.removeOrdered(f)
                me.toFreeForms.add(f)
            }

            me.capture.selection.clear()
            me.capture.selectionHasChanged()
        }

        #[Swag.Complete]
        switch me.kind
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
            me.removeSelection()

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
            me.capture.backImg      = me.newImg
            me.capture.backImagePos = me.newImagePos
            me.capture.dirtyBackImg = true

        case SetImageInside:
            foreach s, i in me.selection
            {
                let f = me.capture.forms[s]
                if @kindof(f) != FormImage:
                    continue
                let b = cast(*FormImage) @dataof(f)
                if b.kind != .Image:
                    continue
                b.image = me.oldImg
                b.dirty = true
            }

        case SetText:
            foreach s, i in me.selection
            {
                let f = me.capture.forms[s]
                if @kindof(f) != FormText:
                    continue
                let b = cast(*FormText) @dataof(f)
                b.text    = me.text
                b.isDirty = true
            }

        case SetFont:
            foreach s, i in me.selection
            {
                let f = me.capture.forms[s]
                if @kindof(f) != FormText:
                    continue
                let b = cast(*FormText) @dataof(f)
                b.fontName    = me.text
                b.fontStyle   = me.fontStyle
                b.isDirtyFont = true
            }

        case SetFontStyle:
            foreach s, i in me.selection
            {
                let f = me.capture.forms[s]
                if @kindof(f) != FormText:
                    continue
                let b = cast(*FormText) @dataof(f)
                b.fontStyle   = me.fontStyle
                b.isDirtyFont = true
            }

        case SetFormFromModel:
            foreach s in me.selection
            {
                let f = me.capture.forms[s]
                if !me.acceptsType(@kindof(f)):
                    continue
                let bf = cast(*BaseForm) @dataof(f)
                if bf.subKind != me.mdlValue.subKind:
                    continue
                f.copy(cast(*BaseForm) @dataof(f), me.mdlValue)
            }

        case SetTypedValue:
            foreach s in me.selection
            {
                let f = me.capture.forms[s]
                if !me.acceptsType(@kindof(f)):
                    continue
                Reflection.setFieldValue(@dataof(f), me.valRef, &me.valU32, true)
            }

            me.capture.updateModelFromSelection()

        case AlignLeft:
            foreach s in me.selection
            {
                let f = me.capture.forms[s]
                let r = f.boundRect()
                f.moveBaseForm(me.minP - cast(s32) r.x, 0, 0, 0)
            }

        case AlignRight:
            foreach s in me.selection
            {
                let f = me.capture.forms[s]
                let r = f.boundRect()
                f.moveBaseForm((me.maxP - cast(s32) r.width) - cast(s32) r.x, 0, 0, 0)
            }

        case AlignCenter:
            foreach s in me.selection
            {
                let f = me.capture.forms[s]
                let r = f.boundRect()
                f.moveBaseForm(((me.maxP + me.minP) / 2 - (cast(s32) r.width / 2)) - cast(s32) r.x, 0, 0, 0)
            }

        case AlignTop:
            foreach s in me.selection
            {
                let f = me.capture.forms[s]
                let r = f.boundRect()
                f.moveBaseForm(0, me.minP - cast(s32) r.y, 0, 0)
            }

        case AlignBottom:
            foreach s in me.selection
            {
                let f = me.capture.forms[s]
                let r = f.boundRect()
                f.moveBaseForm(0, (me.maxP - cast(s32) r.height) - cast(s32) r.y, 0, 0)
            }

        case AlignMiddle:
            foreach s in me.selection
            {
                let f = me.capture.forms[s]
                let r = f.boundRect()
                f.moveBaseForm(0, ((me.maxP + me.minP) / 2 - (cast(s32) r.height / 2)) - cast(s32) r.y, 0, 0)
            }

        case MovePoint:
            me.baseForm.movePoint(me.pointIdx, me.newPosX, me.newPosY)

        case MoveBaseForm:
            me.baseForm.moveBaseForm(me.newPosX, me.newPosY, me.newSizeX, me.newSizeY)

        case MoveCapture:
            me.capture.backImagePos.x += me.newPosX
            me.capture.backImagePos.y += me.newPosY
            foreach f in me.capture.forms:
                f.moveForm(me.newPosX, me.newPosY)
            me.capture.width += me.newSizeX
            me.capture.height += me.newSizeY
            me.capture.width  = Math.clamp(me.capture.width, Capture.MinSize, Capture.MaxSize)
            me.capture.height = Math.clamp(me.capture.height, Capture.MinSize, Capture.MaxSize)

        case OffsetSelection:
            foreach s in me.selection
            {
                let f = me.capture.forms[s]
                f.moveForm(me.newPosX, me.newPosY)
            }

        case MoveToFront:
            me.capture.hotForm = Swag.U64.Max
            var dstIdx = cast(s32) (me.capture.forms.count - 1)
            for #reverse i in me.selection
            {
                let srcIdx = me.selection[i]
                if srcIdx != dstIdx
                {
                    let form = me.capture.forms[srcIdx]
                    me.capture.forms.removeAtOrdered(srcIdx)
                    me.capture.forms.insertAt(cast() dstIdx, form)
                    me.capture.selection.remove(srcIdx)
                    me.capture.selection.add(cast() dstIdx)
                }

                dstIdx -= 1
            }

        case MoveToBack:
            me.capture.hotForm = Swag.U64.Max
            var dstIdx = 0
            for i in me.selection
            {
                let srcIdx = me.selection[i]
                if srcIdx != dstIdx
                {
                    let form = me.capture.forms[srcIdx]
                    me.capture.forms.removeAtOrdered(srcIdx)
                    me.capture.forms.insertAt(cast() dstIdx, form)
                    me.capture.selection.remove(srcIdx)
                    me.capture.selection.add(cast() dstIdx)
                }

                dstIdx += 1
            }

        case MoveForward:
            me.capture.hotForm = Swag.U64.Max
            for #reverse i in me.selection
            {
                let srcIdx = me.selection[i]
                if srcIdx != me.capture.forms.count - 1
                {
                    let form = me.capture.forms[srcIdx]
                    me.capture.forms.removeAtOrdered(srcIdx)
                    me.capture.forms.insertAt(srcIdx + 1, form)
                    me.capture.selection.remove(srcIdx)
                    me.capture.selection.add(srcIdx + 1)
                }
            }

        case MoveBackward:
            me.capture.hotForm = Swag.U64.Max
            for i in me.selection
            {
                let srcIdx = me.selection[i]
                if srcIdx != 0
                {
                    let form = me.capture.forms[srcIdx]
                    me.capture.forms.removeAtOrdered(srcIdx)
                    me.capture.forms.insertAt(srcIdx - 1, form)
                    me.capture.selection.remove(srcIdx)
                    me.capture.selection.add(srcIdx - 1)
                }
            }

        case DeleteSelection:
            me.removeSelection()

        case CreateForm:
            me.toFreeForms.clear()
            foreach f in me.oldForms:
                me.capture.forms.add(f)
        }

        me.capture.hasChanged()
    }

    mtd undoIt()
    {
        #[Swag.Complete]
        switch me.kind
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
            me.capture.backImg      = me.oldImg
            me.capture.backImagePos = me.oldImagePos
            me.capture.dirtyBackImg = true

        case SetImageInside:
            foreach s, i in me.selection
            {
                let f = me.capture.forms[s]
                if @kindof(f) != FormImage:
                    continue
                let b = cast(*FormImage) @dataof(f)
                if b.kind != .Image:
                    continue
                b.image = me.oldImage[i]
                b.dirty = true
            }

        case SetText:
            foreach s, i in me.selection
            {
                let f = me.capture.forms[s]
                if @kindof(f) != FormText:
                    continue
                let b = cast(*FormText) @dataof(f)
                b.text    = me.oldText[i]
                b.isDirty = true
            }

        case SetFont:
            foreach s, i in me.selection
            {
                let f = me.capture.forms[s]
                if @kindof(f) != FormText:
                    continue
                let b = cast(*FormText) @dataof(f)
                b.fontName    = me.oldText[i]
                b.fontStyle   = cast() me.oldU32[i]
                b.isDirtyFont = true
            }

        case SetFontStyle:
            foreach s, i in me.selection
            {
                let f = me.capture.forms[s]
                if @kindof(f) != FormText:
                    continue
                let b = cast(*FormText) @dataof(f)
                b.fontStyle   = cast() me.oldU32[i]
                b.isDirtyFont = true
            }

        case SetFormFromModel:
            foreach s, i in me.selection
            {
                let f = me.capture.forms[s]
                if !me.acceptsType(@kindof(f)):
                    continue
                let bf = cast(*BaseForm) @dataof(f)
                if bf.subKind != me.mdlValue.subKind:
                    continue
                f.copy(cast(*BaseForm) @dataof(f), me.oldForm[i])
            }

        case SetTypedValue:
            foreach s, i in me.selection
            {
                let f = me.capture.forms[s]
                if !me.acceptsType(@kindof(f)):
                    continue
                var val = me.oldU32[i]
                Reflection.setFieldValue(@dataof(f), me.valRef, &val, true)
            }
            me.capture.updateModelFromSelection()

        case AlignLeft, AlignRight, AlignCenter, AlignTop, AlignBottom, AlignMiddle:
            foreach s, i in me.selection
            {
                let b = cast(*BaseForm) @dataof(me.capture.forms[s])
                b.x = me.oldS32[i]
            }

        case MovePoint:
            me.baseForm.movePoint(me.pointIdx, -me.newPosX, -me.newPosY)

        case MoveBaseForm:
            me.baseForm.moveBaseForm(-me.newPosX, -me.newPosY, -me.newSizeX, -me.newSizeY)

        case MoveCapture:
            me.capture.backImagePos.x += -me.newPosX
            me.capture.backImagePos.y += -me.newPosY
            foreach f in me.capture.forms:
                f.moveForm(-me.newPosX, -me.newPosY)
            me.capture.width -= me.newSizeX
            me.capture.height -= me.newSizeY

        case OffsetSelection:
            foreach s in me.selection
            {
                let f = me.capture.forms[s]
                f.moveForm(-me.newPosX, -me.newPosY)
            }

        case MoveToFront, MoveToBack, MoveForward, MoveBackward, DeleteSelection:
            me.toFreeForms.clear()
            me.capture.selection = me.selection
            me.capture.forms     = me.oldForms
            me.capture.selectionHasChanged()

        case CreateForm:
            me.toFreeForms = me.oldForms
            for me.oldForms
            {
                me.capture.selection.remove(me.capture.forms.count - 1)
                me.capture.forms.removeBack()
            }

            me.capture.selectionHasChanged()
        }

        me.capture.hasChanged()
    }
}
