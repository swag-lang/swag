impl Properties
{
    func setDescEditBox(item: *PropertiesItem)
    {
        let arrData = cast(*Array'u8) item.data
        item.editBox.setText(Format.toString("% elements", arrData.count))
    }

    mtd reconstructArray(item: *PropertiesItem)
    {
        let valueType        = assume Reflection.getStructArrayType(item.type)
        let arrData          = cast(*Array'u8) item.data
        let countItemsBefore = me.items.count
        let itemIdx          = Slice.findLinear(me.items.toSlice(), item)
        let insertItemIdx    = itemIdx + item.numChilds + 1

        // Reconstruct all items (add the end)
        var newData = arrData.buffer
        for i in arrData.count
        {
            var subValue: Swag.TypedValue
            subValue.name        = Format.toString("%", i).toString()
            subValue.pointedType = valueType
            me.addValue(subValue, newData, item)
            newData += valueType.sizeof
        }

        // Move all newly reconstructed items at the right place
        let numAddedChilds = cast(u32) (me.items.count - countItemsBefore)
        for i in numAddedChilds
        {
            me.items.insertAt(insertItemIdx + i, me.items[me.items.count - numAddedChilds + i])
            me.edView.childs.insertAt(insertItemIdx + i, me.edView.childs[me.edView.childs.count - numAddedChilds + i])
        }

        me.items.count -= numAddedChilds
        me.edView.childs.count -= numAddedChilds

        // Remove all old items and childs
        for i in item.numChilds
        {
            me.items.deletePtr(me.items[itemIdx + 1])
            me.items.removeAtOrdered(itemIdx + 1)
            me.edView.childs[itemIdx + 1].destroyNow()
        }

        // Update state of all newly added items
        item.numChilds = numAddedChilds
        for i in item.numChilds:
            me.updateItemState(me.items[itemIdx + i + 1])
    }

    mtd refreshArray(item: *PropertiesItem)
    {
        let arrData = cast(*Array'u8) item.data
        if arrData.count == cast(u64) item.userData0:
            return
        item.userData0 = cast(*void) arrData.count

        me.reconstructArray(item)
        setDescEditBox(item)
        me.edView.sendResizeEvent()
    }

    mtd clearArray(item: *PropertiesItem)
    {
        let valueType = assume Reflection.getStructArrayType(item.type)
        Reflection.clearStructArray(item.data, valueType)
        me.refreshArray(item)
        me.hasChanged(item)
    }

    mtd removeArrayValue(item: *PropertiesItem)
    {
        let valueType     = assume Reflection.getStructArrayType(item.parent.type)
        let parentItemIdx = Slice.findLinear(me.items.toSlice(), item.parent)
        let itemIdx       = Slice.findLinear(me.items.toSlice(), item)
        let removeItemIdx = itemIdx - parentItemIdx - 1

        Reflection.removeValueStructArray(item.parent.data, valueType, removeItemIdx)

        let parentItem = me.items[parentItemIdx]
        me.refreshArray(parentItem)
        me.hasChanged(parentItem)
    }

    mtd appendArrayValue(item: *PropertiesItem)
    {
        let valueType = assume Reflection.getStructArrayType(item.type)

        discard Reflection.appendValueStructArray(item.data, valueType)

        me.refreshArray(item)
        me.hasChanged(item)
    }

    mtd constructArray(value: Swag.TypedValue, data: *u8, parentItem: *PropertiesItem)
    {
        let item           = me.addItem(value, data, parentItem)
        let numItemsBefore = me.items.count
        item.flags.add(.NoSeparator)

        let valueType = assume Reflection.getStructArrayType(item.type)
        let arrData   = cast(*Array'u8) data
        var newData   = arrData.buffer

        item.userData0 = cast(*void) arrData.count

        for i in arrData.count
        {
            var subValue: Swag.TypedValue
            subValue.name        = Format.toString("%", i).toString()
            subValue.pointedType = valueType
            me.addValue(subValue, newData, item)
            newData += valueType.sizeof
        }

        let parentForBtn = me.kind == .Grid ? item.line : cast(*Wnd) item.label
        let addBtn       = IconButton.create(parentForBtn, me.getTheme().getIcon24(ThemeIcons24.Plus, 20), {0, 0, 24, 24})
        addBtn.dockStyle = .Right
        addBtn.setForm(IconButtonForm.Round)
        addBtn.sigPressed += mtd|item|(btn)
        {
            me.appendArrayValue(item)
        }

        with item.editBox = EditBox.create(item.line, "")
        {
            .dockStyle = .Left
            .setForm(.Transparent)
            .enable(false)
        }

        setDescEditBox(item)
        item.numChilds += cast(u32) (me.items.count - numItemsBefore)
    }
}
