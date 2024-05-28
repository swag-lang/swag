impl Properties
{
    mtd validateDynamicString(item: *PropertiesItem)
    {
        var data = cast(*String) item.data
        if item.editBox do
            dref data = item.editBox.getText()
        hasChanged(item)
    }

    mtd constructDynamicString(value: Swag.TypeValue, data: *u8, parentItem: *PropertiesItem)
    {
        let item = addItem(value, data, parentItem)
        with let ed = EditBox.create(item.line, dref cast(*String) data)
        {
            .maxSize.x = acast Reflection.getAttributeValue(value, EditSize, "size", sizeEditValue)
            if kind == .Grid do
                .dockStyle = .CenterKeepSizeVert
            else do
                .anchorStyle = .X
            .sigLoseFocus += closure|self, item|(ed) { self.validateDynamicString(item); }
        }

        item.editBox = ed
    }

    mtd constructStaticString(value: Swag.TypeValue, data: *u8, parentItem: *PropertiesItem)
    {
        let item = addItem(value, data, parentItem)
        with let ed = EditBox.create(item.line, dref cast(*string) data)
        {
            .maxSize.x = acast Reflection.getAttributeValue(value, EditSize, "size", sizeEditValue)
            if kind == .Grid do
                .dockStyle = .CenterKeepSizeVert
            else do
                .anchorStyle = .X
            .editBoxFlags = .ReadOnly
        }

        item.editBox = ed
    }
}
