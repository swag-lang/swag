impl Properties
{
    mtd appendValue(item: *PropertiesItem, valueType: typeinfo)
    {
        let arrData = cast(*Array'u8) item.data
        let itemIdx = Slice.findLinear(self.items.toSlice(), item)

        let oldBuffer = arrData.buffer
        discard Reflection.roomInArray(item.data, valueType)

        // If the array buffer has been reallocated, and this is not a pod, then we must reconstruct
        // the full array.
        if oldBuffer != arrData.buffer and (Reflection.hasDrop(valueType) or Reflection.hasPostCopy(valueType))
        {
            let insertItemIdx    = itemIdx + item.numChilds + 1
            let countItemsBefore = items.count

            var newData = arrData.buffer
            loop i: arrData.count
            {
                var subValue: Swag.TypeValue
                subValue.name        = Format.toString("%", i).toString()
                subValue.pointedType = valueType
                addValue(subValue, newData, item)
                newData += valueType.sizeof
            }

            let numAddedChilds = cast(u32) (items.count - countItemsBefore)
            loop i: numAddedChilds
            {
                items.insertAt(insertItemIdx + i, items[items.count - numAddedChilds + i])
                edView.childs.insertAt(insertItemIdx + i, edView.childs[edView.childs.count - numAddedChilds + i])
            }

            items.count -= numAddedChilds
            edView.childs.count -= numAddedChilds

            loop item.numChilds
            {
                items.deletePtr(items[itemIdx + 1])
                items.removeAtOrdered(itemIdx + 1)
                edView.childs[itemIdx + 1].destroyNow()
            }

            item.numChilds = numAddedChilds

            loop i: item.numChilds do
                updateState(self.items[itemIdx + i + 1])
        }

        // Do not reconstruct the full array. Append items, and move them at the correct position.
        else
        {
            let insertItemIdx    = itemIdx + item.numChilds + 1
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
        addBtn.sigPressed += closure|self, item, valueType|(btn)
        {
            self.appendValue(item, valueType)
        }

        var numDesc = Format.toString("% elements", arrData.count)
        with let desc = EditBox.create(item.line, numDesc)
        {
            .dockStyle = .Left
            .setForm(.Transparent)
            .enable(false)
        }

        item.numChilds += cast(u32) (items.count - numItemsBefore)
    }
}
