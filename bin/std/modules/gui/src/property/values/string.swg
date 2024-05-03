impl PropertyList
{
    mtd validateString(item: *PropertyListItem)
    {
        var data = cast(*String) item.data
        if item.editBox do
            dref data = item.editBox.getText()
        hasChanged(item)
    }

    mtd constructString(value: Swag.TypeValue, data: *u8)
    {
        let item = addItem(value, data)
        let ed   = EditBox.create(item.line, dref cast(*String) data)
        item.editBox = ed
        ed.sigLoseFocus += closure|self, item|(ed)
        {
            self.validateString(item)
        }
    }
}
