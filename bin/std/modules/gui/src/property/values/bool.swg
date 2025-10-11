impl Properties
{
    func refreshBool(item: *PropertiesItem)
    {
        if item.checkBox
        {
            item.checkBox.checked = dref cast(*bool) item.data ? .Checked : .UnChecked
            item.checkBox.invalidate()
        }
    }

    mtd validateBool(item: *PropertiesItem)
    {
        var data = cast(*bool) item.data
        if item.checkBox do
            dref data = item.checkBox.checked == .Checked ? true : false
        .hasChanged(item)
    }

    mtd constructBool(value: Swag.TypeValue, data: *u8, parentItem: *PropertiesItem)
    {
        let dispDesc = Reflection.getAttributeValue(value, Properties.Description, "desc")
        let item     = .addItem(value, data, parentItem, flags: AddItemFlags.Name)

        let lineHeight = .getThemeMetrics().btnPush_Height
        let line       = Wnd.create'Wnd(item.line, {0, 0, .position.width, lineHeight})
        line.dockStyle = .CenterKeepSizeVert

        if !item.label
        {
            line.margin.y = .listNameMarginBefore
            line.margin.w = .listNameMarginAfter
        }

        with let ed = CheckButton.create(line, "", {0, 0, lineHeight, 0})
        {
            .dockStyle = .Left
        }

        if .kind == .List
        {
            with let label = Label.create(line, "", {lineHeight, 0, me.position.width - lineHeight})
            {
                .dockStyle = .Center
                .labelFlags |= .WordWrap | .AutoHeight
                .horzAlignmentText = .Left
                .setText(dispDesc == null ? "" : cast(string) dispDesc)
            }

            item.sigResize += func|label, line, lineHeight|(item)
            {
                label.autoHeight()
                line.position.height = Math.max(lineHeight, label.position.height)
            }
        }

        item.checkBox = ed
        ed.sigChanged += mtd|item|(ed)
        {
            .validateBool(item)
        }

        refreshBool(item)
    }
}
