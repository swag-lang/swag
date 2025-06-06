#global public
using Core, Pixel

// To specialize the edition of a given struct.
interface IPropertyValue
{
    mtd construct(list: *Properties, value: Swag.TypeValue, data: ^u8, parent: *PropertiesItem)->*PropertiesItem;
}

// To notify a struct about properties changes
#[Swag.Warning("Wrn0006", .Disable)]
interface IPropertyNotify
{
    mtd itemHasChanged(list: *Properties, item: *PropertiesItem) {}
    mtd isItemVisible(list: *Properties, item: *PropertiesItem)->bool { return true; }
    mtd isItemEnabled(list: *Properties, item: *PropertiesItem)->bool { return true; }
    mtd fillPopupMenu(list: *Properties, item: *PropertiesItem, menu: *MenuCtrl) {}
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

    sizeEditValue:      s32 = 200

    // For grid kind
    gridLineHeight:       f32 = 35
    gridLineMargin:       s32 = 12
    gridColumnSize:       s32 = 150
    gridMarginColumn:     s32 = 16

    // For list kind
    listNameMarginBefore:     s32 = 20
    listNameMarginAfter:      s32 = 0
    listDescMarginAfter:      s32 = 5

    sigChanged:               SigArray'func||(*Self, *PropertiesItem)

    leftMargin:               s32 = 15
    rightMargin:              s32 = 15
    fontSection:              FontFamily
    fontCategory:             FontFamily
    fontName:                 FontFamily
    scrollWnd:                *ScrollWnd
    edView:                   *PropertiesView
    items:                    ArrayPtr'PropertiesItem
    sections:                 Array'{ item: *PropertiesItem, lvl: s32 }
    hotItem:                  *PropertiesItem
    selItem:                  *PropertiesItem
    currentSectionIndex:      u32 = Swag.U32.Max
    stringFilter:             String
    cursorSet:                bool
    resizingColumn:           bool
    aniBar:                   BlendColor
    editPopup:                *Wnd
}

#[Swag.EnumFlags]
enum AddItemFlags
{
    Zero        = 0
    Name
    Description
}

#[Swag.EnumFlags]
enum ItemFlags
{
    Zero        = 0
    NoSeparator
    NoFocus
    Section
    Category
    Expanded
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
    userData0:          *void
    userData1:          *void

    label:              *Label
    description:        String
    labelDesc:          *Label

    editBox:            *EditBox
    checkBox:           *CheckButton
    comboBox:           *ComboBox
    sliderBox:          *SliderCtrl

    sigResize:          SigArray'func||(*Self)
    sigRefresh:         SigArray'func||(*Self)

    ntfyItf:            IPropertyNotify

    aniFocusBk:         BlendColor
    aniFocusBorder:     BlendColor
    level:              u32
    numChilds:          u32
    section:            u32
    extendFocus:        f32 = 10
}

impl IWnd for Properties
{
    mtd impl onPaintEvent(evt: *PaintEvent)
    {
        var colorBar = cursorSet ? getThemeColors().splitView_BarHot : getThemeColors().prop_GridColumn
        aniBar.apply(self, colorBar)
    }
}

internal impl Properties
{
    mtd getColumnX()->f32
    {
        return gridColumnSize + gridMarginColumn * 0.5
    }

    mtd hitTest(ptLoc: Math.Point)->#null *PropertiesItem
    {
        var pt = ptLoc + scrollWnd.getScrollPos()
        foreach item in items
        {
            if !item.line.isVisible():
                continue
            var r = item.line.position
            r.inflate(item.extendFocus)
            if r.contains(pt):
                return item
        }

        return null
    }

    mtd popupMenu(item: *PropertiesItem, pt: Math.Point)
    {
        ToolTip.hide(item.line)

        let menu = MenuCtrl.createPopup(self)
        var scan = item
        while scan
        {
            if scan.ntfyItf:
                scan.ntfyItf.fillPopupMenu(self, item, menu)
            scan = scan.parent
        }

        let canChange     = canChangeItem(item)
        var itemDisabled: bool

        if Reflection.isStructArray(item.type)
        {
            let arrData = cast(*Array'u8) item.data
            itemDisabled = !canChange or arrData.isEmpty()
            menu.addItem("Remove All", "ClearArray", getTheme().getIcon24(.RemoveAll), disabled: itemDisabled)
        }

        if item.parent and item.parent.type and Reflection.isStructArray(item.parent.type)
        {
            itemDisabled = !canChange
            menu.addItem("Remove", "ClearElementArray", getTheme().getIcon24(.Remove), disabled: itemDisabled)
        }

        if item.data and item.type
        {
            if menu.items.count:
                menu.addSeparator()
            menu.addItem("Copy", "Copy", getTheme().getIcon24(.Copy))
            itemDisabled = !canChange or !Clipboard.hasTypedValue(item.type)
            menu.addItem("Paste", "Paste", getTheme().getIcon24(.Paste), disabled: itemDisabled)
        }

        if !menu.items.count
        {
            menu.destroy()
            return
        }

        editPopup = menu
        let result = menu.doModal(pt)
        editPopup = null

        switch result
        {
        case "ClearArray":
            if MessageDlg.confirm(wnd.surface, "This will remove all elements") != Dialog.BtnYes:
                break
            clearArray(item)

        case "ClearElementArray":
            if MessageDlg.confirm(wnd.surface, "This will remove the element") != Dialog.BtnYes:
                break
            removeArrayValue(item)

        case "Copy":
            Clipboard.addTypedValue(item.data, item.type)

        case "Paste":
            if Clipboard.getTypedValue(item.data, item.type)
            {
                refreshItem(item)
                hasChanged(item)
            }
        }
    }

    func onHookChildEvent(wnd: *Wnd, evt: IEvent)->bool
    {
        let prop = cast(*Properties) wnd
        if prop.editPopup:
            return false

        switch evt
        {
        case FocusEvent as focusEvt where focusEvt.kind == .CanSetFocus:
            if prop.cursorSet:
                focusEvt.canChangeFocus = false

        case MouseEvent as mouseEvt where mouseEvt.kind == .MousePressed:
            var pt = prop.surfaceToLocal(mouseEvt.surfacePos)

            prop.selItem = prop.hitTest(pt)
            wnd.invalidate()

            if mouseEvt.button != .Left:
                break

            // On column bar
            if prop.cursorSet
            {
                prop.resizingColumn = true
                return true
            }

            // Toggle/Expand
            if prop.kind == .Grid and
               prop.selItem and
               prop.selItem.numChilds and
               pt.x < prop.leftMargin
            {
                prop.selItem.flags.toggle(.Expanded)
                prop.updateStates()
            }

        case MouseEvent as mouseEvt where mouseEvt.kind == .MouseDoubleClick and mouseEvt.button == .Left:

            // Toggle/Expand
            if prop.kind == .Grid and
               prop.selItem and
               prop.selItem.numChilds
            {
                prop.selItem.flags.toggle(.Expanded)
                prop.updateStates()
            }

            // Toggle checkbox
            elif prop.selItem.checkBox
            {
                prop.selItem.checkBox.toggle()
            }

        case MouseEvent as mouseEvt where mouseEvt.kind == .MouseReleased:
            prop.resizingColumn = false

        case MouseEvent as mouseEvt where mouseEvt.kind == .MouseLeave:
            if prop.hotItem and prop.kind == .Grid:
                ToolTip.hide(prop.hotItem.line)
            prop.hotItem = null

        case MouseEvent as mouseEvt where (mouseEvt.kind == .MouseMove or mouseEvt.kind == .MouseSetCursor):
            var pt = prop.surfaceToLocal(mouseEvt.surfacePos)

            // Resize of the grid column
            if prop.resizingColumn
            {
                prop.setColumnSize(cast() (pt.x - prop.gridMarginColumn * 0.5))
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
                    if hit.label.getClientRect().contains(pt):
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
                    prop.getApp().setFrameCursor(Cursor.from(.SizeWE))
                    prop.cursorSet = true
                    prop.invalidate()
                }
                elif prop.cursorSet
                {
                    prop.cursorSet = false
                    prop.invalidate()
                }
            }
        }

        return false
    }

    mtd paintBackItem(painter: *Painter, item: *PropertiesItem, wndItem: *Wnd)
    {
        var colBkBk, colBk, colBorder: Color

        let colors = wnd.getThemeColors()
        if item == selItem
        {
            colBk     = colors.prop_FocusBk
            colBorder = colors.prop_FocusBorder
        }
        elif hotItem == item and hotItem.line.isEnabled()
        {
            colBk     = colors.prop_HotBk
            colBorder = colors.prop_HotBorder
        }

        item.aniFocusBk.apply(wndItem, colBk)
        item.aniFocusBorder.apply(wndItem, colBorder)

        var r = wndItem.getClientRect()
        r.moveTop(wndItem.childs.front().position.y)
        r.inflate(item.extendFocus)
        if kind == .Grid
        {
            r.x     = 0
            r.width = edView.position.width - 1
        }
        else
        {
            r.x     = -leftMargin + 1
            r.width = edView.position.width - 1
        }

        // Background
        if kind == .Grid and item.flags.has(.Section):
            colBkBk = colors.prop_SectionBk
        elif kind == .Grid and item.flags.has(.Category):
            colBkBk = colors.prop_CategBk
        elif kind == .Grid and item.level > 1:
            colBkBk = colors.prop_Bk1
        painter.fillRect(r, colBkBk)

        // Hot and focus box background
        {
            var r1 = r
            r1.inflate(-2)
            painter.fillRoundRect(r1, 4, 4, item.aniFocusBk.cur())
        }

        // In grid mode, display borders and column separator
        if kind == .Grid
        {
            var r1 = r
            r1.height += 1
            painter.drawRect(r1, colors.prop_GridBorder)

            if !item.flags.has(.NoSeparator)
            {
                let x    = getColumnX() - item.line.position.x
                let size = cursorSet ? 4 : 1
                let rect = Math.Rectangle{x - size * 0.5, r1.y, size, r1.height}
                painter.fillRect(rect, aniBar.cur())
            }
        }

        // Hot and focus border
        {
            var r1 = r
            r1.inflate(-2)
            painter.drawRoundRect(r1, 4, 4, item.aniFocusBorder.cur())
        }

        // Expand/Collapse icon
        if kind == .Grid and item.numChilds
        {
            var expand = getTheme().getIcon24(item.flags.has(.Expanded) ? .Expanded : .Collapsed)
            expand.paint(painter, r.x + 2, r.vertCenter() - expand.sizeY * 0.5, Argb.White)
        }
    }

    mtd onItemEvent(item: *PropertiesItem, wndItem: *Wnd, evt: IEvent)->bool
    {
        if wndItem.childs.isEmpty():
            return false

        switch evt
        {
        case MouseEvent as mouseEvt where mouseEvt.kind == .MousePressed and mouseEvt.button == .Right:
            popupMenu(item, mouseEvt.surfacePos)
            return true

        case NotifyEvent as ntfyEvt where ntfyEvt.ntfyKind == .ChildFocusChanged:
            wndItem.invalidate()
            return true

        case PaintEvent as paintEvt where paintEvt.kind == .PrePaint:
            paintBackItem(paintEvt.bc.painter, item, wndItem)
        }

        return false
    }

    mtd getItemState(item: *PropertiesItem)->{ visible, enable: bool }
    {
        var result: retval{true, true}
        var scan    = item

        while scan
        {
            if scan.ntfyItf
            {
                if !scan.ntfyItf.isItemVisible(self, item):
                    result.visible = false
                if !scan.ntfyItf.isItemEnabled(self, item):
                    result.enable = false
            }

            if !result.visible and !result.enable:
                return result

            scan = scan.parent

            if scan and kind == .Grid and scan.numChilds and !scan.flags.has(.Expanded):
                result.visible = false
        }

        return result
    }

    mtd refreshItem(item: *PropertiesItem)
    {
        if !item.sigRefresh.isEmpty()
        {
            item.sigRefresh.call(item)
            return
        }

        let pointed = item.type orelse item.typeValue.pointedType
        if pointed
        {
            if Reflection.isEnum(pointed) and Reflection.hasAttribute(pointed, Swag.EnumFlags):
                refreshEnumFlags(item)
            elif Reflection.isEnum(pointed):
                refreshEnum(item)
            elif Reflection.isBool(pointed):
                refreshBool(item)
            elif Reflection.isInteger(pointed) or Reflection.isFloat(pointed):
                refreshNative(item)
            elif Reflection.isString(pointed) and item.editBox:
                item.editBox.setText(dref cast(*string) item.data)
            elif pointed == Core.String and item.editBox:
                item.editBox.setText(dref cast(*String) item.data)
            elif Reflection.isStruct(pointed)
            {
                let typeStruct = cast(const *Swag.TypeInfoStruct) pointed
                if typeStruct.fromGeneric == Array:
                    refreshArray(item)
            }
        }
    }

    mtd addItem(value: Swag.TypeValue, data: #null *u8, parentItem: #null *PropertiesItem, height: f32 = 0, flags = AddItemFlags.Name | AddItemFlags.Description)->*PropertiesItem
    {
        var line: *Wnd

        if kind == .Grid:
            line = Wnd.create'Wnd(edView, {0, 0, position.width, height orelse gridLineHeight})
        else
        {
            let stk = StackLayoutCtrl.create(edView, .Top, {0, 0, position.width, height orelse gridLineHeight})
            stk.fitToContent = true
            line = stk
        }

        with line
        {
            .margin.z = rightMargin
            .wndFlags.add(.PreChildsPaint)

            if kind == .Grid
            {
                .margin.y = gridLineMargin * 0.5
                .margin.w = gridLineMargin * 0.5
            }
            else:
                .margin.x = leftMargin
        }

        with let item = items.addNewPtr()
        {
            .line      = line
            .type      = value.pointedType
            .typeValue = value
            .data      = data
            .parent    = parentItem
            .level     = parentItem ? parentItem.level + 1 : 0
            .section   = parentItem ? parentItem.section : 0
            if kind == .Grid:
                .extendFocus = gridLineMargin * 0.5
        }

        line.hookOnEvent = func|self, item|(wnd, evt) { return self.onItemEvent(item, wnd, evt); }

        // Name
        if flags.has(.Name)
        {
            var dispName: String

            let attrName = Reflection.getAttributeValue(value, Properties.Name, "name")
            if attrName != null:
                dispName = cast(string) attrName
            else:
                dispName = Utf8.beautifyName(value.name)

            if !dispName.isEmpty()
            {
                with let label = Label.create(line, dispName, {0, 0, gridColumnSize, item.line.position.height})
                {
                    if kind == .Grid
                    {
                        .vertAlignmentText = .Center
                        .horzAlignmentText = .Left
                        .dockStyle         = .Left
                        .margin.z          = gridMarginColumn
                        .paddingText.x     = leftMargin
                    }
                    else
                    {
                        .labelFlags |= .WordWrap | .AutoHeight | .DontAdaptSingleLineHeight
                        .vertAlignmentText = kind == .Grid ? .Center : .Top
                        .horzAlignmentText = .Left
                        .margin.y          = listNameMarginBefore
                        .margin.w          = listNameMarginAfter
                    }

                    .style.setFont(fontName)
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
                    with let labelDesc = Label.create(line, "", {0, 0, position.width, 0})
                    {
                        .labelFlags |= .WordWrap | .AutoHeight | .DontAdaptSingleLineHeight
                        .vertAlignmentText = .Top
                        .horzAlignmentText = .Left
                        .margin.w          = listDescMarginAfter
                        .setText(cast(string) dispDesc)
                        item.labelDesc = labelDesc
                    }
                }
            }
        }

        return item
    }

    mtd addValue(value: Swag.TypeValue, data: ^u8, parentItem: *PropertiesItem)
    {
        // Value
        switch
        {
        case Reflection.isStaticArray(value.pointedType):
            let item           = addItem(value, data, parentItem)
            let typeArr        = cast(const *Swag.TypeInfoArray) value.pointedType
            let numItemsBefore = items.count

            var newData = data
            for i in typeArr.count
            {
                var subValue: Swag.TypeValue
                subValue.name        = Format.toString("%", i).toString()
                subValue.pointedType = typeArr.pointedType
                addValue(subValue, newData, item)
                newData += typeArr.pointedType.sizeof
            }

            item.numChilds += cast(u32) (items.count - numItemsBefore)

        case Reflection.isEnum(value.pointedType):
            let typeEnum = cast(const *Swag.TypeInfoEnum) value.pointedType
            constructEnum(value, typeEnum, data, parentItem)

        case value.pointedType == Core.String:
            constructDynamicString(value, data, parentItem)

        case Reflection.isString(value.pointedType):
            constructStaticString(value, data, parentItem)

        case Reflection.isBool(value.pointedType):
            constructBool(value, data, parentItem)

        case Reflection.isInteger(value.pointedType) or Reflection.isFloat(value.pointedType):
            let typeNative = cast(const *Swag.TypeInfoNative) value.pointedType
            constructNative(value, typeNative, data, parentItem)

        case value.pointedType == Array:
            constructArray(value, data, parentItem)

        case Reflection.isStruct(value.pointedType):
            let typeSubStruct = cast(const *Swag.TypeInfoStruct) value.pointedType
            if typeSubStruct.fromGeneric == Array
            {
                constructArray(value, data, parentItem)
                break
            }

            parentItem.flags.add(.NoSeparator | .NoFocus)
            let iprop = @mkinterface(data, value.pointedType, IPropertyValue)
            if iprop != null
            {
                let item = iprop.construct(self, value, data, parentItem)
                Debug.assert(item != null)
                item.sigRefresh.call(item)
            }
            else
            {
                let item           = addItem(value, data, parentItem)
                let numItemsBefore = items.count
                addStructInternal(typeSubStruct, data, item)
                item.numChilds = cast(u32) (items.count - numItemsBefore)
            }
        }
    }

    mtd addStructInternal(typeStruct: const *Swag.TypeInfoStruct, addr: ^u8, parentItem: *PropertiesItem)
    {
        var newParent = parentItem

        // If the struct has implemented the notify interface, store it
        if parentItem:
            parentItem.ntfyItf = @mkinterface(addr, typeStruct, IPropertyNotify)

        var categItem: *PropertiesItem
        foreach value in typeStruct.fields
        {
            // Category
            let subCateg = Reflection.getAttributeValue(value, Properties.Category, "name")
            if subCateg != null
            {
                var v:   Swag.TypeValue{name: cast(string) subCateg}
                let item = addItem(v, null, parentItem)
                item.label.style.setFont(&fontCategory)
                item.flags.add(.Category | .NoSeparator | .Expanded | .NoFocus)
                item.level = 0
                sections.add({item, 1})
                newParent = item
                categItem = item
            }

            let numItemsBefore = items.count
            addValue(value, addr + value.offset, newParent)
            if categItem:
                categItem.numChilds += cast(u32) (items.count - numItemsBefore)
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

            let theme = parent.getTheme()

            if kind == .List
            {
                const FontSectionSize  = 30'u32
                const FontCategorySize = 24'u32
                .fontName             = theme.res.fontDefault
                .fontSection          = theme.createDefaultFontfamily(FontSectionSize)
                .fontCategory         = theme.createDefaultFontfamily(FontCategorySize)
                .fontSection.regular  = .fontSection.bold
                .fontCategory.regular = .fontCategory.bold
                .fontName.regular     = .fontName.bold
                .leftMargin           = 15
                .rightMargin          = 15
            }
            else
            {
                .fontName             = theme.res.fontDefault
                .fontSection          = theme.res.fontDefault
                .fontCategory         = theme.res.fontDefault
                .fontSection.regular  = .fontSection.bold
                .fontCategory.regular = .fontCategory.bold
                .leftMargin           = 25
                .rightMargin          = 15
            }

            .hookOnChildEvent = func(wnd, evt) { return onHookChildEvent(wnd, evt); }
        }

        return ed
    }

    // Add a new struct to display.
    mtd addStruct(val: any, section: string = null)
    {
        let dataof = @dataof(val)
        let typeof = @kindof(val)
        Debug.assert(typeof.kind == .Struct)

        var item:          *PropertiesItem
        let numItemsBefore = items.count

        // Section
        if section != null
        {
            var v: Swag.TypeValue{name: section}
            item = addItem(v, null, null)
            item.flags.add(.Section | .NoSeparator | .Expanded | .NoFocus)
            item.label.style.setFont(&fontSection)
            item.section = cast(u32) sections.count
            sections.add({item, 0})
        }

        // Creates an empty parent item
        else
        {
            var v: Swag.TypeValue
            item = addItem(v, null, null, 0, .Zero)
            item.flags.add(.Section | .NoSeparator | .Expanded | .NoFocus)
        }

        let typeStruct = cast(const *Swag.TypeInfoStruct) typeof
        addStructInternal(typeStruct, dataof, item)

        item.numChilds = cast(u32) (items.count - numItemsBefore)

        updateStates()
        edView.sendResizeEvent()
    }

    // Refresh all displayed values.
    mtd refresh()
    {
        foreach it in items:
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
            if scan.ntfyItf:
                scan.ntfyItf.itemHasChanged(self, item)
            scan = scan.parent
        }

        if Reflection.hasAttribute(item.typeValue, RecalcLayout):
            updateStates()
    }

    // Show hidden content depending on the input 'filter'.
    mtd setStringFilter(filter: string)
    {
        if stringFilter == filter:
            return
        stringFilter = filter
        updateStates()
    }

    // Only display a given section.
    // If 'sectionIndex' is too big, all sections will be displayed.
    mtd setCurrentSection(sectionIndex: u32)
    {
        currentSectionIndex = sectionIndex
        updateStates()
    }

    // Returns true if the given item can be changed
    func canChangeItem(item: *PropertiesItem)->bool
    {
        if !item.data or !item.type:
            return false

        if item.type == string:
            return false

        var scan = item
        while scan
        {
            if scan.line and scan.line.isDisabled():
                return false
            scan = scan.parent
        }

        return true
    }

    // Update the state of the given item
    mtd updateItemState(item: *PropertiesItem)
    {
        var mustShow   = false
        var mustEnable = false

        if item.section == currentSectionIndex or currentSectionIndex >= sections.count
        {
            if stringFilter.length == 0:
                mustShow = true
            elif Utf8.indexOf(item.filter.toSlice(), stringFilter, comparisonType: Utf8.ComparisonType.Latin1NoCase) != Swag.U64.Max:
                mustShow = true

            if mustShow:
                (mustShow, mustEnable) = getItemState(item)
        }

        item.line.show(mustShow)
        item.line.enable(mustEnable)

        // If an item is visible, then we show also all of its parents, in order
        // to put the item in context
        if mustShow
        {
            var scan = item.parent
            while scan and !scan.line.isVisible()
            {
                scan.line.show()

                (?, mustEnable) = getItemState(scan)
                scan.line.enable(mustEnable)

                scan = scan.parent
            }
        }
    }

    // Update the show/hide state of all content
    mtd updateStates()
    {
        foreach it in items:
            updateItemState(it)
        applyLayout()
        scrollWnd.view.sendResizeEvent()
    }

    // Set the grid mode column size
    mtd setColumnSize(size: s32)
    {
        if kind != .Grid:
            return

        gridColumnSize = Math.clamp(size, gridMarginColumn, cast(s32) position.width - gridMarginColumn - leftMargin - rightMargin)
        foreach item in items
        {
            if !item.label:
                continue
            item.label.position.width = gridColumnSize
        }

        applyLayout()
        invalidate()
    }
}
