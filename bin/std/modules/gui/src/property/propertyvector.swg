using Core, Pixel

func propertyVector(list: *PropertyList, typeStruct: const *Swag.TypeInfoStruct, item: *PropertyListItem, ptrData: ^u8, value: Swag.TypeValue)
{
    let line1 = Wnd.create'Wnd(item.line)
    visit v: typeStruct.fields
    {
        with let itemB = list.items.addNewPtr()
        {
            .type        = v.pointedType
            .typeValue   = v
            .ptrData     = ptrData + v.offset
            .extendFocus = 5
            .filter      = item.filter
            .line        = line1
            .forceShow   = item.line
        }

        with let label1 = Label.create(line1, v.name, {0, 0, 30, 30})
        {
            .dockStyle         = .Left
            .horzAlignmentText = .Center
        }

        with let ed = EditBox.create(line1, "")
        {
            .maxSize.x   = 70
            .anchorStyle = .X
            .dockStyle   = .Left
            .userData    = cast,unconst(*void) v.value
            .setInputF64()
            .sigLoseFocus += closure|list, itemB|(ed) { list.validateNative(itemB); }
        }

        itemB.editBox = ed
        PropertyList.refreshNative(itemB)
    }

    item.onResize += closure|line1|(item)
    {
        line1.position.height = 30
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