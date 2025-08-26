#global marked
impl Properties
{
    mtd validateDynamicString(item: *PropertiesItem)
    {
        var data = cast(*String) item.data
        if item.editBox:
            dref data = item.editBox.getText()
        me.hasChanged(item)
    }

    mtd constructDynamicString(value: Swag.TypedValue, data: *u8, parentItem: *PropertiesItem)
    {
        let item = me.addItem(value, data, parentItem)
        with let ed = EditBox.create(item.line, dref cast(*String) data)
        {
            .maxSize.x = cast() Reflection.getAttributeValue(value, me.EditSize, "size", me.sizeEditValue)
            if me.kind == .Grid:
                .dockStyle = .CenterKeepSizeVert
            else:
                .anchorStyle = .X
            .sigLoseFocus += mtd|item|(ed) { me.validateDynamicString(item); }
        }

        item.editBox = ed
    }

    mtd constructStaticString(value: Swag.TypedValue, data: *u8, parentItem: *PropertiesItem)
    {
        let item = me.addItem(value, data, parentItem)

        with let ed = EditBox.create(item.line, dref cast(*string) data)
        {
            .maxSize.x = cast() Reflection.getAttributeValue(value, me.EditSize, "size", me.sizeEditValue)
            if me.kind == .Grid:
                .dockStyle = .CenterKeepSizeVert
            else:
                .anchorStyle = .X
            .editBoxFlags = .ReadOnly
        }

        item.editBox = ed
    }
}
