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
    name: string
}

private impl PropertyCtrl
{
    mtd addLine(f: Swag.TypeValue, canDesc = true)->*Wnd
    {
        line := StackLayoutCtrl.create(edView, .TopAutoSize)

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

        // Description
        if canDesc
        {
            dispDesc := Reflection.getAttributeValue(f, Property.Description, "desc")
            if dispDesc != null
            {
                labelDesc := Label.create(line, "", @{0, 0, position.width, 0})
                labelDesc.labelFlags |= .WordWrapAutoSize
                labelDesc.horzAlignmentText = .Justify
                labelDesc.setText(dispDesc)
                labelDesc.margin.w = 5
            }
        }

        return line
    }

    mtd addStruct(addr: ^u8, typeStruct: const *Swag.TypeInfoStruct)
    {
        visit f: typeStruct.fields
        {       
            ptrData := addr + f.offset

            if Reflection.isBool(f.pointedType)
            {
                item := items.newPtr()     
                item.name = f.name

                line := addLine(f, false)
                dispDesc := Reflection.getAttributeValue(f, Property.Description, "desc")

                ed := CheckButton.create(line, dispDesc)
                if dref cast(*bool) ptrData
                    ed.checked = .Checked
            }
            elif Reflection.isInteger(f.pointedType) or Reflection.isFloat(f.pointedType)
            {
                item := items.newPtr()     
                item.name = f.name

                line := addLine(f)
                ed := EditBox.create(line, f.name)

                nativeType := cast(const *Swag.TypeInfoNative) f.pointedType
                switch nativeType.nativeKind
                {
                case U8: 
                    p := cast(^u8) ptrData
                    ed.setText(Format.toString("%", dref p))
                    ed.setInputU64(acast Swag.U8.Min, acast Swag.U8.Max)
                case U16: 
                    p := cast(^u16) ptrData
                    ed.setText(Format.toString("%", dref p))
                    ed.setInputU64(acast Swag.U16.Min, acast Swag.U16.Max)
                case U32: 
                    p := cast(^u32) ptrData
                    ed.setText(Format.toString("%", dref p))
                    ed.setInputU64(acast Swag.U32.Min, acast Swag.U32.Max)
                case U64, UInt: 
                    p := cast(^u64) ptrData
                    ed.setText(Format.toString("%", dref p))
                    ed.setInputU64(acast Swag.U64.Min, acast Swag.U64.Max)
                case S8: 
                    p := cast(^s8) ptrData
                    ed.setText(Format.toString("%", dref p))
                    ed.setInputS64(acast Swag.S8.Min, acast Swag.S8.Max)
                case S16: 
                    p := cast(^s16) ptrData
                    ed.setText(Format.toString("%", dref p))
                    ed.setInputS64(acast Swag.S16.Min, acast Swag.S16.Max)
                case S32: 
                    p := cast(^s32) ptrData
                    ed.setText(Format.toString("%", dref p))
                    ed.setInputS64(acast Swag.S32.Min, acast Swag.S32.Max)
                case S64, Int: 
                    p := cast(^s64) ptrData
                    ed.setText(Format.toString("%", dref p))
                    ed.setInputS64(acast Swag.S64.Min, acast Swag.S64.Max)
                case F32: 
                    p := cast(^f32) ptrData
                    ed.setText(Format.toString("%", dref p))
                    ed.setInputF64()
                case F64: 
                    p := cast(^f64) ptrData
                    ed.setText(Format.toString("%", dref p))
                    ed.setInputF64()
                }
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
            .edView = .scrollWnd.createView'PropertyView()
            .edView.ed = ed
            .edView.wndFlags |= .NoScroll
            .edView.layoutKind = .Top
        }

        return ed
    }

    // 
    mtd addItem(val: any)
    {
        dataof := @dataof(val)
        typeof := @kindof(val) 
        Debug.assert(typeof.kind == .Struct)
        typeStruct := cast(const *Swag.TypeInfoStruct) typeof
        addStruct(dataof, typeStruct)
        edView.sendResizeEvent()
    }
}