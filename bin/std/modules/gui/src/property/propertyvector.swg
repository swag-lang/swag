using Core, Pixel

func propertyVector(list: *PropertyList, typeStruct: const *Swag.TypeInfoStruct, item: *PropertyListItem, ptrData: ^u8, value: Swag.TypeValue)
{
    let line = Wnd.create'Wnd(item.line)

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

        let lineHeight = line.getThemeMetrics().editBox_Height

        with let label = Label.create(line, v.name, {0, 0, lineHeight, 0})
        {
            .dockStyle         = .Left
            .horzAlignmentText = .Center
        }

        with let ed = EditBox.create(line, "", {0, 0, lineHeight * 2.5, 0})
        {
            .anchorStyle = .X
            .dockStyle   = .Left
            .userData    = cast,unconst(*void) v.value
            .setInputF64()
            .sigLoseFocus += closure|list, itemB|(ed) { list.validateNative(itemB); }
        }

        itemB.editBox = ed
        PropertyList.refreshNative(itemB)
    }

    item.onResize += closure|line|(item)
    {
        line.position.height = line.getThemeMetrics().editBox_Height
        item.line.applyLayout()
    }
}

public impl IPropertyValue for Math.Vector2
{
    mtd impl edit(list: *PropertyList, item: *PropertyListItem, ptrData: ^u8, value: Swag.TypeValue)
    {
        propertyVector(list, Math.Vector2, item, ptrData, value)
    }
}

public impl IPropertyValue for Math.Vector3
{
    mtd impl edit(list: *PropertyList, item: *PropertyListItem, ptrData: ^u8, value: Swag.TypeValue)
    {
        propertyVector(list, Math.Vector3, item, ptrData, value)
    }
}

public impl IPropertyValue for Math.Vector4
{
    mtd impl edit(list: *PropertyList, item: *PropertyListItem, ptrData: ^u8, value: Swag.TypeValue)
    {
        propertyVector(list, Math.Vector4, item, ptrData, value)
    }
}