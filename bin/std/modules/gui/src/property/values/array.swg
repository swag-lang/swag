impl Properties
{
    func setDescEditBox(item: *PropertiesItem)
    {
        let arrData = cast(*Array'u8) item.data
        item.editBox.setText(Format.toString("% elements", arrData.count))
    }

    mtd reconstructArray(item: *PropertiesItem)
    {
        let arrData          = cast(*Array'u8) item.data
        let typeArr          = cast(const *Swag.TypeInfoStruct) item.typeValue.pointedType
        let valueType        = typeArr.generics[0].pointedType
        let countItemsBefore = items.count
        let itemIdx          = Slice.findLinear(self.items.toSlice(), item)
        let insertItemIdx    = itemIdx + item.numChilds + 1

        // Reconstruct all items (add the end)
        var newData = arrData.buffer
        loop i: arrData.count
        {
            var subValue: Swag.TypeValue
            subValue.name        = Format.toString("%", i).toString()
            subValue.pointedType = valueType
            addValue(subValue, newData, item)
            newData += valueType.sizeof
        }

        // Move all newly reconstructed items at the right place
        let numAddedChilds = cast(u32) (items.count - countItemsBefore)
        loop i: numAddedChilds
        {
            items.insertAt(insertItemIdx + i, items[items.count - numAddedChilds + i])
            edView.childs.insertAt(insertItemIdx + i, edView.childs[edView.childs.count - numAddedChilds + i])
        }

        items.count -= numAddedChilds
        edView.childs.count -= numAddedChilds

        // Remove all old items and childs
        loop i: item.numChilds
        {
            items.deletePtr(items[itemIdx + 1])
            items.removeAtOrdered(itemIdx + 1)
            edView.childs[itemIdx + 1].destroyNow()
        }

        // Update state of all newly added items
        item.numChilds = numAddedChilds
        loop i: item.numChilds do
            updateState(self.items[itemIdx + i + 1])
    }

    mtd refreshArray(item: *PropertiesItem)
    {
        let arrData = cast(*Array'u8) item.data
        if arrData.count == cast(u64) item.userData0 do
            return
        item.userData0 = cast(*void) arrData.count

        reconstructArray(item)
        setDescEditBox(item)
        edView.sendResizeEvent()
    }

    mtd clearArray(item: *PropertiesItem)
    {
        let typeArr   = cast(const *Swag.TypeInfoStruct) item.typeValue.pointedType
        let valueType = typeArr.generics[0].pointedType
        Reflection.clearArray(item.data, valueType)
        refreshArray(item)
        hasChanged(item)
    }

    mtd appendArrayValue(item: *PropertiesItem)
    {
        let typeArr       = cast(const *Swag.TypeInfoStruct) item.typeValue.pointedType
        let valueType     = typeArr.generics[0].pointedType
        let arrData       = cast(*Array'u8) item.data
        let itemIdx       = Slice.findLinear(self.items.toSlice(), item)
        let insertItemIdx = itemIdx + item.numChilds + 1

        let oldBuffer = arrData.buffer
        discard Reflection.roomInArray(item.data, valueType)
        setDescEditBox(item)
        item.userData0 = cast(*void) arrData.count

        // If the array buffer has been reallocated, and this is not a pod, then we must reconstruct
        // the full array.
        if oldBuffer != arrData.buffer and (Reflection.hasDrop(valueType) or Reflection.hasPostCopy(valueType))
        {
            reconstructArray(item)
        }

        // Do not reconstruct the full array. Append items, and move them at the correct position.
        else
        {
            let countItemsBefore = items.count

            var subValue: Swag.TypeValue
            subValue.name        = Format.toString("%", arrData.count - 1).toString()
            subValue.pointedType = valueType
            addValue(subValue, arrData.buffer + (arrData.count - 1) * valueType.sizeof, item)

            let numAddedChilds = cast(u32) (items.count - countItemsBefore)
            loop i: numAddedChilds
            {
                items.insertAt(insertItemIdx + i, items[items.count - numAddedChilds + i])
                edView.childs.insertAt(insertItemIdx + i, edView.childs[edView.childs.count - numAddedChilds + i])
            }

            items.count -= numAddedChilds
            edView.childs.count -= numAddedChilds
            item.numChilds += numAddedChilds

            loop i: numAddedChilds do
                updateState(self.items[insertItemIdx + i])
        }

        edView.sendResizeEvent()
    }

    mtd constructArray(value: Swag.TypeValue, data: *u8, parentItem: *PropertiesItem)
    {
        let item           = addItem(value, data, parentItem)
        let numItemsBefore = items.count
        item.flags.add(.NoSeparator)

        let typeArr   = cast(const *Swag.TypeInfoStruct) value.pointedType
        let valueType = typeArr.generics[0].pointedType
        let arrData   = cast(*Array'u8) data
        var newData   = arrData.buffer

        item.userData0 = cast(*void) arrData.count

        loop i: arrData.count
        {
            var subValue: Swag.TypeValue
            subValue.name        = Format.toString("%", i).toString()
            subValue.pointedType = valueType
            addValue(subValue, newData, item)
            newData += valueType.sizeof
        }

        let parentForBtn = kind == .Grid ? item.line : cast(*Wnd) item.label
        let addBtn       = IconButton.create(parentForBtn, getTheme().getIcon24(ThemeIcons24.Plus, 20), {0, 0, 24, 24})
        addBtn.dockStyle = .Right
        addBtn.setForm(IconButtonForm.Round)
        addBtn.sigPressed += closure|self, item|(btn)
        {
            self.appendArrayValue(item)
            self.hasChanged(item)
        }

        with item.editBox = EditBox.create(item.line, "")
        {
            .dockStyle = .Left
            .setForm(.Transparent)
            .enable(false)
        }

        setDescEditBox(item)
        item.numChilds += cast(u32) (items.count - numItemsBefore)
    }
}
