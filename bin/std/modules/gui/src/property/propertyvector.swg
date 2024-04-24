using Core, Pixel

func propertyConstructVector(list: *PropertyList, item: *PropertyListItem, _value: Swag.TypeValue, typeStruct: const *Swag.TypeInfoStruct, ptrData: ^u8)
{
    let lineHeight = list.getThemeMetrics().editBox_Height
    with let line1 = Wnd.create'WrapLayoutCtrl(item.line)
    {
        .wrapWidth  = lineHeight * 4
        .wrapHeight = lineHeight
    }

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

        let one = Wnd.create'Wnd(line1)

        with let label = Label.create(one, v.name, {0, 0, lineHeight, lineHeight})
        {
            .dockStyle         = .Left
            .horzAlignmentText = .Center
        }

        with let ed = EditBox.create(one, "", {0, 0, lineHeight * 3, lineHeight})
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

    item.onResize += closure|line1|(item)
    {
        line1.computeLayout()
        line1.position.height = line1.resultHeight
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
