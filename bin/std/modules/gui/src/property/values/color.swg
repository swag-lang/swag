using Core, Pixel

impl Properties
{
    mtd constructColor(value: Swag.TypedValue, data: ^u8, parentItem: *PropertiesItem)->*PropertiesItem
    {
        let item  = me.addItem(value, data, parentItem)
        let color = cast(*Color) data
        let line  = Wnd.create'Wnd(item.line, {0, 0, me.position.width, me.getThemeMetrics().editBox_Height})
        line.dockStyle = .CenterKeepSizeVert

        let opaque    = Reflection.hasAttribute(value, me.Opaque)
        let keepAlpha = Reflection.hasAttribute(value, me.KeepAlpha)
        let editAlpha = !opaque and !keepAlpha

        let width       = cast() Reflection.getAttributeValue(value, me.EditSize, "size", me.sizeEditValue)
        const MarginBtn = 4
        const WidthBtn  = 60

        with let btn = IconButton.create(line, {}, {0, 0, WidthBtn, item.line.position.height})
        {
            .dockStyle   = .Left
            .arrowPos    = .RightDown
            .checkedForm = .None
            .setForm(.RoundSquare)
            .iconBtnFlags.add(.Popup | .CheckableToggle)
        }

        with let ed = EditBox.create(line, "", {0, 0, width - MarginBtn - WidthBtn, item.line.position.height})
        {
            .dockStyle = .Left
            .margin.z  = MarginBtn
            .setInputX64(0, opaque ? 0xFFFFFF : 0xFFFFFFFF)
            .sigLoseFocus += mtd|color, item, opaque, keepAlpha|(ed)
            {
                var result = cast(u32) (catch StrConv.parseX64(ed.text).value & 0xFFFFFFFF)
                if opaque:
                    result |= 0xFF000000
                elif keepAlpha:
                    result |= color.a << 24
                color.argb = result
                me.hasChanged(item)
            }
        }

        btn.sigPaintIcon = func|color|(btn, bc, pos, paintFlags)
        {
            bc.painter.setParams(.Antialiased)
            if !color.isOpaque():
                bc.painter.fillRoundRect(pos, 5, 5, Brush.createHatch(.CheckBoardLarge, Argb.DarkGray))
            bc.painter.fillRoundRect(pos, 5, 5, dref color)
        }

        btn.sigShowPopup += mtd|color, item, opaque, keepAlpha|(btn)
        {
            if !btn:
                return
            let frame = btn.createAnchorPopup(356, 382)
            me.editPopup = frame

            var pal: Array'Color
            for i in 9:
                pal.add(Color.getBlendKeepAlpha(Argb.White, Argb.Black, i / 8.0))

            var flags = ColorPickerFlags.All
            if opaque or keepAlpha:
                flags.remove(ColorPickerFlags.Alpha)

            let picker = ColorPickerCtrl.createWithPalette(frame, {8, 54}, pal, flags)
            picker.setColor(color)
            picker.sigChanged += mtd|color, item, opaque, keepAlpha|(pk)
            {
                var result = pk.getColor().argb
                if opaque:
                    result |= 0xFF000000
                elif keepAlpha:
                    result |= color.a << 24
                color.argb = result
                me.hasChanged(item)
            }
        }

        btn.sigHidePopup += mtd||(btn)
        {
            me.editPopup = null
        }

        item.sigRefresh += func|ed, btn, color, editAlpha|(item)
        {
            if editAlpha:
                ed.setText(Format.toString("%{X08}", color.argb))
            else:
                ed.setText(Format.toString("%{X06}", color.argb & 0xFFFFFF))
            btn.invalidate()
        }

        return item
    }
}

public impl IPropertyValue for Color
{
    mtd impl construct(list: *Properties, value: Swag.TypedValue, data: ^u8, parent: *PropertiesItem)->*PropertiesItem
    {
        return list.constructColor(value, data, parent)
    }
}
