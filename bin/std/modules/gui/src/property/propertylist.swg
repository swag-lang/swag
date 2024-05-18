#global public
using Core, Pixel

// To specialize the edition of a given struct.
interface IPropertyValue
{
    mtd construct(list: *PropertyList, value: Swag.TypeValue, data: ^u8, parent: *PropertyListItem)->*PropertyListItem;
}

enum PropertyListKind
{
    List    // This is a flat list
    Grid    // This is a more compact property grid
}

struct PropertyList
{
    using frameWnd:            FrameWnd

    kind                       = PropertyListKind.List

    leftMargin:                s32 = 15
    rightMargin:               s32 = 15
    nameHeight:                s32 = 25

    nameMarginBefore:          s32 = 25
    nameMarginAfter:           s32 = 5
    descMarginAfter:           s32 = 5
    enumFlagsMarginBefore:     s32 = 10
    emptyNameMargin:           s32 = 10
    sizeEditValue:             s32 = 200

    sigChanged:                SigArray'closure(*Self, *PropertyListItem)

    columnSize                 = 120
    font0:                     FontFamily
    font1:                     FontFamily
    font2:                     FontFamily
    scrollWnd:                 *ScrollWnd
    edView:                    *PropertyListView
    items:                     ArrayPtr'PropertyListItem
    categs:                    Array'{ item: *PropertyListItem, lvl: s32 }
    hotItem:                   *PropertyListItem
}

#[Swag.EnumFlags]
enum AddItemFlags
{
    Zero
    Name
    Description
}

struct PropertyListItem
{
    filter:             String
    line:               *Wnd
    data:               *u8
    type:               typeinfo
    typeValue:          Swag.TypeValue
    forceShow:          *Wnd
    parent:             *PropertyListItem

    label:              *Label
    description:        String
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
    mtd hitTest(ptLoc: Math.Point)->*PropertyListItem
    {
        var pt = ptLoc + scrollWnd.getScrollPos()
        visit item: items
        {
            if !item.line.isVisible() do
                continue
            var r = item.line.position
            r.inflate(item.extendFocus)
            if r.contains(pt) do
                return item
        }

        return null
    }

    func onHookChildEvent(wnd: *Wnd, evt: *Event)->bool
    {
        let prop = cast(*PropertyList) wnd

        switch evt.kind
        {
        case .MouseLeave:
            let mouseEvt = cast(*MouseEvent) evt
            if prop.hotItem and prop.kind == .Grid do
                ToolTip.hide(prop.hotItem.line)
            prop.hotItem = null
            wnd.invalidate()

        case .MouseMove:
            let mouseEvt = cast(*MouseEvent) evt
            var pt       = prop.surfaceToLocal(mouseEvt.surfacePos)
            let hit      = prop.hitTest(pt)
            if hit != prop.hotItem
            {
                prop.hotItem = hit
                if hit and hit.label and prop.kind == .Grid
                {
                    pt = hit.label.surfaceToLocal(mouseEvt.surfacePos)
                    if hit.label.getClientRect().contains(pt) do
                        ToolTip.show(hit.line, mouseEvt.surfacePos, hit.description)
                }

                wnd.invalidate()
            }
        }

        return false
    }

    mtd onItemEvent(item: *PropertyListItem, wndItem: *Wnd, evt: *Event)->bool
    {
        switch evt.kind
        {
        case Notify:
            let ntfyEvt = cast(*NotifyEvent) evt
            if ntfyEvt.ntfyKind == .ChildFocusChanged
            {
                wndItem.invalidate()
                return true
            }

        case PrePaint:
            let paintEvt = cast(*PaintEvent) evt
            let painter  = paintEvt.bc.painter
            let colors   = wnd.getThemeColors()

            var colBk, colBorder: Color
            let focusWnd = wndItem.getApp().keybFocusWnd
            if wndItem == focusWnd or wndItem.isOwnerOf(focusWnd)
            {
                colBk     = colors.propList_focusBk
                colBorder = colors.propList_focusBorder
            }
            elif hotItem == item
            {
                colBk     = colors.propList_hotBk
                colBorder = colors.propList_hotBorder
            }

            item.aniFocusBk.apply(wndItem, colBk)
            item.aniFocusBorder.apply(wndItem, colBorder)

            var r = wndItem.getClientRect()
            r.moveTop(wndItem.childs.front().position.y)
            r.inflate(item.extendFocus)
            painter.fillRoundRect(r, 4, 4, item.aniFocusBk.cur())
            painter.drawRoundRect(r, 4, 4, item.aniFocusBorder.cur())
        }

        return false
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

    mtd addItem(value: Swag.TypeValue, data: *u8, parentItem: *PropertyListItem, flags = AddItemFlags.Name | AddItemFlags.Description)->*PropertyListItem
    {
        var line: *StackLayoutCtrl

        if kind == .Grid do
            line = StackLayoutCtrl.create(edView, .Left, {0, 0, position.width, nameHeight})
        else do
            line = StackLayoutCtrl.create(edView, .TopAutoSize)

        with line
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
            .parent    = parentItem
        }

        line.hookOnEvent = closure|self, item|(wnd, evt) { return self.onItemEvent(item, wnd, evt); }

        // Name
        if flags.has(.Name)
        {
            var dispName: String
            let attrName  = Reflection.getAttributeValue(value, PropertyList.Name, "name")
            if attrName != null do
                dispName = Format.toString("<b>%</b>", cast(string) attrName)
            else do
                dispName = Format.toString("<b>%</b>", Utf8.beautifyName(value.name))

            with let label = Label.create(line, dispName, {0, 0, columnSize, nameHeight})
            {
                if kind == .Grid
                {
                    .vertAlignmentText = .Center
                    .horzAlignmentText = .Left
                }
                else
                {
                    .labelFlags |= .WordWrap | .AutoHeight | .DontAdaptSingleLineHeight
                    .vertAlignmentText = kind == .Grid ? .Center : .Top
                    .horzAlignmentText = .Left
                    .margin.y          = nameMarginBefore
                    .margin.w          = nameMarginAfter
                }
            }

            item.label  = label
            if parentItem
            {
                item.filter += " "
                item.filter = parentItem.filter
                item.filter += " "
            }

            item.filter += dispName
        }

        // Description
        let dispDesc = Reflection.getAttributeValue(value, PropertyList.Description, "desc")
        if dispDesc != null
        {
            item.filter += " "
            item.filter += cast(string) dispDesc
            item.description = cast(string) dispDesc

            if flags.has(.Description)
            {
                if kind != .Grid
                {
                    let labelDesc = Label.create(line, "", {0, 0, position.width, 0})
                    labelDesc.labelFlags |= .WordWrap | .AutoHeight | .DontAdaptSingleLineHeight
                    labelDesc.vertAlignmentText = .Top
                    labelDesc.horzAlignmentText = .Left
                    labelDesc.setText(dispDesc)
                    labelDesc.margin.w = descMarginAfter
                    item.labelDesc = labelDesc
                }
            }
        }

        return item
    }

    #[Swag.Overload]
    mtd addStruct(typeStruct: const *Swag.TypeInfoStruct, addr: ^u8, parentItem: *PropertyListItem)
    {
        var newParent = parentItem

        visit value: typeStruct.fields
        {
            let data = addr + value.offset

            // Sub Category
            let subCateg = Reflection.getAttributeValue(value, PropertyList.SubCategory, "name")
            if subCateg != null
            {
                var v: Swag.TypeValue
                v.name = cast(string) subCateg
                let item = addItem(v, null, parentItem)
                item.label.style.setFont(&font1)
                categs.add({item, 1})
                newParent = item
            }

            // Value
            switch
            {
            case Reflection.isEnum(value.pointedType):
                let typeEnum = cast(const *Swag.TypeInfoEnum) value.pointedType
                constructEnum(value, typeEnum, data, newParent)

            case value.pointedType == Core.String:
                constructDynamicString(value, data, newParent)

            case Reflection.isString(value.pointedType):
                constructStaticString(value, data, newParent)

            case Reflection.isBool(value.pointedType):
                constructBool(value, data, newParent)

            case Reflection.isInteger(value.pointedType) or Reflection.isFloat(value.pointedType):
                let typeNative = cast(const *Swag.TypeInfoNative) value.pointedType
                constructNative(value, typeNative, data, newParent)

            case Reflection.isStruct(value.pointedType):
                let typeSubStruct = cast(const *Swag.TypeInfoStruct) value.pointedType
                let iprop         = @mkinterface(data, value.pointedType, IPropertyValue)
                if iprop != null
                {
                    let item = iprop.construct(self, value, data, newParent)
                    Debug.assert(item != null)
                    item.sigRefresh.call(item)
                }
                else
                {
                    let item = addItem(value, data, newParent)
                    item.label.style.setFont(&font2)
                    addStruct(typeSubStruct, data, item)
                }
            }
        }
    }
}

impl PropertyList
{
    // Creates a new PropertyList instance
    #[Swag.Discardable]
    func create(parent: *Wnd, position: Math.Rectangle = {}, kind = PropertyListKind.List, id: WndId = null)->*PropertyList
    {
        let ed = Wnd.create'PropertyList(parent, "", position, id)

        with ed
        {
            .kind = kind

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

            const Font0Size = 30'u32
            const Font1Size = 24'u32
            const Font2Size = 20'u32

            let theme = parent.getTheme()
            .font0 = theme.createDefaultFontfamily(Font0Size)
            .font1 = theme.createDefaultFontfamily(Font1Size)
            .font2 = theme.createDefaultFontfamily(Font2Size)

            .hookOnChildEvent = func(wnd, evt) { return onHookChildEvent(wnd, evt); }
        }

        return ed
    }

    // Add a new struct to display.
    #[Swag.Overload]
    mtd addStruct(val: any, category: string = null)
    {
        let dataof = @dataof(val)
        let typeof = @kindof(val)
        Debug.assert(typeof.kind == .Struct)

        // Category
        var item: *PropertyListItem
        if category != null
        {
            var v: Swag.TypeValue
            v.name = category
            item   = addItem(v, null, null)
            item.label.style.setFont(&font0)
            categs.add({item, 0})
        }

        let typeStruct = cast(const *Swag.TypeInfoStruct) typeof
        addStruct(typeStruct, dataof, item)

        edView.sendResizeEvent()
    }

    // Refresh all displayed values.
    mtd refresh()
    {
        visit it: items do
            refreshItem(it)
    }

    // To be called when a value has changed.
    mtd hasChanged(item: *PropertyListItem)
    {
        refresh()
        sigChanged.call(self, item)
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
