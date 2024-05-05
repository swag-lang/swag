using Core, Core.Math, Pixel

impl PropertyList
{
    mtd constructVector(value: Swag.TypeValue, typeStruct: const *Swag.TypeInfoStruct, data: ^u8, parentItem: *PropertyListItem)->*PropertyListItem
    {
        let lineHeight = getThemeMetrics().editBox_Height
        let item       = addItem(value, data, parentItem)
        let line       = Wnd.create'Wnd(item.line, {0, 0, 0, lineHeight})

        visit v: typeStruct.fields
        {
            let dataField = cast(*f32) (data + v.offset)

            with let label = Label.create(line, v.name, {0, 0, lineHeight, lineHeight})
            {
                .dockStyle         = .Left
                .horzAlignmentText = .Center
            }

            with let ed = EditBox.create(line, "", {0, 0, lineHeight * 3, lineHeight})
            {
                .dockStyle = .Left
                .setInputF64()
                .sigLoseFocus += closure|self, dataField, item|(ed)
                {
                    dref dataField = catch StrConv.toF32(ed.text)
                    self.hasChanged(item)
                }
            }

            item.sigRefresh += closure|ed, dataField|(item)
            {
                ed.setText(Format.toString("%", dref dataField))
            }
        }

        return item
    }
}

public impl IPropertyValue for Math.Vector2
{
    mtd impl construct(list: *PropertyList, value: Swag.TypeValue, data: ^u8, parent: *PropertyListItem)->*PropertyListItem
    {
        return list.constructVector(value, Math.Vector2, data, parent)
    }
}

public impl IPropertyValue for Math.Vector3
{
    mtd impl construct(list: *PropertyList, value: Swag.TypeValue, data: ^u8, parent: *PropertyListItem)->*PropertyListItem
    {
        return list.constructVector(value, Math.Vector3, data, parent)
    }
}

public impl IPropertyValue for Math.Vector4
{
    mtd impl construct(list: *PropertyList, value: Swag.TypeValue, data: ^u8, parent: *PropertyListItem)->*PropertyListItem
    {
        return list.constructVector(value, Math.Vector4, data, parent)
    }
}
