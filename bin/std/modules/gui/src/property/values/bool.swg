impl PropertyList
{
    func refreshBool(item: *PropertyListItem)
    {
        if item.checkBox
        {
            item.checkBox.checked = dref cast(*bool) item.data ? .Checked : .UnChecked
            item.checkBox.invalidate()
        }
    }

    mtd validateBool(item: *PropertyListItem)
    {
        var data = cast(*bool) item.data
        if item.checkBox do
            dref data = item.checkBox.checked == .Checked ? true : false
        hasChanged(item)
    }

    mtd constructBool(value: Swag.TypeValue, data: *u8, parent: *PropertyListItem)
    {
        let item = addItem(value, data, parent, false)

        let lineHeight = getThemeMetrics().btnPush_Height
        let line       = Wnd.create'Wnd(item.line)

        with let ed = CheckButton.create(line, "", {0, 0, lineHeight, 0})
        {
            .dockStyle = .Left
        }

        let dispDesc = Reflection.getAttributeValue(value, PropertyList.Description, "desc")
        with let label = Label.create(line, "", {lineHeight, 0, position.width - lineHeight})
        {
            .dockStyle = .Center
            .labelFlags |= .WordWrap | .AutoHeight
            .horzAlignmentText = .Left
            .setText(dispDesc == null ? "" : cast(string) dispDesc)
        }

        item.sigResize += closure|label, line, lineHeight|(item)
        {
            label.autoHeight()
            line.position.height = Math.max(lineHeight, label.position.height)
        }

        item.checkBox = ed
        ed.sigChanged += closure|self, item|(ed)
        {
            self.validateBool(item)
        }

        refreshBool(item)
    }
}
