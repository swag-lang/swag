using Core, Pixel

func createColorPicker(btn: *IconButton, color: Color)->*ColorPickerCtrl
{
    let frame  = btn.createAnchorPopup(356, 382)
    let picker = ColorPickerCtrl.createWithPalette(frame, {8, 54})
    picker.setColor(color)
    return picker
}

func propertyConstructColor(list: *PropertyList, item: *PropertyListItem, _value: Swag.TypeValue, ptrData: ^u8)
{
    let lineHeight = list.getThemeMetrics().editBox_Height
    let line1      = Wnd.create'Wnd(item.line)

    let ptrColor = cast(*Color) ptrData
    with let ed = EditBox.create(line1, "", {0, 0, lineHeight * 3, lineHeight})
    {
        .dockStyle = .Left
        .margin.z  = 4
        .setInputX64()
        .sigLoseFocus += closure|list, ptrColor|(ed)
        {
            ptrColor.argb = cast(u32) catch StrConv.parseX64(ed.text).value
            list.onChanged()
        }
    }

    item.onRefresh = closure|ed, ptrColor|(item)
    {
        ed.setText(Format.toString("%{X08}", ptrColor.argb))
    }

    item.onRefresh(item)

    with let btn = IconButton.create(line1, {}, {0, 0, 60, lineHeight})
    {
        .dockStyle   = .Left
        .arrowPos    = .RightDown
        .checkedForm = .None
        .setForm(.RoundSquare)
        .iconBtnFlags.add(.Popup | .CheckableToggle)
    }

    btn.sigPaintIcon = closure|ptrColor|(btn, bc, pos)
    {
        bc.painter.setParams(.Antialiased)
        if !ptrColor.isOpaque() do
            bc.painter.fillRoundRect(pos, 5, 5, Brush.createHatch(.CheckBoardLarge, Argb.DarkGray))
        bc.painter.fillRoundRect(pos, 5, 5, dref ptrColor)
    }

    btn.sigShowPopup += closure|list, ptrColor|(btn)
    {
        var picker = createColorPicker(btn, dref ptrColor)
        picker.sigChanged += closure|list, ptrColor|(pk)
        {
            ptrColor.argb = pk.getColor().argb
            list.onChanged()
        }
    }

    item.onResize += closure|lineHeight, line1|(item)
    {
        line1.position.height = lineHeight
        item.line.applyLayout()
    }
}

public impl IPropertyValue for Color
{
    mtd impl construct(list: *PropertyList, item: *PropertyListItem, ptrData: ^u8, value: Swag.TypeValue)
    {
        propertyConstructColor(list, item, value, ptrData)
    }
}
