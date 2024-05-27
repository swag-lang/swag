impl Properties
{
    mtd constructArray(value: Swag.TypeValue, data: *u8, parentItem: *PropertiesItem)
    {
        let item = addItem(value, data, parentItem, getThemeMetrics().editBox_Height)
        item.flags.add(.NoSeparator)

        let typeArr = cast(const *Swag.TypeInfoStruct) value.pointedType
        let valueType = typeArr.generics[0].pointedType

        var arrData = cast(*Array'u8) data
        var newData = arrData.buffer

        loop i: arrData.count
        {
            var subValue: Swag.TypeValue
            subValue.name = Format.toString("%", i).toString()
            subValue.pointedType = valueType
            addValue(subValue, newData, item)
            newData += valueType.sizeof
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
