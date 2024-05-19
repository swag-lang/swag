#global public
using Core, Pixel

// To specialize the edition of a given struct.
interface IPropertyValue
{
    mtd construct(list: *Properties, value: Swag.TypeValue, data: ^u8, parent: *PropertiesItem)->*PropertiesItem;
}

// To notify a struct about properties changes
interface IPropertyNotify
{
    mtd hasChangedItem(list: *Properties, item: *PropertiesItem);
    mtd isVisibleItem(list: *Properties, item: *PropertiesItem)->bool;
}

enum PropertiesKind
{
    List     // This is a flat list
    Grid     // This is a more compact property grid
}

struct Properties
{
    using frameWnd:     FrameWnd

    kind                = PropertiesKind.List

    leftMargin:         s32 = 15
    rightMargin:        s32 = 15
    sizeEditValue:      s32 = 200

    // For grid kind
    gridLineMargin:       s32 = 12
    gridColumnSize:       s32 = 150
    gridMarginColumn:     s32 = 16

    // For list kind
    listNameHeight:            s32 = 25
    listNameMarginBefore:      s32 = 25
    listNameMarginAfter:       s32 = 5
    listDescMarginAfter:       s32 = 5

    sigChanged:                SigArray'closure(*Self, *PropertiesItem)

    fontSection:               FontFamily
    fontCategory:              FontFamily
    scrollWnd:                 *ScrollWnd
    edView:                    *PropertiesView
    items:                     ArrayPtr'PropertiesItem
    sections:                  Array'{ item: *PropertiesItem, lvl: s32 }
    hotItem:                   *PropertiesItem
    stringFilter:              String
    cursorSet:                 bool
    resizingColumn:            bool
}

#[Swag.EnumFlags]
enum AddItemFlags
{
    Zero = 0
    Name
    Description
}

#[Swag.EnumFlags]
enum ItemFlags
{
    Zero = 0
    NoSeparator
    CategoryBk
}

struct PropertiesItem
{
    filter:             String
    line:               *Wnd
    data:               *u8
    type:               typeinfo
    typeValue:          Swag.TypeValue
    parent:             *PropertiesItem
    flags:              ItemFlags

    label:              *Label
    description:        String
    labelDesc:          *Label

    editBox:            *EditBox
    checkBox:           *CheckButton
    comboBox:           *ComboBox
    sliderBox:          *SliderCtrl

    sigResize:          SigArray'closure(*Self)
    sigRefresh:         SigArray'closure(*Self)

    ntfyItf:            IPropertyNotify

    aniFocusBk:         BlendColor
    aniFocusBorder:     BlendColor
    extendFocus:        f32 = 10
}

internal impl Properties
{
    mtd getColumnX()->f32
    {
        return gridColumnSize + leftMargin + gridMarginColumn * 0.5
    }

    mtd hitTest(ptLoc: Math.Point)->*PropertiesItem
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
        let prop = cast(*Properties) wnd

        switch evt.kind
        {
        case .MousePressed:
            if prop.cursorSet
            {
                let mouseEvt = cast(*MouseEvent) evt
                if mouseEvt.button == .Left
                {
                    prop.resizingColumn = true
                    return true
                }
            }

        case .MouseReleased:
            if prop.resizingColumn
            {
                prop.resizingColumn = false
            }

        case .MouseLeave:
            if prop.hotItem and prop.kind == .Grid do
                ToolTip.hide(prop.hotItem.line)

            prop.hotItem = null
            wnd.invalidate()

            if prop.cursorSet
            {
                prop.cursorSet = false
                prop.getApp().popCursor()
            }

        case .MouseMove:
            let mouseEvt = cast(*MouseEvent) evt
            var pt       = prop.surfaceToLocal(mouseEvt.surfacePos)

            // Resize of the grid column
            if prop.resizingColumn
            {
                prop.setColumnSize(acast (pt.x - prop.leftMargin - prop.gridMarginColumn * 0.5))
                return true
            }

            // Hot item
            let hit = prop.hitTest(pt)
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

            // On the column size bar
            if prop.kind == .Grid
            {
                let x = prop.getColumnX()
                if pt.x >= x - 3 and pt.x <= x + 3 and hit and !hit.flags.has(.NoSeparator)
                {
                    if !prop.cursorSet do
                        prop.getApp().pushCursor(Cursor.from(.SizeWE))
                    prop.cursorSet = true
                }
                elif prop.cursorSet
                {
                    prop.cursorSet = false
                    prop.getApp().popCursor()
                }
            }
        }

        return false
    }

    mtd onItemEvent(item: *PropertiesItem, wndItem: *Wnd, evt: *Event)->bool
    {
        if wndItem.childs.isEmpty() do
            return false

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
                colBk     = colors.propList_FocusBk
                colBorder = colors.propList_FocusBorder
            }
            elif hotItem == item
            {
                colBk     = colors.propList_HotBk
                colBorder = colors.propList_HotBorder
            }

            item.aniFocusBk.apply(wndItem, colBk)
            item.aniFocusBorder.apply(wndItem, colBorder)

            var r = wndItem.getClientRect()
            r.moveTop(wndItem.childs.front().position.y)
            r.inflate(item.extendFocus)
            r.x     = -leftMargin + 1
            r.width = edView.position.width - 1

            if kind == .Grid
            {
                var r1 = r
                r1.height += 1

                if !item.flags.has(.NoSeparator)
                {
                    let x = cast(f32) gridColumnSize + leftMargin - gridMarginColumn * 0.5
                    painter.drawLine(x, r1.y, x, r1.bottom(), getThemeColors().propList_GridBorder, 1)
                }

                if item.flags.has(.CategoryBk)
                {
                    painter.fillRect(r1, colors.propList_CategBk)
                }

                painter.drawRect(r1, colors.propList_GridBorder)
            }

            painter.fillRoundRect(r, 4, 4, item.aniFocusBk.cur())
            painter.drawRoundRect(r, 4, 4, item.aniFocusBorder.cur())
        }

        return false
    }

    mtd askVisibleItem(item: *PropertiesItem)->bool
    {
        var scan = item
        while scan
        {
            if scan.ntfyItf
            {
                if !scan.ntfyItf.isVisibleItem(self, item) do
                    return false
            }

            scan = scan.parent
        }

        return true
    }

    func refreshItem(item: *PropertiesItem)
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

    mtd addItem(value: Swag.TypeValue, data: *u8, parentItem: *PropertiesItem, height: f32, flags = AddItemFlags.Name | AddItemFlags.Description)->*PropertiesItem
    {
        var line: *Wnd

        if kind == .Grid do
            line = Wnd.create'Wnd(edView, {0, 0, position.width, height})
        else do
            line = StackLayoutCtrl.create(edView, .TopAutoSize)

        with line
        {
            .margin.x = leftMargin
            .margin.z = rightMargin
            .wndFlags.set(.PreChildsPaint)

            if kind == .Grid
            {
                .margin.y = gridLineMargin * 0.5
                .margin.w = gridLineMargin * 0.5
            }
        }

        with let item = items.addNewPtr()
        {
            .line      = line
            .type      = value.pointedType
            .typeValue = value
            .data      = data
            .parent    = parentItem
            if kind == .Grid do
                .extendFocus = gridLineMargin * 0.5
        }

        line.hookOnEvent = closure|self, item|(wnd, evt) { return self.onItemEvent(item, wnd, evt); }

        // Name
        if flags.has(.Name)
        {
            var dispNameContent: String

            let attrName  = Reflection.getAttributeValue(value, Properties.Name, "name")
            if attrName != null do
                dispNameContent = cast(string) attrName
            else do
                dispNameContent = Utf8.beautifyName(value.name)

            // Name is bold in list mode, and regular in grid mode
            var dispName: String
            if kind == .Grid do
                dispName = dispNameContent
            else do
                dispName = Format.toString("<b>%</b>", dispNameContent)

            with let label = Label.create(line, dispName, {0, 0, gridColumnSize, listNameHeight})
            {
                if kind == .Grid
                {
                    .vertAlignmentText = .Center
                    .horzAlignmentText = .Left
                    .dockStyle         = .Left
                    .margin.z          = gridMarginColumn
                }
                else
                {
                    .labelFlags |= .WordWrap | .AutoHeight | .DontAdaptSingleLineHeight
                    .vertAlignmentText = kind == .Grid ? .Center : .Top
                    .horzAlignmentText = .Left
                    .margin.y          = listNameMarginBefore
                    .margin.w          = listNameMarginAfter
                }
            }

            item.label = label
            if parentItem
            {
                item.filter += " "
                item.filter = parentItem.filter
                item.filter += " "
            }

            item.filter += dispName
        }

        // Description
        let dispDesc = Reflection.getAttributeValue(value, Properties.Description, "desc")
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
                    labelDesc.margin.w = listDescMarginAfter
                    item.labelDesc     = labelDesc
                }
            }
        }

        return item
    }

    #[Swag.Overload]
    mtd addStruct(typeStruct: const *Swag.TypeInfoStruct, addr: ^u8, parentItem: *PropertiesItem)
    {
        var newParent = parentItem

        // If the struct has implemented the notify interface, store it
        if parentItem do
            parentItem.ntfyItf = @mkinterface(addr, typeStruct, IPropertyNotify)

        visit value: typeStruct.fields
        {
            let data = addr + value.offset

            // Category
            let subCateg = Reflection.getAttributeValue(value, Properties.Category, "name")
            if subCateg != null
            {
                var v: Swag.TypeValue{name: cast(string) subCateg}
                let item = addItem(v, null, parentItem, fontCategory.regular.height)
                item.label.style.setFont(&fontCategory)
                item.flags.add(.CategoryBk)
                sections.add({item, 1})
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
                    let item = addItem(value, data, newParent, getThemeMetrics().editBox_Height)
                    addStruct(typeSubStruct, data, item)
                }
            }
        }
    }
}

impl Properties
{
    // Creates a new Properties instance
    #[Swag.Discardable]
    func create(parent: *Wnd, position: Math.Rectangle = {}, kind = PropertiesKind.List, id: WndId = null)->*Properties
    {
        let ed = Wnd.create'Properties(parent, "", position, id)

        with ed
        {
            .kind = kind

            .scrollWnd               = .createView'ScrollWnd()
            .scrollWnd.dockStyle     = .Center
            .scrollWnd.focusStrategy = .None
            .scrollWnd.padding       = 0
            .scrollWnd.scrollWndFlags |= .TransparentBar

            .edView            = .scrollWnd.createView'PropertiesView()
            .edView.ed         = ed
            .edView.layoutKind = .Top
            .edView.spacing    = 0
            .edView.wndFlags |= .NoScroll

            const FontSectionSize = 30'u32
            const FontCategorySize = 24'u32

            let theme = parent.getTheme()
            .fontSection = theme.createDefaultFontfamily(FontSectionSize)
            .fontCategory = theme.createDefaultFontfamily(FontCategorySize)

            .hookOnChildEvent = func(wnd, evt) { return onHookChildEvent(wnd, evt); }
        }

        return ed
    }

    // Add a new struct to display.
    #[Swag.Overload]
    mtd addStruct(val: any, section: string = null)
    {
        let dataof = @dataof(val)
        let typeof = @kindof(val)
        Debug.assert(typeof.kind == .Struct)

        // Section
        var item: *PropertiesItem
        if section != null
        {
            var v: Swag.TypeValue{name: section}
            item = addItem(v, null, null, fontSection.regular.height)
            item.flags.add(.CategoryBk)
            item.label.style.setFont(&fontSection)
            sections.add({item, 0})
        }
        else
        {
            var v: Swag.TypeValue
            item = addItem(v, null, null, 0, .Zero)
        }

        let typeStruct = cast(const *Swag.TypeInfoStruct) typeof
        addStruct(typeStruct, dataof, item)

        updateVisibleItems()
        edView.sendResizeEvent()
    }

    // Refresh all displayed values.
    mtd refresh()
    {
        visit it: items do
            refreshItem(it)
    }

    // To be called when a value has changed.
    mtd hasChanged(item: *PropertiesItem)
    {
        refresh()
        sigChanged.call(self, item)

        // Notify all the hierarchy
        var scan = item
        while scan
        {
            if scan.ntfyItf do
                scan.ntfyItf.hasChangedItem(self, item)
            scan = scan.parent
        }

        if Reflection.hasAttribute(item.typeValue, RecalcLayout) do
            updateVisibleItems()
    }

    // Show hidden content depending on the input 'filter'.
    mtd setStringFilter(filter: string)
    {
        if stringFilter == filter do
            return
        stringFilter = filter
        updateVisibleItems()
    }

    // Update the show/hide state of all content
    mtd updateVisibleItems()
    {
        visit it: items
        {
            var mustShow = false
            if stringFilter.length == 0 do
                mustShow = true
            elif Utf8.indexOf(it.filter.toSlice(), stringFilter, comparisonType: Utf8.ComparisonType.Latin1NoCase) != Swag.U64.Max do
                mustShow = true

            if mustShow do
                mustShow = askVisibleItem(it)

            it.line.show(mustShow)

            // If an item is visible, then we show also all of its parents, in order
            // to put the item in context
            if mustShow
            {
                var scan = it.parent
                while scan and !scan.line.isVisible()
                {
                    scan.line.show()
                    scan = scan.parent
                }
            }
        }

        applyLayout()
        scrollWnd.view.sendResizeEvent()
    }

    // Set the grid mode column size
    mtd setColumnSize(size: s32)
    {
        if kind != .Grid do
            return

        gridColumnSize = Math.clamp(size, gridMarginColumn, cast(s32) position.width - gridMarginColumn - leftMargin - rightMargin)
        visit item: items
        {
            if !item.label do
                continue
            item.label.position.width = gridColumnSize
        }

        applyLayout()
        invalidate()
    }
}
