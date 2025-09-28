using Core, Core.Math, Pixel

impl Properties
{
    mtd constructVector(value: Swag.TypeValue, typeStruct: const *Swag.TypeInfoStruct, data: [*] u8, parentItem: *PropertiesItem)->*PropertiesItem
    {
        let item = .addItem(value, data, parentItem)
        let line = Wnd.create'Wnd(item.line, {0, 0, .position.width, .getThemeMetrics().editBox_Height})
        line.dockStyle = .CenterKeepSizeVert

        foreach v in typeStruct.fields
        {
            let dataField = cast(*f32) (data + v.offset)

            with let label = Label.create(line, v.name[0 to 0], {0, 0, item.line.position.height})
            {
                .dockStyle         = .Left
                .horzAlignmentText = .Center
            }

            with let ed = EditBox.create(line, "", {0, 0, item.line.position.height * 2})
            {
                .dockStyle = .Left
                .setInputF64()
                .sigLoseFocus += mtd|dataField, item|(ed)
                {
                    dref dataField = catch StrConv.toF32(ed.text)
                    .hasChanged(item)
                }
            }

            item.sigRefresh += func|ed, dataField|(item)
            {
                ed.setText(Format.toString("%", dref dataField))
            }
        }

        return item
    }
}

public impl IPropertyValue for Math.Vector2
{
    mtd impl construct(list: *Properties, value: Swag.TypeValue, data: [*] u8, parent: *PropertiesItem)->*PropertiesItem
    {
        return list.constructVector(value, Math.Vector2, data, parent)
    }
}

public impl IPropertyValue for Math.Vector3
{
    mtd impl construct(list: *Properties, value: Swag.TypeValue, data: [*] u8, parent: *PropertiesItem)->*PropertiesItem
    {
        return list.constructVector(value, Math.Vector3, data, parent)
    }
}

public impl IPropertyValue for Math.Vector4
{
    mtd impl construct(list: *Properties, value: Swag.TypeValue, data: [*] u8, parent: *PropertiesItem)->*PropertiesItem
    {
        return list.constructVector(value, Math.Vector4, data, parent)
    }
}

public impl IPropertyValue for Math.Point
{
    mtd impl construct(list: *Properties, value: Swag.TypeValue, data: [*] u8, parent: *PropertiesItem)->*PropertiesItem
    {
        return list.constructVector(value, Math.Point, data, parent)
    }
}

public impl IPropertyValue for Math.Rectangle
{
    mtd impl construct(list: *Properties, value: Swag.TypeValue, data: [*] u8, parent: *PropertiesItem)->*PropertiesItem
    {
        return list.constructVector(value, Math.Rectangle, data, parent)
    }
}
