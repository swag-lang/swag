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
        let countItemsBefore = .items.count
        let itemIdx          = Slice.findLinear(.items.toSlice(), item)
        let insertItemIdx    = itemIdx + item.numChilds + 1

        // Reconstruct all items (add the end)
        var newData = arrData.buffer
        for i in arrData.count
        {
            var subValue: Swag.TypeValue
            subValue.name        = Format.toString("%", i).toString()
            subValue.pointedType = valueType
            .addValue(subValue, newData, item)
            newData += valueType.sizeof
        }

        // Move all newly reconstructed items at the right place
        let numAddedChilds = cast(u32) (.items.count - countItemsBefore)
        for i in numAddedChilds
        {
            .items.insertAt(insertItemIdx + i, .items[.items.count - numAddedChilds + i])
            .edView.childs.insertAt(insertItemIdx + i, .edView.childs[.edView.childs.count - numAddedChilds + i])
        }

        .items.count -= numAddedChilds
        .edView.childs.count -= numAddedChilds

        // Remove all old items and childs
        for i in item.numChilds
        {
            .items.deletePtr(.items[itemIdx + 1])
            .items.removeAtOrdered(itemIdx + 1)
            .edView.childs[itemIdx + 1].destroyNow()
        }

        // Update state of all newly added items
        item.numChilds = numAddedChilds
        for i in item.numChilds do
            .updateItemState(.items[itemIdx + i + 1])
    }

    mtd refreshArray(item: *PropertiesItem)
    {
        let arrData = cast(*Array'u8) item.data
        if arrData.count == cast(u64) item.userData0 do
            return
        item.userData0 = cast(*void) arrData.count

        .reconstructArray(item)
        setDescEditBox(item)
        .edView.sendResizeEvent()
    }

    mtd clearArray(item: *PropertiesItem)
    {
        let valueType = assume Reflection.getStructArrayType(item.type)
        Reflection.clearStructArray(item.data, valueType)
        .refreshArray(item)
        .hasChanged(item)
    }

    mtd removeArrayValue(item: *PropertiesItem)
    {
        let valueType     = assume Reflection.getStructArrayType(item.parent.type)
        let parentItemIdx = Slice.findLinear(.items.toSlice(), item.parent)
        let itemIdx       = Slice.findLinear(.items.toSlice(), item)
        let removeItemIdx = itemIdx - parentItemIdx - 1

        Reflection.removeValueStructArray(item.parent.data, valueType, removeItemIdx)

        let parentItem = .items[parentItemIdx]
        .refreshArray(parentItem)
        .hasChanged(parentItem)
    }

    mtd appendArrayValue(item: *PropertiesItem)
    {
        let valueType = assume Reflection.getStructArrayType(item.type)

        discard Reflection.appendValueStructArray(item.data, valueType)

        .refreshArray(item)
        .hasChanged(item)
    }

    mtd constructArray(value: Swag.TypeValue, data: *u8, parentItem: *PropertiesItem)
    {
        let item           = .addItem(value, data, parentItem)
        let numItemsBefore = .items.count
        item.flags.add(.NoSeparator)

        let valueType = assume Reflection.getStructArrayType(item.type)
        let arrData   = cast(*Array'u8) data
        var newData   = arrData.buffer

        item.userData0 = cast(*void) arrData.count

        for i in arrData.count
        {
            var subValue: Swag.TypeValue
            subValue.name        = Format.toString("%", i).toString()
            subValue.pointedType = valueType
            .addValue(subValue, newData, item)
            newData += valueType.sizeof
        }

        let parentForBtn = .kind == .Grid ? item.line : cast(*Wnd) item.label
        let addBtn       = IconButton.create(parentForBtn, .getTheme().getIcon24(ThemeIcons24.Plus, 20), {0, 0, 24, 24})
        addBtn.dockStyle = .Right
        addBtn.setForm(IconButtonForm.Round)
        addBtn.sigPressed += mtd|item|(btn)
        {
            .appendArrayValue(item)
        }

        with item.editBox = EditBox.create(item.line, "")
        {
            .dockStyle = .Left
            .setForm(.Transparent)
            .enable(false)
        }

        setDescEditBox(item)
        item.numChilds += cast(u32) (.items.count - numItemsBefore)
    }
}
