using Core, Pixel

impl PropertyList
{
    mtd constructColor(value: Swag.TypeValue, data: ^u8)->*PropertyListItem
    {
        let lineHeight = getThemeMetrics().editBox_Height
        let item       = addItem(value, data)
        let color      = cast(*Color) data
        let line       = Wnd.create'Wnd(item.line, {0, 0, 0, lineHeight})

        with let ed = EditBox.create(line, "", {0, 0, lineHeight * 3, lineHeight})
        {
            .dockStyle = .Left
            .margin.z  = 4
            .setInputX64()
            .sigLoseFocus += closure|self, color, item|(ed)
            {
                color.argb = cast(u32) catch StrConv.parseX64(ed.text).value
                self.hasChanged(item)
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

        btn.sigShowPopup += closure|self, color, item|(btn)
        {
            let frame  = btn.createAnchorPopup(356, 382)
            let picker = ColorPickerCtrl.createWithPalette(frame, {8, 54})
            picker.setColor(color)
            picker.sigChanged += closure|self, color, item|(pk)
            {
                color.argb = pk.getColor().argb
                self.hasChanged(item)
            }
        }

        item.sigRefresh += closure|ed, btn, color|(item)
        {
            ed.setText(Format.toString("%{X08}", color.argb))
            btn.invalidate()
        }

        return item
    }
}

public impl IPropertyValue for Color
{
    mtd impl construct(list: *PropertyList, value: Swag.TypeValue, data: ^u8)->*PropertyListItem
    {
        return list.constructColor(value, data)
    }
}
