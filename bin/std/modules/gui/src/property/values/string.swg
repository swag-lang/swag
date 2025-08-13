impl Properties
{
    mtd validateDynamicString(item: *PropertiesItem)
    {
        var data = cast(*String) item.data
        if item.editBox:
            dref data = item.editBox.getText()
        hasChanged(item)
    }

    mtd constructDynamicString(value: Swag.TypedValue, data: *u8, parentItem: *PropertiesItem)
    {
        let item = addItem(value, data, parentItem)
        with let ed = EditBox.create(item.line, dref cast(*String) data)
        {
            .maxSize.x = cast() Reflection.getAttributeValue(value, EditSize, "size", sizeEditValue)
            if kind == .Grid:
                .dockStyle = .CenterKeepSizeVert
            else:
                .anchorStyle = .X
            .sigLoseFocus += mtd|item|(ed) { validateDynamicString(item); }
        }

        item.editBox = ed
    }

    mtd constructStaticString(value: Swag.TypedValue, data: *u8, parentItem: *PropertiesItem)
    {
        let item = addItem(value, data, parentItem)

        with let ed = EditBox.create(item.line, dref cast(*string) data)
        {
            .maxSize.x = cast() Reflection.getAttributeValue(value, EditSize, "size", sizeEditValue)
            if kind == .Grid:
                .dockStyle = .CenterKeepSizeVert
            else:
                .anchorStyle = .X
            .editBoxFlags = .ReadOnly
        }

        item.editBox = ed
    }
}
