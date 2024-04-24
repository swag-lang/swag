using Core, Pixel

func createPopup(btn: *IconButton, w, h: f32)->*FrameWnd
{
    var pos     = btn.getSurfaceRect()
    let topView = btn.getTopView()
    var pt      = topView.surfaceToLocal({pos.x, pos.bottom()})
    pt.y += btn.getThemeMetrics().frameWnd_AnchorHeight

    var sdw = btn.getThemeMetrics().frameWnd_SmallShadowSize

    with let frame = Wnd.create'FrameWnd(topView, {pt.x, pt.y, w, h})
    {
        .setFrameForm(.Round)
        .setFrameFlags(FrameWndFlags.SmallShadow)
        .style.addStyleSheetColors("frameWnd_Border $frameWnd_PopupBorder")
        .fitPosInParent(sdw.z, sdw.w)
        .anchor    = .Top
        .anchorPos = pos.horzCenter() - frame.getSurfaceRect().x
    }

    btn.popup = frame
    return frame
}

func createColorPicker(btn: *IconButton, color: Color)->*ColorPickerCtrl
{
    let frame = createPopup(btn, 356, 382)

    let picker = ColorPickerCtrl.create(frame, {8, 54})
    picker.setColor(color)

    with let palette = PalettePicker.create(frame, {16, 16, 340, 48})
    {
        .addColor(Argb.White)
        .addColor(Argb.Gray)
        .addColor(Argb.Black)
        .addColor(Argb.Red)
        .addColor(Argb.Green)
        .addColor(Argb.Blue)
        .addColor(Argb.Yellow)
        .addColor(Argb.Purple)
        .addColor(Color.fromArgb(0))
        .boxSize = 32
        .sigChanged += closure|picker|(pal) { picker.setColor(pal.selectedColor); }
    }

    return picker
}

func propertyConstructColor(list: *PropertyList, item: *PropertyListItem, _value: Swag.TypeValue, ptrData: ^u8)
{
    let lineHeight = list.getThemeMetrics().editBox_Height
    let ptrColor = cast(*Color) ptrData
    let line1 = Wnd.create'Wnd(item.line)

    let typeInfo = Color

    with let itemB = list.items.addNewPtr()
    {
        .type        = u32
        .typeValue   = dref Reflection.getField(typeInfo, @nameof(Color.argb))
        .ptrData     = ptrData
        .extendFocus = 5
        .filter      = item.filter
        .line        = line1
        .forceShow   = item.line
    }

    with let ed = EditBox.create(line1, "", {0, 0, lineHeight * 3, lineHeight})
    {
        .dockStyle   = .Left
        .userData    = ptrData
        .setInputU64()
        .sigLoseFocus += closure|list, itemB|(ed) { list.validateNative(itemB); }
    }

    itemB.editBox = ed
    PropertyList.refreshNative(itemB)

    with let btn = IconButton.create(line1, {}, {0, 0, 70, lineHeight})
    {
        .dockStyle   = .Left
        .setForm(.RoundSquare)
        .style.addStyleSheetColors("btnIcon_Bk        $lighterMedium")
        .style.addStyleSheetColors("btnIcon_HotBk     $lighterBig")
        .style.addStyleSheetColors("btnIcon_PressedBk $lighterVeryBig")
        .iconBtnFlags.add(.Popup | .CheckableToggle)
        .arrowPos    = .RightDown
        .textPos     = .Left
        .checkedForm = .None
    }

    btn.userData = ptrData
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
