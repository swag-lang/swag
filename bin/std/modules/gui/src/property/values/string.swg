impl PropertyList
{
    mtd validateDynamicString(item: *PropertyListItem)
    {
        var data = cast(*String) item.data
        if item.editBox do
            dref data = item.editBox.getText()
        hasChanged(item)
    }

    mtd constructDynamicString(value: Swag.TypeValue, data: *u8, parentItem: *PropertyListItem)
    {
        let item = addItem(value, data, parentItem, getThemeMetrics().editBox_Height)
        with let ed = EditBox.create(item.line, dref cast(*String) data)
        {
            if kind == .Grid
            {
                .dockStyle = .CenterKeepSizeVert
            }
            else
            {
                .maxSize.x   = acast Reflection.getAttributeValue(value, EditSize, "size", sizeEditValue)
                .anchorStyle = .X
            }

            .sigLoseFocus += closure|self, item|(ed) { self.validateDynamicString(item); }
        }

        item.editBox = ed
    }

    mtd constructStaticString(value: Swag.TypeValue, data: *u8, parentItem: *PropertyListItem)
    {
        let item = addItem(value, data, parentItem, getThemeMetrics().editBox_Height)
        with let ed = EditBox.create(item.line, dref cast(*string) data)
        {
            if kind == .Grid
            {
                .dockStyle = .CenterKeepSizeVert
            }
            else
            {
                .maxSize.x   = acast Reflection.getAttributeValue(value, EditSize, "size", sizeEditValue)
                .anchorStyle = .X
            }

            .editBoxFlags = .ReadOnly
        }

        item.editBox = ed
    }
}
