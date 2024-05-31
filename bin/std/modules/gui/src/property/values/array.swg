impl Properties
{
    mtd appendValue(item: *PropertiesItem, valueType: typeinfo)
    {
        discard Reflection.roomInArray(item.data, valueType)

        let insertItemIdx    = Slice.findLinear(self.items.toSlice(), item) + item.numChilds + 1
        let countItemsBefore = items.count

        var subValue: Swag.TypeValue
        let arrData   = cast(*Array'u8) item.data

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
