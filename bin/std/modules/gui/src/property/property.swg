#global public
using Core, Pixel

struct PropertyCtrl
{
    using frameWnd: FrameWnd

    flatModeNameSpacing: s32 = 25
    flatModeNameHeight:  s32 = 15

    scrollWnd:      *ScrollWnd
    edView:         *PropertyView
    items:          ArrayPtr'PropertyItem
}

struct PropertyItem
{
    filter:     String
    line:       *Wnd
    ptrData:    *u8
    typeValue:  Swag.TypeValue

    editBox:    *EditBox
    checkBox:   *CheckButton
    comboBox:   *ComboBox
}

private impl PropertyCtrl
{
    mtd addItem(f: Swag.TypeValue, ptrData: *u8, canDesc = true)->*PropertyItem
    {
        line := StackLayoutCtrl.create(edView, .TopAutoSize)
        item := items.addNewPtr()
        item.line = line
        item.typeValue = f
        item.ptrData = ptrData

        // Name
        var dispName: String
        attrName := Reflection.getAttributeValue(f, Property.Name, "name") 
        if attrName != null
            dispName = cast(string) attrName
        else 
            dispName = Format.toString("<b>%<b>", f.name)
        label := Label.create(line, dispName, @{0, 0, 0, flatModeNameHeight})
        label.horzAlignmentText = .Left
        label.margin.y = flatModeNameSpacing
        item.filter = dispName

        // Description
        dispDesc := Reflection.getAttributeValue(f, Property.Description, "desc")
        if dispDesc != null
        {
            item.filter += " "
            item.filter += cast(string) dispDesc
            if canDesc
            {
                labelDesc := Label.create(line, "", @{0, 0, position.width, 0})
                labelDesc.labelFlags |= .WordWrapAutoSize
                labelDesc.horzAlignmentText = .Justify
                labelDesc.setText(dispDesc)
                labelDesc.margin.w = 5
            }
        }

        return item
    }

    mtd refreshEnumFlags(using item: *PropertyItem)
    {
        var val1, val2: u64
        switch typeValue.pointedType.sizeof
        {
        case 1:  
            val1 = cast(u64) dref cast(*u8) ptrData
            val2 = cast(u64) dref cast(const *u8) typeValue.value
        case 2:  
            val1 = cast(u64) dref cast(*u16) ptrData
            val2 = cast(u64) dref cast(const *u16) typeValue.value
        case 4:  
            val1 = cast(u64) dref cast(*u32) ptrData
            val2 = cast(u64) dref cast(const *u32) typeValue.value
        case 8:  
            val1 = cast(u64) dref cast(*u64) ptrData
            val2 = cast(u64) dref cast(const *u64) typeValue.value
        }

        checkBox.checked = val1 & val2 ? .Checked : .UnChecked
    }

    mtd refreshEnum(using item: *PropertyItem)
    {
        typeEnum := cast(const *Swag.TypeInfoEnum) typeValue.pointedType

        var val1, val2: u64
        var val1s, val2s: string
        switch typeEnum.rawType.sizeof
        {
        case 1:  val1 = cast(u64) dref cast(*u8) ptrData
        case 2:  val1 = cast(u64) dref cast(*u16) ptrData
        case 4:  val1 = cast(u64) dref cast(*u32) ptrData
        case 8:  val1 = cast(u64) dref cast(*u64) ptrData
        case 16: val1s = dref cast(*string) ptrData
        }

        idxSel := 0'uint
        visit v, idx: typeEnum.values
        {
            switch typeEnum.rawType.sizeof
            {
            case 1:  val2 = cast(u64) dref cast(const *u8) v.value
            case 2:  val2 = cast(u64) dref cast(const *u16) v.value
            case 4:  val2 = cast(u64) dref cast(const *u32) v.value
            case 8:  val2 = cast(u64) dref cast(const *u64) v.value
            case 16: val2s = dref cast(const *string) v.value
            }

            if val1 == val2 and val1s == val2s
                idxSel = idx
        }
        
        comboBox.selectItem(acast idxSel)
    }

    mtd addEnum(f: Swag.TypeValue, typeEnum: const *Swag.TypeInfoEnum, ptrData: *u8)
    {
        item := addItem(f, ptrData)
        line := item.line

        if Reflection.hasAttribute(typeEnum, Swag.EnumFlags)
        {
            visit v, idx: typeEnum.values
            {
                itemB := items.addNewPtr()                    
                itemB.typeValue = dref v
                itemB.ptrData = ptrData
                dispDesc := Reflection.getAttributeValue(v, Property.Description, "desc")
                strDesc := dispDesc != null ? cast(string) dispDesc : ""
                cb := CheckButton.create(edView, strDesc)
                itemB.checkBox = cb
                itemB.filter = dispDesc
                itemB.line = cb                
                refreshEnumFlags(itemB)
            }
        }
        else
        {
            ed := ComboBox.create(line, kind: .CheckBox)
            ed.maxSize.x = 200
            ed.anchorStyle = .X
            item.comboBox = ed

            visit v, idx: typeEnum.values
            {
                ed.addItem(v.name)
            }

            refreshEnum(item)
        }
    }

    mtd refreshNative(item: *PropertyItem)
    {
        nativeType := cast(const *Swag.TypeInfoNative) item.typeValue.pointedType
        ptrData    := item.ptrData
        ed         := item.editBox

        switch nativeType.nativeKind
        {
        case U8: 
            p := cast(^u8) ptrData
            ed.setText(Format.toString("%", dref p))
        case U16: 
            p := cast(^u16) ptrData
            ed.setText(Format.toString("%", dref p))
        case U32: 
            p := cast(^u32) ptrData
            ed.setText(Format.toString("%", dref p))
        case U64, UInt: 
            p := cast(^u64) ptrData
            ed.setText(Format.toString("%", dref p))
        case S8: 
            p := cast(^s8) ptrData
            ed.setText(Format.toString("%", dref p))
        case S16: 
            p := cast(^s16) ptrData
            ed.setText(Format.toString("%", dref p))
        case S32: 
            p := cast(^s32) ptrData
            ed.setText(Format.toString("%", dref p))
        case S64, Int: 
            p := cast(^s64) ptrData
            ed.setText(Format.toString("%", dref p))
        case F32: 
            p := cast(^f32) ptrData
            ed.setText(Format.toString("%", dref p))
        case F64: 
            p := cast(^f64) ptrData
            ed.setText(Format.toString("%", dref p))
        }
    }

    mtd addNative(f: Swag.TypeValue, nativeType: const *Swag.TypeInfoNative, ptrData: *u8)
    {
        item := addItem(f, ptrData)
        line := item.line
        ed := EditBox.create(line, f.name)
        ed.maxSize.x = 200
        ed.anchorStyle = .X
        item.editBox = ed

        switch nativeType.nativeKind
        {
        case U8: 
            ed.setInputU64(acast Swag.U8.Min, acast Swag.U8.Max)
        case U16: 
            ed.setInputU64(acast Swag.U16.Min, acast Swag.U16.Max)
        case U32: 
            ed.setInputU64(acast Swag.U32.Min, acast Swag.U32.Max)
        case U64, UInt: 
            ed.setInputU64(acast Swag.U64.Min, acast Swag.U64.Max)
        case S8: 
            ed.setInputS64(acast Swag.S8.Min, acast Swag.S8.Max)
        case S16: 
            ed.setInputS64(acast Swag.S16.Min, acast Swag.S16.Max)
        case S32: 
            ed.setInputS64(acast Swag.S32.Min, acast Swag.S32.Max)
        case S64, Int: 
            ed.setInputS64(acast Swag.S64.Min, acast Swag.S64.Max)
        case F32, F64: 
            ed.setInputF64()
        }

        refreshNative(item)
    }

    mtd refreshItem(using item: *PropertyItem)
    {
        if Reflection.isEnum(typeValue.pointedType)
        {
            if Reflection.hasAttribute(typeValue, Swag.EnumFlags)
            {
                refreshEnumFlags(item)
                checkBox.invalidate()
            }
            else
            {
                refreshEnum(item)
                comboBox.invalidate()
            }
        }
        elif typeValue.pointedType == Core.String
        {
            editBox.setText(dref cast(*String) ptrData)
        }
        elif Reflection.isBool(typeValue.pointedType)
        {
            checkBox.checked = dref cast(*bool) ptrData ? .Checked : .UnChecked
            checkBox.invalidate()
        }
        elif Reflection.isInteger(typeValue.pointedType) or Reflection.isFloat(typeValue.pointedType)
        {
            refreshNative(item)
            editBox.invalidate()
        }
    }

    mtd addStruct(addr: ^u8, typeStruct: const *Swag.TypeInfoStruct)
    {
        visit f: typeStruct.fields
        {       
            ptrData := addr + f.offset

            if Reflection.isEnum(f.pointedType)
            {
                typeEnum := cast(const *Swag.TypeInfoEnum) f.pointedType
                addEnum(f, typeEnum, ptrData)
            }
            elif f.pointedType == Core.String
            {
                item := addItem(f, ptrData)
                ed := EditBox.create(item.line, dref cast(*String) ptrData)
                item.editBox = ed
            }
            elif Reflection.isBool(f.pointedType)
            {
                item := addItem(f, ptrData, false)
                dispDesc := Reflection.getAttributeValue(f, Property.Description, "desc")

                ed := CheckButton.create(item.line, dispDesc)
                item.checkBox = ed
                if dref cast(*bool) ptrData
                    ed.checked = .Checked
            }
            elif Reflection.isInteger(f.pointedType) or Reflection.isFloat(f.pointedType)
            {
                nativeType := cast(const *Swag.TypeInfoNative) f.pointedType
                addNative(f, nativeType, ptrData)
            }
        }
    }
}

impl PropertyCtrl
{
    #[Swag.Discardable]
    func create(parent: *Wnd, position: Math.Rectangle = @{}, id: WndId = null)->*PropertyCtrl
    {
        ed := Wnd.create'PropertyCtrl(parent, "", position, id)

        with ed
        {
            .scrollWnd = .createView'ScrollWnd()
            .scrollWnd.dockStyle = .Center
            .scrollWnd.focusStrategy = .None
            .scrollWnd.padding = 10
            .scrollWnd.scrollWndFlags |= .TransparentBar
            .edView = .scrollWnd.createView'PropertyView()
            .edView.ed = ed
            .edView.wndFlags |= .NoScroll
            .edView.layoutKind = .Top
            .edView.spacing = 0
        }

        return ed
    }

    // Add a new struct to display
    mtd addItem(val: any)
    {
        dataof := @dataof(val)
        typeof := @kindof(val) 
        Debug.assert(typeof.kind == .Struct)
        typeStruct := cast(const *Swag.TypeInfoStruct) typeof
        addStruct(dataof, typeStruct)     

        edView.sendResizeEvent()
    }

    // Refresh displayed values
    mtd refresh()
    {       
        visit it: items
        {
            refreshItem(it)
        } 
    }

    // Show hiden content depending on the input filter
    mtd setStringFilter(filter: string)
    {        
        visit it: items
        {
            if filter == null or @countof(filter) == 0
                it.line.show()
            elif Utf8.indexOf(it.filter.toSlice(), filter, comparisonType: Utf8.ComparisonType.Latin1NoCase) != Swag.UInt.Max
                it.line.show()
            else
                it.line.hide()
        }

        applyLayout()
    }
}