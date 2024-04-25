using Core, Pixel

func propertyConstructColor(list: *PropertyList, item: *PropertyListItem, _value: Swag.TypeValue, color: *Color)
{
    let lineHeight = list.getThemeMetrics().editBox_Height
    let line       = Wnd.create'Wnd(item.line)

    with let ed = EditBox.create(line, "", {0, 0, lineHeight * 3, lineHeight})
    {
        .dockStyle = .Left
        .margin.z  = 4
        .setInputX64()
        .sigLoseFocus += closure|list, color|(ed)
        {
            color.argb = cast(u32) catch StrConv.parseX64(ed.text).value
            list.onChanged()
        }
    }

    with let btn = IconButton.create(line, {}, {0, 0, 60, lineHeight})
    {
        .dockStyle   = .Left
        .arrowPos    = .RightDown
        .checkedForm = .None
        .setForm(.RoundSquare)
        .iconBtnFlags.add(.Popup | .CheckableToggle)
    }

    btn.sigPaintIcon = closure|color|(btn, bc, pos)
    {
        bc.painter.setParams(.Antialiased)
        if !color.isOpaque() do
            bc.painter.fillRoundRect(pos, 5, 5, Brush.createHatch(.CheckBoardLarge, Argb.DarkGray))
        bc.painter.fillRoundRect(pos, 5, 5, dref color)
    }

    btn.sigShowPopup += closure|list, color|(btn)
    {
        let frame  = btn.createAnchorPopup(356, 382)
        let picker = ColorPickerCtrl.createWithPalette(frame, {8, 54})
        picker.setColor(color)
        picker.sigChanged += closure|list, color|(pk)
        {
            color.argb = pk.getColor().argb
            list.onChanged()
        }
    }

    item.sigRefresh += closure|ed, btn, color|(item)
    {
        ed.setText(Format.toString("%{X08}", color.argb))
        btn.invalidate()
    }

    item.sigResize += closure|lineHeight, line|(item)
    {
        line.position.height = lineHeight
        item.line.applyLayout()
    }
}

public impl IPropertyValue for Color
{
    mtd impl construct(list: *PropertyList, item: *PropertyListItem, data: ^u8, value: Swag.TypeValue)
    {
        propertyConstructColor(list, item, value, cast(*Color) data)
    }
}
