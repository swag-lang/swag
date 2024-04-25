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

// To specialize the edition of a given struct.
interface IPropertyValue
{
    mtd construct(list: *PropertyList, value: Swag.TypeValue, data: ^u8)->*PropertyListItem;
}

struct PropertyList
{
    using frameWnd:            FrameWnd

    leftMargin:                s32 = 15
    rightMargin:               s32 = 15
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
    data:               *u8
    type:               typeinfo
    typeValue:          Swag.TypeValue
    forceShow:          *Wnd

    label:              *Label
    labelDesc:          *Label

    editBox:            *EditBox
    checkBox:           *CheckButton
    comboBox:           *ComboBox
    sliderBox:          *SliderCtrl

    sigResize:          SigArray'closure(*Self)
    sigRefresh:         SigArray'closure(*Self)

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
            if wnd.isOwnerOf(wnd.getApp().keybFocusWnd)
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

    func refreshBool(item: *PropertyListItem)
    {
        if item.checkBox
        {
            item.checkBox.checked = dref cast(*bool) item.data ? .Checked : .UnChecked
            item.checkBox.invalidate()
        }
    }

    func refreshItem(item: *PropertyListItem)
    {
        if !item.sigRefresh.isEmpty()
        {
            item.sigRefresh.call(item)
            return
        }

        let pointed = item.type orelse item.typeValue.pointedType
        if pointed
        {
            if Reflection.isEnum(pointed) and Reflection.hasAttribute(pointed, Swag.EnumFlags) do
                refreshEnumFlags(item)
            elif Reflection.isEnum(pointed) do
                refreshEnum(item)
            elif Reflection.isBool(pointed) do
                refreshBool(item)
            elif Reflection.isInteger(pointed) or Reflection.isFloat(pointed) do
                refreshNative(item)
            elif Reflection.isString(pointed) and item.editBox do
                item.editBox.setText(dref cast(*string) item.data)
            elif pointed == Core.String and item.editBox do
                item.editBox.setText(dref cast(*String) item.data)
        }
    }

    ////////////////////////////
    // Validate
    ////////////////////////////

    mtd validateBool(item: *PropertyListItem)
    {
        var data = cast(*bool) item.data
        if item.checkBox do
            dref data = item.checkBox.checked == .Checked ? true : false
        onChanged()
    }

    mtd validateString(item: *PropertyListItem)
    {
        var data = cast(*String) item.data
        if item.editBox do
            dref data = item.editBox.getText()
        onChanged()
    }

    mtd onChanged()
    {
        refresh()
        sigChanged.call(self)
    }

    ////////////////////////////
    // Add
    ////////////////////////////

    #[Swag.Overload]
    mtd addItem(value: Swag.TypeValue, data: *u8, canDesc = true)->*PropertyListItem
    {
        with let line = StackLayoutCtrl.create(edView, .TopAutoSize)
        {
            .margin.x = leftMargin
            .margin.z = rightMargin
            .wndFlags.set(.PreChildsPaint)
        }

        with let item = items.addNewPtr()
        {
            .line      = line
            .type      = value.pointedType
            .typeValue = value
            .data      = data
        }

        line.hookOnEvent = closure|item|(wnd, evt) { return onEvent(item, wnd, evt); }

        // Name
        var dispName: String
        let attrName  = Reflection.getAttributeValue(value, PropertyList.Name, "name")
        if attrName != null do
            dispName = cast(string) attrName
        else do
            dispName = Format.toString("<b>%<b>", value.name)

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
        let dispDesc = Reflection.getAttributeValue(value, PropertyList.Description, "desc")
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

    mtd constructString(value: Swag.TypeValue, data: *u8)
    {
        let item = addItem(value, data)
        let ed   = EditBox.create(item.line, dref cast(*String) data)
        item.editBox = ed
        ed.sigLoseFocus += closure|self, item|(ed)
        {
            self.validateString(item)
        }
    }

    mtd constructBool(value: Swag.TypeValue, data: *u8)
    {
        let item = addItem(value, data, false)

        let lineHeight = getThemeMetrics().btnPush_Height
        let line       = Wnd.create'Wnd(item.line)

        with let ed = CheckButton.create(line, "", {0, 0, lineHeight, 0})
        {
            .dockStyle = .Left
        }

        let dispDesc = Reflection.getAttributeValue(value, PropertyList.Description, "desc")
        with let label = Label.create(line, "", {lineHeight, 0, position.width - lineHeight})
        {
            .dockStyle = .Center
            .labelFlags |= .WordWrap | .AutoHeight
            .horzAlignmentText = .Justify
            .setText(dispDesc)
        }

        item.sigResize += closure|label, line, lineHeight|(item)
        {
            label.autoHeight()
            line.position.height = Math.max(lineHeight, label.position.height)
        }

        item.checkBox = ed
        ed.sigChanged += closure|self, item|(ed)
        {
            self.validateBool(item)
        }

        refreshBool(item)
    }

    mtd addStruct(addr: ^u8, typeStruct: const *Swag.TypeInfoStruct)
    {
        visit value: typeStruct.fields
        {
            let data = addr + value.offset

            // Category
            let categ = Reflection.getAttributeValue(value, PropertyList.Category, "name")
            if categ != null
            {
                var v: Swag.TypeValue
                v.name = cast(string) categ
                let item = addItem(v, null)
                item.label.style.setFont(&font0)
                categs.add({item, 0})
            }

            // Sub Category
            let subCateg = Reflection.getAttributeValue(value, PropertyList.SubCategory, "name")
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
            case Reflection.isEnum(value.pointedType):
                let typeEnum = cast(const *Swag.TypeInfoEnum) value.pointedType
                constructEnum(value, typeEnum, data)

            case value.pointedType == Core.String:
                constructString(value, data)

            case Reflection.isString(value.pointedType):
                let item = addItem(value, data)
                let ed   = EditBox.create(item.line, dref cast(*string) data)
                ed.editBoxFlags = .ReadOnly
                item.editBox    = ed

            case Reflection.isBool(value.pointedType):
                constructBool(value, data)

            case Reflection.isInteger(value.pointedType) or Reflection.isFloat(value.pointedType):
                let typeNative = cast(const *Swag.TypeInfoNative) value.pointedType
                constructNative(value, typeNative, data)

            case Reflection.isStruct(value.pointedType):
                let typeSubStruct = cast(const *Swag.TypeInfoStruct) value.pointedType
                let iprop = @mkinterface(data, value.pointedType, IPropertyValue)
                if iprop != null
                {
                    let item = iprop.construct(self, value, data)
                    item.sigRefresh.call(item)
                }
                else
                {
                    let item = addItem(value, data)
                    item.label.style.setFont(&font2)
                    addStruct(data, typeSubStruct)
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

            .edView            = .scrollWnd.createView'PropertyListView()
            .edView.ed         = ed
            .edView.layoutKind = .Top
            .edView.spacing    = 0
            .edView.wndFlags |= .NoScroll

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
