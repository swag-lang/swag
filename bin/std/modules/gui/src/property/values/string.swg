impl Properties
{
    mtd validateDynamicString(item: nn *PropertiesItem)
    {
        var data = cast(*String) item.data
        if item.editBox:
            dref data = item.editBox.getText()
        hasChanged(item)
    }

    mtd constructDynamicString(value: Swag.TypeValue, data: nn *u8, parentItem: *PropertiesItem)
    {
        let item = addItem(value, data, parentItem)
        with let ed = EditBox.create(item.line, dref cast(*String) data)
        {
            .maxSize.x = cast() Reflection.getAttributeValue(value, EditSize, "size", sizeEditValue)
            if kind == .Grid:
                .dockStyle = .CenterKeepSizeVert
            else:
                .anchorStyle = .X
            .sigLoseFocus += func|self, item|(ed) { self.validateDynamicString(item); }
        }

        item.editBox = ed
    }

    mtd constructStaticString(value: Swag.TypeValue, data: nn *u8, parentItem: *PropertiesItem)
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
