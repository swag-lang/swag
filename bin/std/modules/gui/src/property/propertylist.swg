#global public
using Core, Pixel

impl PropertyList
{
    // Start a new category
    attr Category(name: string)

    // Start a new sub category
    attr SubCategory(name: string)

    // Change the field display name
    attr Name(name: string)

    // Add a description
    attr Description(desc: string)

    // Native float edit width a slider
    attr EditSlider(min, max: f32)
}

interface IPropertyValue
{
    mtd edit(list: *PropertyList, item: *PropertyListItem, ptrData: ^u8, value: Swag.TypeValue)
}

struct PropertyList
{
    using frameWnd:            FrameWnd

    nameMarginBefore:          s32 = 25
    nameHeight:                s32 = 25
    nameMarginAfter:           s32 = 5
    descMarginAfter:           s32 = 5
    enumFlagsMarginBefore:     s32 = 10
    maxSizeCombo:              s32 = 200
    maxSizeEditValue:          s32 = 200

    sigChanged:                SigArray'closure(*Self)

    font0:                     FontFamily
    font1:                     FontFamily
    font2:                     FontFamily
    scrollWnd:                 *ScrollWnd
    edView:                    *PropertyListView
    items:                     ArrayPtr'PropertyListItem
    categs:                    Array'{ item: *PropertyListItem, lvl: s32 }
}

struct PropertyListItem
{
    filter:             String
    line:               *Wnd
    ptrData:            *u8
    type:               typeinfo
    typeValue:          Swag.TypeValue
    forceShow:          *Wnd

    label:              *Label
    labelDesc:          *Label

    editBox:            *EditBox
    checkBox:           *CheckButton
    comboBox:           *ComboBox
    sliderBox:          *SliderCtrl

    onResize:           SigArray'closure(*Self)

    aniFocusBk:         BlendColor
    aniFocusBorder:     BlendColor
    extendFocus:        f32 = 10
}

internal impl PropertyList
{
    func onEvent(item: *PropertyListItem, wnd: *Wnd, evt: *Event)->bool
    {
        switch evt.kind
        {
        case Notify:
            let ntfyEvt = cast(*NotifyEvent) evt
            if ntfyEvt.ntfyKind == .ChildFocusChanged
            {
                wnd.invalidate()
                return true
            }

        case PrePaint:
            let paintEvt = cast(*PaintEvent) evt
            let painter  = paintEvt.bc.painter
            let colors   = wnd.getThemeColors()

            var colBk, colBorder: Color
            if wnd.isParentOf(wnd.getApp().keybFocusWnd)
            {
                colBk     = colors.propList_focusBk
                colBorder = colors.propList_focusBorder
            }

            item.aniFocusBk.apply(wnd, colBk)
            item.aniFocusBorder.apply(wnd, colBorder)

            var r = wnd.getClientRect()
            r.moveTop(wnd.childs.front().position.y)
            r.inflate(item.extendFocus)
            painter.fillRoundRect(r, 4, 4, item.aniFocusBk.cur())
            painter.drawRoundRect(r, 4, 4, item.aniFocusBorder.cur())
        }

        return false
    }

    ////////////////////////////
    // Refresh
    ////////////////////////////

    func refreshEnumFlags(item: *PropertyListItem)
    {
        if item.checkBox
        {
            var val1, val2: u64
            switch item.typeValue.pointedType.sizeof
            {
            case 1:
                val1 = cast(u64) dref cast(*u8) item.ptrData
                val2 = cast(u64) dref cast(const *u8) item.typeValue.value
            case 2:
                val1 = cast(u64) dref cast(*u16) item.ptrData
                val2 = cast(u64) dref cast(const *u16) item.typeValue.value
            case 4:
                val1 = cast(u64) dref cast(*u32) item.ptrData
                val2 = cast(u64) dref cast(const *u32) item.typeValue.value
            case 8:
                val1 = cast(u64) dref cast(*u64) item.ptrData
                val2 = cast(u64) dref cast(const *u64) item.typeValue.value
            }

            item.checkBox.checked = val1 & val2 ? .Checked : .UnChecked
            item.checkBox.invalidate()
        }
    }

    func refreshEnum(item: *PropertyListItem)
    {
        if item.comboBox
        {
            let typeEnum = cast(const *Swag.TypeInfoEnum) item.typeValue.pointedType

            var val1, val2: u64
            var val1s, val2s: string
            switch typeEnum.rawType.sizeof
            {
            case 1:
                val1 = cast(u64) dref cast(*u8) item.ptrData
            case 2:
                val1 = cast(u64) dref cast(*u16) item.ptrData
            case 4:
                val1 = cast(u64) dref cast(*u32) item.ptrData
            case 8:
                val1 = cast(u64) dref cast(*u64) item.ptrData
            case 16:
                val1s = dref cast(*string) item.ptrData
            }

            var idxSel = 0'u64
            visit v, idx: typeEnum.values
            {
                switch typeEnum.rawType.sizeof
                {
                case 1:
                    val2 = cast(u64) dref cast(const *u8) v.value
                case 2:
                    val2 = cast(u64) dref cast(const *u16) v.value
                case 4:
                    val2 = cast(u64) dref cast(const *u32) v.value
                case 8:
                    val2 = cast(u64) dref cast(const *u64) v.value
                case 16:
                    val2s = dref cast(const *string) v.value
                }

                if val1 == val2 and val1s == val2s do
                    idxSel = idx
            }

            item.comboBox.selectItem(acast idxSel)
        }
    }

    func refreshNative(item: *PropertyListItem)
    {
        let nativeType = cast(const *Swag.TypeInfoNative) item.typeValue.pointedType
        let ptrData    = item.ptrData

        if item.sliderBox
        {
            #[Swag.Mixin]
            func(T) setEd()
            {
                let p = cast(*T) ptrData
                item.sliderBox.slider.setValue(cast(f32) dref p)
            }

            item.sliderBox.invalidate()
            switch nativeType.nativeKind
            {
            case U8:
                setEd'u8()
            case U16:
                setEd'u16()
            case U32:
                setEd'u32()
            case U64:
                setEd'u64()
            case S8:
                setEd's8()
            case S16:
                setEd's16()
            case S32:
                setEd's32()
            case S64:
                setEd's64()
            case F32:
                setEd'f32()
            case F64:
                setEd'f64()
            }
        }
        elif item.editBox
        {
            #[Swag.Mixin]
            func(T) setEd()
            {
                let p = cast(*T) ptrData
                item.editBox.setText(Format.toString("%", dref p))
            }

            item.editBox.invalidate()
            switch nativeType.nativeKind
            {
            case U8:
                setEd'u8()
            case U16:
                setEd'u16()
            case U32:
                setEd'u32()
            case U64:
                setEd'u64()
            case S8:
                setEd's8()
            case S16:
                setEd's16()
            case S32:
                setEd's32()
            case S64:
                setEd's64()
            case F32:
                setEd'f32()
            case F64:
                setEd'f64()
            }
        }
    }

    func refreshBool(item: *PropertyListItem)
    {
        if item.checkBox
        {
            item.checkBox.checked = dref cast(*bool) item.ptrData ? .Checked : .UnChecked
            item.checkBox.invalidate()
        }
    }

    func refreshItem(item: *PropertyListItem)
    {
        let pointed = item.type orelse item.typeValue.pointedType
        if pointed == null do
            return

        if Reflection.isEnum(pointed) and Reflection.hasAttribute(pointed, Swag.EnumFlags) do
            refreshEnumFlags(item)
        elif Reflection.isEnum(pointed) do
            refreshEnum(item)
        elif Reflection.isBool(pointed) do
            refreshBool(item)
        elif Reflection.isInteger(pointed) or Reflection.isFloat(pointed) do
            refreshNative(item)
        elif Reflection.isString(pointed) and item.editBox do
            item.editBox.setText(dref cast(*string) item.ptrData)
        elif pointed == Core.String and item.editBox do
            item.editBox.setText(dref cast(*String) item.ptrData)
    }

    ////////////////////////////
    // Validate
    ////////////////////////////

    mtd validateEnumFlags(item: *PropertyListItem)
    {
        if item.checkBox
        {
            #[Swag.Mixin]
            func(T) setEd()
            {
                let p  = cast(*T) item.ptrData
                let p1 = cast(*T) item.checkBox.userData
                if item.checkBox.checked == .Checked do
                    dref p |= dref p1
                else do
                    dref p &= ~(dref p1)
            }

            switch item.typeValue.pointedType.sizeof
            {
            case 1:
                setEd'u8()
            case 2:
                setEd'u16()
            case 4:
                setEd'u32()
            case 8:
                setEd'u64()
            }
        }

        sigChanged.call(self)
    }

    mtd validateEnum(item: *PropertyListItem)
    {
        if item.comboBox
        {
            #[Swag.Mixin]
            func(T) setEd()
            {
                let p  = cast(*T) item.ptrData
                let p1 = cast(const *T) typeEnum.values[sel].value
                dref p = dref p1
            }

            let typeEnum = cast(const *Swag.TypeInfoEnum) item.typeValue.pointedType
            let sel      = item.comboBox.selectedIdx
            switch typeEnum.rawType.sizeof
            {
            case 1:
                setEd'u8()
            case 2:
                setEd'u16()
            case 4:
                setEd'u32()
            case 8:
                setEd'u64()
            case 16:
                setEd'string()
            }
        }

        sigChanged.call(self)
    }

    mtd validateBool(item: *PropertyListItem)
    {
        var ptrData = cast(*bool) item.ptrData
        if item.checkBox do
            dref ptrData = item.checkBox.checked == .Checked ? true : false
        sigChanged.call(self)
    }

    mtd validateString(item: *PropertyListItem)
    {
        var ptrData = cast(*String) item.ptrData
        if item.editBox do
            dref ptrData = item.editBox.getText()
        sigChanged.call(self)
    }

    mtd validateNative(item: *PropertyListItem)
    {
        let nativeType = cast(const *Swag.TypeInfoNative) item.typeValue.pointedType
        let ptrData    = item.ptrData

        if item.sliderBox
        {
            #[Swag.Mixin]
            func(T) setEd()
            {
                var p = cast(*T) ptrData
                dref p = cast(T) item.sliderBox.slider.getValue()
            }

            switch nativeType.nativeKind
            {
            case U8:
                setEd'u8()
            case U16:
                setEd'u16()
            case U32:
                setEd'u32()
            case U64:
                setEd'u64()
            case S8:
                setEd's8()
            case S16:
                setEd's16()
            case S32:
                setEd's32()
            case S64:
                setEd's64()
            case F32:
                setEd'f32()
            case F64:
                setEd'f64()
            }
        }
        elif item.editBox
        {
            #[Swag.Mixin]
            func(T) setEd()
            {
                var p = cast(*T) ptrData
                dref p = catch StrConv.toNum'T(txt)
            }

            let txt = item.editBox.getText()
            switch nativeType.nativeKind
            {
            case U8:
                setEd'u8()
            case U16:
                setEd'u16()
            case U32:
                setEd'u32()
            case U64:
                setEd'u64()
            case S8:
                setEd's8()
            case S16:
                setEd's16()
            case S32:
                setEd's32()
            case S64:
                setEd's64()
            case F32:
                setEd'f32()
            case F64:
                setEd'f64()
            }
        }

        sigChanged.call(self)
    }

    ////////////////////////////
    // Add
    ////////////////////////////

    #[Swag.Overload]
    mtd addItem(f: Swag.TypeValue, ptrData: *u8, canDesc = true)->*PropertyListItem
    {
        let line = StackLayoutCtrl.create(edView, .TopAutoSize)
        line.margin.x, line.margin.z = 15
        line.wndFlags.set(.PreChildsPaint)

        let item = items.addNewPtr()
        item.line        = line
        line.hookOnEvent = closure|item|(wnd, evt) { return onEvent(item, wnd, evt); }
        item.type        = f.pointedType
        item.typeValue   = f
        item.ptrData     = ptrData

        // Name
        var dispName: String
        let attrName  = Reflection.getAttributeValue(f, PropertyList.Name, "name")
        if attrName != null do
            dispName = cast(string) attrName
        else do
            dispName = Format.toString("<b>%<b>", f.name)

        with let label = Label.create(line, dispName, {0, 0, position.width, nameHeight})
        {
            .labelFlags |= .WordWrap | .AutoHeight
            .horzAlignmentText = .Justify
            .margin.y          = nameMarginBefore
            .margin.w          = nameMarginAfter
        }

        item.label  = label
        item.filter = dispName

        // Description
        let dispDesc = Reflection.getAttributeValue(f, PropertyList.Description, "desc")
        if dispDesc != null
        {
            item.filter += " "
            item.filter += cast(string) dispDesc
            if canDesc
            {
                let labelDesc = Label.create(line, "", {0, 0, position.width, 0})
                labelDesc.labelFlags |= .WordWrap | .AutoHeight
                labelDesc.horzAlignmentText = .Justify
                labelDesc.setText(dispDesc)
                labelDesc.margin.w = descMarginAfter
                item.labelDesc     = labelDesc
            }
        }

        return item
    }

    mtd addEnum(f: Swag.TypeValue, typeEnum: const *Swag.TypeInfoEnum, ptrData: *u8)
    {
        let item = addItem(f, ptrData)
        let line = item.line

        if Reflection.hasAttribute(typeEnum, Swag.EnumFlags)
        {
            visit v: typeEnum.values
            {
                let itemB = items.addNewPtr()
                itemB.type        = typeEnum
                itemB.typeValue   = v
                itemB.ptrData     = ptrData
                itemB.extendFocus = 5
                itemB.filter      = item.filter

                let line1 = Wnd.create'Wnd(edView)
                line1.margin.x, line1.margin.z = 15
                line1.margin.y = enumFlagsMarginBefore
                line1.wndFlags.set(.PreChildsPaint)
                line1.hookOnEvent = closure|itemB|(wnd, evt) { return onEvent(itemB, wnd, evt); }

                let ed = CheckButton.create(line1, "", {0, 0, 30, 0})
                ed.dockStyle = .Left

                let dispDesc = Reflection.getAttributeValue(v, PropertyList.Description, "desc")
                itemB.filter += cast(string) dispDesc

                with let label1 = Label.create(line1, "", {30, 0, position.width - 30})
                {
                    .dockStyle = .Center
                    .labelFlags |= .WordWrap | .AutoHeight
                    .horzAlignmentText = .Justify
                    .setText(dispDesc)
                }

                let minHeight = ed.position.height
                itemB.onResize += closure|label1, line1, minHeight|(itemB)
                {
                    label1.autoHeight()
                    line1.position.height = Math.max(minHeight, label1.position.height)
                    line1.applyLayout()
                }

                itemB.line      = line1
                itemB.forceShow = item.line
                itemB.checkBox  = ed
                ed.userData     = cast,unconst(*void) v.value

                refreshEnumFlags(itemB)
                itemB.checkBox.sigChanged += closure|self, itemB|(ed) { self.validateEnumFlags(itemB); }
            }
        }
        else
        {
            let ed = ComboBox.create(line, kind: .CheckBox)
            ed.maxSize.x   = maxSizeCombo
            ed.anchorStyle = .X
            item.comboBox  = ed
            visit v, idx: typeEnum.values do
                ed.addItem(v.name)

            refreshEnum(item)
            item.comboBox.sigChanged += closure|self, item|(ed, n) { self.validateEnum(item); }
        }
    }

    mtd addString(f: Swag.TypeValue, ptrData: *u8)
    {
        let item = addItem(f, ptrData)
        let ed   = EditBox.create(item.line, dref cast(*String) ptrData)
        item.editBox = ed
        ed.sigLoseFocus += closure|self, item|(ed) { self.validateString(item); }
    }

    mtd addBool(f: Swag.TypeValue, ptrData: *u8)
    {
        let item = addItem(f, ptrData, false)

        let line1 = Wnd.create'Wnd(item.line)
        let ed    = CheckButton.create(line1, "", {0, 0, 30, 0})
        ed.dockStyle = .Left

        let dispDesc = Reflection.getAttributeValue(f, PropertyList.Description, "desc")
        with let label1 = Label.create(line1, "", {30, 0, position.width - 30})
        {
            .dockStyle = .Center
            .labelFlags |= .WordWrap | .AutoHeight
            .horzAlignmentText = .Justify
            .setText(dispDesc)
        }

        let minHeight = ed.position.height
        item.onResize += closure|label1, line1, minHeight|(item)
        {
            label1.autoHeight()
            line1.position.height = Math.max(minHeight, label1.position.height)
            item.line.applyLayout()
        }

        item.checkBox = ed
        ed.sigChanged += closure|self, item|(ed) { self.validateBool(item); }

        refreshBool(item)
    }

    mtd addNative(f: Swag.TypeValue, nativeType: const *Swag.TypeInfoNative, ptrData: *u8)
    {
        let item = addItem(f, ptrData)
        let line = item.line

        if Reflection.hasAttribute(f, EditSlider)
        {
            let slider = SliderCtrl.create(line, null)
            slider.labelSize = 0
            item.sliderBox   = slider

            let minV = cast(f32) Reflection.getAttributeValue(f, EditSlider, "min")
            let maxV = cast(f32) Reflection.getAttributeValue(f, EditSlider, "max")

            switch nativeType.nativeKind
            {
            case U8, U16, U32, U64:
                slider.setMinMaxU64(acast minV, acast maxV)
            case S8, S16, S32, S64:
                slider.setMinMaxS64(acast minV, acast maxV)
            case F32, F64:
                slider.setMinMaxF32(minV, maxV)
            }
        }
        else
        {
            let ed = EditBox.create(line, f.name)
            ed.maxSize.x   = maxSizeEditValue
            ed.anchorStyle = .X
            item.editBox   = ed

            switch nativeType.nativeKind
            {
            case U8:
                ed.setInputU64(Swag.U8.Min, Swag.U8.Max)
            case U16:
                ed.setInputU64(Swag.U16.Min, Swag.U16.Max)
            case U32:
                ed.setInputU64(Swag.U32.Min, Swag.U32.Max)
            case U64:
                ed.setInputU64(Swag.U64.Min, Swag.U64.Max)
            case S8:
                ed.setInputS64(Swag.S8.Min, Swag.S8.Max)
            case S16:
                ed.setInputS64(Swag.S16.Min, Swag.S16.Max)
            case S32:
                ed.setInputS64(Swag.S32.Min, Swag.S32.Max)
            case S64:
                ed.setInputS64(Swag.S64.Min, Swag.S64.Max)
            case F32, F64:
                ed.setInputF64()
            }
        }

        refreshNative(item)

        if item.sliderBox do
            item.sliderBox.sigChanged += closure|self, item|(ed) { self.validateNative(item); }
        elif item.editBox do
            item.editBox.sigLoseFocus += closure|self, item|(ed) { self.validateNative(item); }
    }

    mtd addStruct(addr: ^u8, typeStruct: const *Swag.TypeInfoStruct)
    {
        visit f: typeStruct.fields
        {
            let ptrData = addr + f.offset

            // Category
            let categ = Reflection.getAttributeValue(f, PropertyList.Category, "name")
            if categ != null
            {
                var v: Swag.TypeValue
                v.name = cast(string) categ
                let item = addItem(v, null)
                item.label.style.setFont(&font0)
                categs.add({item, 0})
            }

            // Sub Category
            let subCateg = Reflection.getAttributeValue(f, PropertyList.SubCategory, "name")
            if subCateg != null
            {
                var v: Swag.TypeValue
                v.name = cast(string) subCateg
                let item = addItem(v, null)
                item.label.style.setFont(&font1)
                categs.add({item, 1})
            }

            // Value
            switch
            {
            case Reflection.isEnum(f.pointedType):
                let typeEnum = cast(const *Swag.TypeInfoEnum) f.pointedType
                addEnum(f, typeEnum, ptrData)

            case f.pointedType == Core.String:
                addString(f, ptrData)

            case Reflection.isString(f.pointedType):
                let item = addItem(f, ptrData)
                let ed   = EditBox.create(item.line, dref cast(*string) ptrData)
                ed.editBoxFlags = .ReadOnly
                item.editBox    = ed

            case Reflection.isBool(f.pointedType):
                addBool(f, ptrData)

            case Reflection.isInteger(f.pointedType) or Reflection.isFloat(f.pointedType):
                let nativeType = cast(const *Swag.TypeInfoNative) f.pointedType
                addNative(f, nativeType, ptrData)

            case Reflection.isStruct(f.pointedType):
                let item          = addItem(f, ptrData)
                let typeSubStruct = cast(const *Swag.TypeInfoStruct) f.pointedType

                // A structure with an implementation of IPropertyValue
                let iprop = @mkinterface(ptrData, f.pointedType, IPropertyValue)
                if iprop != null
                {
                    iprop.edit(self, item, ptrData, f)
                }
                else
                {
                    item.label.style.setFont(&font2)
                    addStruct(ptrData, typeSubStruct)
                }
            }
        }
    }
}

impl PropertyList
{
    // Creates a new PropertyList instance
    #[Swag.Discardable]
    func create(parent: *Wnd, position: Math.Rectangle = {}, id: WndId = null)->*PropertyList
    {
        let ed = Wnd.create'PropertyList(parent, "", position, id)

        with ed
        {
            .scrollWnd               = .createView'ScrollWnd()
            .scrollWnd.dockStyle     = .Center
            .scrollWnd.focusStrategy = .None
            .scrollWnd.padding       = 0
            .scrollWnd.scrollWndFlags |= .TransparentBar
            .edView    = .scrollWnd.createView'PropertyListView()
            .edView.ed = ed
            .edView.wndFlags |= .NoScroll
            .edView.layoutKind = .Top
            .edView.spacing    = 0

            let theme = parent.getTheme()

            const Font0Size = 30'u32
            .font0.regular    = Font.create(theme.res.defaultTypeFaceR, Font0Size)
            .font0.bold       = Font.create(theme.res.defaultTypeFaceB, Font0Size)
            .font0.italic     = Font.create(theme.res.defaultTypeFaceI, Font0Size)
            .font0.boldItalic = Font.create(theme.res.defaultTypeFaceBI, Font0Size)

            const Font1Size = 25'u32
            .font1.regular    = Font.create(theme.res.defaultTypeFaceR, Font1Size)
            .font1.bold       = Font.create(theme.res.defaultTypeFaceB, Font1Size)
            .font1.italic     = Font.create(theme.res.defaultTypeFaceI, Font1Size)
            .font1.boldItalic = Font.create(theme.res.defaultTypeFaceBI, Font1Size)

            const Font2Size = 20'u32
            .font2.regular    = Font.create(theme.res.defaultTypeFaceR, Font2Size)
            .font2.bold       = Font.create(theme.res.defaultTypeFaceB, Font2Size)
            .font2.italic     = Font.create(theme.res.defaultTypeFaceI, Font2Size)
            .font2.boldItalic = Font.create(theme.res.defaultTypeFaceBI, Font2Size)
        }

        return ed
    }

    // Add a new struct to display.
    #[Swag.Overload]
    mtd addItem(val: any)
    {
        let dataof = @dataof(val)
        let typeof = @kindof(val)
        Debug.assert(typeof.kind == .Struct)

        let typeStruct = cast(const *Swag.TypeInfoStruct) typeof
        addStruct(dataof, typeStruct)

        edView.sendResizeEvent()
    }

    // Refresh all displayed values.
    mtd refresh()
    {
        visit it: items do
            refreshItem(it)
    }

    // Show hidden content depending on the input 'filter'.
    mtd setStringFilter(filter: string)
    {
        visit it: items
        {
            if filter == null or @countof(filter) == 0 do
                it.line.show()
            elif Utf8.indexOf(it.filter.toSlice(), filter, comparisonType: Utf8.ComparisonType.Latin1NoCase) != Swag.U64.Max
            {
                it.line.show()
                if it.forceShow do
                    it.forceShow.show()
            }
            else do
                it.line.hide()
        }

        applyLayout()
        scrollWnd.view.sendResizeEvent()
    }
}
