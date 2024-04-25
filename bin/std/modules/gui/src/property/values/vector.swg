using Core, Pixel

func propertyConstructVector(list: *PropertyList, item: *PropertyListItem, _value: Swag.TypeValue, typeStruct: const *Swag.TypeInfoStruct, ptrData: ^u8)
{
    let lineHeight = list.getThemeMetrics().editBox_Height
    let line       = Wnd.create'Wnd(item.line)

    visit v: typeStruct.fields
    {
        with let itemB = list.items.addNewPtr()
        {
            .type        = v.pointedType
            .typeValue   = v
            .ptrData     = ptrData + v.offset
            .extendFocus = 5
            .filter      = item.filter
            .line        = line
            .forceShow   = item.line
        }

        with let label = Label.create(line, v.name, {0, 0, lineHeight, lineHeight})
        {
            .dockStyle         = .Left
            .horzAlignmentText = .Center
        }

        with let ed = EditBox.create(line, "", {0, 0, lineHeight * 3, lineHeight})
        {
            .anchorStyle = .X
            .dockStyle   = .Left
            .setInputF64()
            .sigLoseFocus += closure|list, itemB|(ed) { list.validateNative(itemB); }
        }

        itemB.editBox = ed
        PropertyList.refreshNative(itemB)
    }

    item.onResize += closure|line, lineHeight|(item)
    {
        line.position.height = lineHeight
        item.line.applyLayout()
    }
}

public impl IPropertyValue for Math.Vector2
{
    mtd impl construct(list: *PropertyList, item: *PropertyListItem, ptrData: ^u8, value: Swag.TypeValue)
    {
        propertyConstructVector(list, item, value, Math.Vector2, ptrData)
    }
}

public impl IPropertyValue for Math.Vector3
{
    mtd impl construct(list: *PropertyList, item: *PropertyListItem, ptrData: ^u8, value: Swag.TypeValue)
    {
        propertyConstructVector(list, item, value, Math.Vector3, ptrData)
    }
}

public impl IPropertyValue for Math.Vector4
{
    mtd impl construct(list: *PropertyList, item: *PropertyListItem, ptrData: ^u8, value: Swag.TypeValue)
    {
        propertyConstructVector(list, item, value, Math.Vector4, ptrData)
    }
}
