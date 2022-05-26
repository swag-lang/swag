using Gui, Core, Pixel

struct PropWnd
{
    using layoutWnd:    StackLayoutCtrl
    main:               *MainWnd
}

impl PropWnd
{
    mtd getCapture()->*Capture
    {
        return main.editWnd.editView.capture
    }

    mtd createSlider(name: string, minv, maxv: s64, val: f32)->*SliderCtrl
    {
        ctrl := SliderCtrl.create(self, #self.name)
        ctrl.edit.setForm(.Square)
        ctrl.setMinMaxS64(minv, maxv)
        ctrl.dockStyle = .Top
        ctrl.setValue(val)
        return ctrl
    }

    func pushTypedValue(capture: *Capture, val: u32, type: typeinfo, name: string)
    {
        undo := capture.newUndo(.SetTypedValue)
        undo.valU32 = val
        undo.valRef = Reflection.getField(type, name)
        capture.pushUndo(undo)
    }

    func createColorPickerButton(stack: *Wnd)->*IconButton
    {
        btn := IconButton.create(stack, @{}, @{0, 0, 64, 88})
        btn.setForm(.RoundSquare)
        btn.style.addStyleSheetColors("btnIcon_Bk $btnIcon_HotBk")
        btn.iconBtnFlags = .Popup | .CheckableToggle
        btn.checkedForm = .None
        btn.marginTopIcon = 16
        btn.marginBottomText = 56
        return btn
    }

    func createColorPicker(btn: *IconButton, color: Color)->*ColorPickerCtrl
    {
        pos     := btn.getSurfaceRect()
        topView := btn.getTopView()
        pt      := topView.surfaceToLocal(@{pos.x, pos.bottom()})
        picker  := ColorPickerCtrl.create(topView, @{pt.x, pt.y})
        picker.setFrameForm(.Round)
        picker.setFrameFlags(FrameWndFlags.SmallShadow)
        sdw := btn.getThemeMetrics().frameWnd_SmallShadowSize
        picker.setColor(color)
        picker.fitPosInParent(sdw.z, sdw.w)
        btn.popup = picker
        return picker
    }

    mtd propBorderColor(capture: *Capture, stack: *Wnd)
    {
        btn := createColorPickerButton(stack)

        curSelType := capture.getSelectedFormType()
        switch curSelType
        {
        case FormLine, FormArrow:
            btn.name = "Color"
        default:
            btn.name = "Outline"
        }

        btn.sigPaintIcon = @|capture|(btn, bc, pos)
        {
            bc.painter.setParams(.Antialiased)
            curSel := capture.getSelectedBaseForm()
            rect := pos
            rect.inflate(-16, -16)
            rect.height = rect.width
            if !curSel.borderColor.isOpaque()
                bc.painter.drawRoundRect(rect, 5, 5, Pen.createHatch(.CheckBoardLarge, Argb.DarkGray, 5))
            bc.painter.drawRoundRect(rect, 5, 5, curSel.borderColor, 5)
        }

        btn.sigShowPopup += @|capture|(btn) {
            curSel := capture.getSelectedBaseForm()
            picker := createColorPicker(btn, curSel.borderColor)
            pushTypedValue(capture, curSel.borderColor.argb, @typeof(BaseForm), @nameof(FormArrow.borderColor))
            picker.sigChanged += @|capture, btn|(pk) {
                capture.undo()
                pushTypedValue(capture, pk.getColor().argb, @typeof(BaseForm), @nameof(FormArrow.borderColor))
                btn.invalidate()
            }
        }
    }

    mtd propBackColor(capture: *Capture, stack: *Wnd)
    {
        curSelType := capture.getSelectedFormType()
        if curSelType == FormLine or curSelType == FormArrow
            return

        btn := createColorPickerButton(stack)
        btn.name = "Fill"
        btn.sigPaintIcon = @|capture|(btn, bc, pos)
        {
            bc.painter.setParams(.Antialiased)
            curSel := capture.getSelectedBaseForm()
            rect := pos
            rect.inflate(-12, -12)
            rect.height = rect.width
            if !curSel.backColor.isOpaque()
                bc.painter.fillRoundRect(rect, 10, 10, Brush.createHatch(.CheckBoardLarge, Argb.DarkGray))
            bc.painter.fillRoundRect(rect, 10, 10, curSel.backColor)
        }

        btn.sigShowPopup += @|capture|(btn) {
            curSel := capture.getSelectedBaseForm()
            picker := createColorPicker(btn, curSel.backColor)
            pushTypedValue(capture, curSel.backColor.argb, @typeof(BaseForm), @nameof(FormArrow.backColor))
            picker.sigChanged += @|capture, btn|(pk) {
                capture.undo()
                pushTypedValue(capture, pk.getColor().argb, @typeof(BaseForm), @nameof(FormArrow.backColor))
                btn.invalidate()
            }
        }
    }

    mtd propStartSize(capture: *Capture)
    {
        curSelType := capture.getSelectedFormType()
        if curSelType != FormArrow
            return
        curSel := cast(*FormArrow) capture.getSelectedBaseForm()

        ctrl := createSlider("Start Size", 1, 50, curSel.startSize)
        ctrl.slider.sigStartChange += @|capture|(slider) {
            pushTypedValue(capture, bitcast(u32) slider.getValue(), @typeof(FormArrow), @nameof(FormArrow.startSize))
        }
        ctrl.slider.sigChanged += @|capture|(slider) {
            capture.undo()
            pushTypedValue(capture, bitcast(u32) slider.getValue(), @typeof(FormArrow), @nameof(FormArrow.startSize))
        }

        if curSel.startShape == .Flat
            ctrl.disable()
    }

    mtd propEndSize(capture: *Capture)
    {
        curSelType := capture.getSelectedFormType()
        if curSelType != FormArrow
            return

        curSel := cast(*FormArrow) capture.getSelectedBaseForm()
        ctrl := createSlider("End Size", 1, 50, curSel.endSize)
        ctrl.slider.sigStartChange += @|capture|(slider) {
            pushTypedValue(capture, bitcast(u32) slider.getValue(), @typeof(FormArrow), @nameof(FormArrow.endSize))
        }
        ctrl.slider.sigChanged += @|capture|(slider) {
            capture.undo()
            pushTypedValue(capture, bitcast(u32) slider.getValue(), @typeof(FormArrow), @nameof(FormArrow.endSize))
        }

        if curSel.endShape == .Flat
            ctrl.disable()
    }

    mtd propBorderSize(capture: *Capture)
    {
        curSel := capture.getSelectedBaseForm()
        ctrl := createSlider("Tickness", 1, 50, curSel.borderSize)
        ctrl.slider.sigStartChange += @|capture|(slider) {
            pushTypedValue(capture, bitcast(u32) slider.getValue(), @typeof(BaseForm), @nameof(BaseForm.borderSize))
        }
        ctrl.slider.sigChanged += @|capture|(slider) {
            capture.undo()
            pushTypedValue(capture, bitcast(u32) slider.getValue(), @typeof(BaseForm), @nameof(BaseForm.borderSize))
        }
    }

    mtd propOpactiy(capture: *Capture)
    {
        curSel := capture.getSelectedBaseForm()
        ctrl := createSlider("Opacity", 0, 100, acast curSel.alpha)
        ctrl.slider.sigStartChange += @|capture|(slider) {
            pushTypedValue(capture, cast(u32) slider.getValue(), @typeof(BaseForm), @nameof(BaseForm.alpha))
        }
        ctrl.slider.sigChanged += @|capture|(slider) {
            capture.undo()
            pushTypedValue(capture, cast(u32) slider.getValue(), @typeof(BaseForm), @nameof(BaseForm.alpha))
        }
    }

    mtd selectionHasChanged()
    {
        while childs.count
            childs[0].destroyNow()

        capture := getCapture()
        if !capture or !capture.selection.count
            return

        {
            stack := StackLayoutCtrl.create(self, .HorzCenter, @{0, 0, 0, 100})
            propBackColor(capture, stack)
            propBorderColor(capture, stack)
        }

        propBorderSize(capture)
        propStartSize(capture)
        propEndSize(capture)
        propOpactiy(capture)

        layoutWnd.padding = 8
        layoutWnd.computeLayout()

        capture.editView.setFocus()
    }
}