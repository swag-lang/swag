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
    with let line1 = Wnd.create'WrapLayoutCtrl(item.line)
    {
        .wrapWidth  = lineHeight * 4
        .wrapHeight = lineHeight
    }

    with let itemB = list.items.addNewPtr()
    {
        .type        = u32
        .typeValue   = dref Reflection.getField(Color, @nameof(Color.argb))
        .ptrData     = ptrData
        .extendFocus = 5
        .filter      = item.filter
        .line        = line1
        .forceShow   = item.line
    }

    let ptrColor = cast(*Color) ptrData
    with let ed = EditBox.create(line1, "", {0, 0, lineHeight * 3, lineHeight})
    {
        .setInputX64()
        .sigLoseFocus += closure|list, ptrColor|(ed)
        {
            var res = catch StrConv.parseX64(ed.text)
            ptrColor.argb = cast(u32) res.value
            list.onChanged()
        }
    }

    itemB.onRefresh = closure|ed, ptrColor|(item)
    {
        ed.setText(Format.toString("%{X08}", ptrColor.argb))
    }

    itemB.onRefresh(itemB)

    with let btn = IconButton.create(line1, {}, {0, 0, 70, lineHeight})
    {
        .setForm(.RoundSquare)
        .style.addStyleSheetColors("btnIcon_Bk        $lighterMedium")
        .style.addStyleSheetColors("btnIcon_HotBk     $lighterBig")
        .style.addStyleSheetColors("btnIcon_PressedBk $lighterVeryBig")
        .iconBtnFlags.add(.Popup | .CheckableToggle)
        .arrowPos    = .RightDown
        .textPos     = .Left
        .checkedForm = .None
    }

    btn.userData     = ptrData
    btn.sigPaintIcon = func(btn, bc, pos)
    {
        bc.painter.setParams(.Antialiased)
        let color = dref cast(*Color) btn.userData
        if !color.isOpaque() do
            bc.painter.fillRoundRect(pos, 5, 5, Brush.createHatch(.CheckBoardLarge, Argb.DarkGray))
        bc.painter.fillRoundRect(pos, 5, 5, color)
    }

    btn.sigShowPopup += closure|list, ptrColor|(btn)
    {
        var picker = createColorPicker(btn, dref ptrColor)
        picker.sigChanged += closure|list, btn|(pk)
        {
            let color = cast(*Color) btn.userData
            dref color = pk.getColor().argb
            list.onChanged()
        }
    }

    item.onResize += closure|line1|(item)
    {
        line1.computeLayout()
        line1.position.height = line1.resultHeight
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
