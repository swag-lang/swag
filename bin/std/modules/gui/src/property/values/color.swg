using Core, Pixel

impl PropertyList
{
    mtd constructColor(value: Swag.TypeValue, data: ^u8, parentItem: *PropertyListItem)->*PropertyListItem
    {
        let lineHeight = getThemeMetrics().editBox_Height
        let item       = addItem(value, data, parentItem, lineHeight)
        let color      = cast(*Color) data
        let line       = Wnd.create'Wnd(item.line, {0, 0, position.width, lineHeight})
        line.dockStyle = .CenterKeepSizeVert

        let width = acast Reflection.getAttributeValue(value, EditSize, "size", sizeEditValue)
        const MarginBtn = 4
        const WidthBtn = 60

        with let btn = IconButton.create(line, {}, {0, 0, WidthBtn, lineHeight})
        {
            .dockStyle   = .Left
            .arrowPos    = .RightDown
            .checkedForm = .None
            .setForm(.RoundSquare)
            .iconBtnFlags.add(.Popup | .CheckableToggle)
        }

        with let ed = EditBox.create(line, "", {0, 0, width - MarginBtn - WidthBtn, lineHeight})
        {
            .dockStyle = .Left
            .margin.z  = MarginBtn
            .setInputX64()
            .sigLoseFocus += closure|self, color, item|(ed)
            {
                color.argb = cast(u32) catch StrConv.parseX64(ed.text).value
                self.hasChanged(item)
            }
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

            var pal: Array'Color
            loop i: 9 do
                pal.add(Color.getBlendKeepAlpha(Argb.White, Argb.Black, i / 8.0))

            var flags = ColorPickerFlags.All
            flags.remove(ColorPickerFlags.Alpha)

            let picker = ColorPickerCtrl.createWithPalette(frame, {8, 54}, pal, flags)
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
    mtd impl construct(list: *PropertyList, value: Swag.TypeValue, data: ^u8, parent: *PropertyListItem)->*PropertyListItem
    {
        return list.constructColor(value, data, parent)
    }
}
