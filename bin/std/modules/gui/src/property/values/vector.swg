using Core, Pixel

func propertyConstructVector(list: *PropertyList, item: *PropertyListItem, _value: Swag.TypeValue, typeStruct: const *Swag.TypeInfoStruct, data: ^u8)
{
    let lineHeight = list.getThemeMetrics().editBox_Height
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
            .sigLoseFocus += closure|list, dataField|(ed)
            {
                dref dataField = catch StrConv.toF32(ed.text)
                list.onChanged()
            }
        }

        item.sigRefresh += closure|ed, dataField|(item)
        {
            ed.setText(Format.toString("%", dref dataField))
        }
    }
}

public impl IPropertyValue for Math.Vector2
{
    mtd impl construct(list: *PropertyList, item: *PropertyListItem, data: ^u8, value: Swag.TypeValue)
    {
        propertyConstructVector(list, item, value, Math.Vector2, data)
    }
}

public impl IPropertyValue for Math.Vector3
{
    mtd impl construct(list: *PropertyList, item: *PropertyListItem, data: ^u8, value: Swag.TypeValue)
    {
        propertyConstructVector(list, item, value, Math.Vector3, data)
    }
}

public impl IPropertyValue for Math.Vector4
{
    mtd impl construct(list: *PropertyList, item: *PropertyListItem, data: ^u8, value: Swag.TypeValue)
    {
        propertyConstructVector(list, item, value, Math.Vector4, data)
    }
}
