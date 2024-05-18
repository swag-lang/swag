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
        let item = addItem(value, data, parentItem)
        let ed   = EditBox.create(item.line, dref cast(*String) data)
        item.editBox = ed
        ed.sigLoseFocus += closure|self, item|(ed)
        {
            self.validateDynamicString(item)
        }

        item.line.position.height = getThemeMetrics().editBox_Height
    }

    mtd constructStaticString(value: Swag.TypeValue, data: *u8, parentItem: *PropertyListItem)
    {
        let item = addItem(value, data, parentItem)
        let ed   = EditBox.create(item.line, dref cast(*string) data)
        ed.editBoxFlags = .ReadOnly
        item.editBox    = ed
        item.line.position.height = getThemeMetrics().editBox_Height
    }
}
