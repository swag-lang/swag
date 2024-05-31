impl Properties
{
    mtd constructArray(value: Swag.TypeValue, data: *u8, parentItem: *PropertiesItem)
    {
        let item = addItem(value, data, parentItem)
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

        let lastItemIndex  = items.count
        let lastChildIndex = edView.childs.count

        let parentForBtn = kind == .Grid ? item.line : cast(*Wnd) item.label
        let addBtn       = IconButton.create(parentForBtn, getTheme().getIcon24(ThemeIcons24.Plus, 20), {0, 0, 24, 24})
        addBtn.dockStyle = .Right
        addBtn.setForm(IconButtonForm.Round)
        addBtn.sigPressed += closure|self, item, valueType, lastItemIndex, lastChildIndex|(btn)
        {
            discard Reflection.roomInArray(item.data, valueType)

            let childsBefore = self.edView.childs.count
            let idxMe        = Slice.findLinear(self.edView.childs.toSlice(), self)
            let itemsBefore  = self.items.count

            var subValue: Swag.TypeValue
            let arrData   = cast(*Array'u8) item.data
            subValue.name        = Format.toString("%", arrData.count - 1).toString()
            subValue.pointedType = valueType
            self.addValue(subValue, arrData.buffer + (arrData.count - 1) * valueType.sizeof, item)

            let numAddedItems = self.items.count - itemsBefore
            loop i: numAddedItems do
                self.items.insertAt(lastItemIndex + i, self.items[self.items.count - numAddedItems + i])
            self.items.count -= numAddedItems

            let numAddedChilds = self.edView.childs.count - childsBefore
            loop i: numAddedChilds do
                self.edView.childs.insertAt(lastChildIndex + i, self.edView.childs[self.edView.childs.count - numAddedChilds + i])
            self.edView.childs.count -= numAddedChilds

            loop i: numAddedItems do
                self.updateState(self.items[lastItemIndex + i])
            self.edView.sendResizeEvent()
        }

        var numDesc = Format.toString("% elements", arrData.count)
        with let desc = EditBox.create(item.line, numDesc)
        {
            .dockStyle = .Left
            .setForm(.Transparent)
            .enable(false)
        }
    }
}
