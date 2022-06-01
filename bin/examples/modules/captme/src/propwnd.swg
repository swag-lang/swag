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

    mtd getSelectedBaseForm()->*BaseForm
    {
        capture := getCapture()
        if !capture or capture.selection.count == 0
        {
            switch main.editMode
            {
            case Arrow:
                return cast(*BaseForm) &main.mdlFormArrow
            default:
                return null
            }
        }

        return capture.getSelectedBaseForm()
    }

    mtd getSelectedFormType()->typeinfo
    {
        capture := getCapture()
        if !capture or capture.selection.count == 0
        {
            switch main.editMode
            {
            case Arrow:
                return BaseForm
            default:
                return null
            }
        }

        return capture.getSelectedFormType()
    }

    mtd pushTypedValue(val: u32, type: typeinfo, name: string, first: bool)
    {
        capture := getCapture()
        if !first
            capture.undo()
        undo := capture.newUndo(.SetTypedValue)
        undo.valU32 = val
        undo.valRef = Reflection.getField(#self.type, #self.name)
        capture.pushUndo(undo)
    }

    func createColorPickerButton(stack: *Wnd)->*IconButton
    {
        btn := IconButton.create(stack, @{}, @{0, 0, 64, 88})
        btn.setForm(.RoundSquare)
        btn.style.addStyleSheetColors("btnIcon_Bk $btnIcon_HotBk")
        btn.iconBtnFlags = .Popup | .CheckableToggle
        btn.arrowPos = .Bottom
        btn.textPos = .Top
        btn.checkedForm = .None
        return btn
    }

    func createColorPicker(btn: *IconButton, color: Color)->*ColorPickerCtrl
    {
        pos     := btn.getSurfaceRect()
        topView := btn.getTopView()
        pt      := topView.surfaceToLocal(@{pos.x, pos.bottom()})
        pt.y += btn.getThemeMetrics().frameWnd_AnchorHeight

        picker  := ColorPickerCtrl.create(topView, @{pt.x, pt.y})
        picker.setFrameForm(.Round)
        picker.setFrameFlags(FrameWndFlags.SmallShadow)
        picker.style.addStyleSheetColors("frameWnd_Border $frameWnd_PopupBorder")
        sdw := btn.getThemeMetrics().frameWnd_SmallShadowSize
        picker.setColor(color)
        picker.fitPosInParent(sdw.z, sdw.w)

        picker.anchor = .Top
        picker.anchorPos = pos.horzCenter() - picker.getSurfaceRect().x

        btn.popup = picker
        return picker
    }

    mtd propBorderColor(stack: *Wnd)
    {
        btn := createColorPickerButton(stack)

        curSelType := getSelectedFormType()
        switch curSelType
        {
        case FormLine, FormArrow:
            btn.name = "Color"
        default:
            btn.name = "Outline"
        }

        me := self
        btn.sigPaintIcon = @|me|(btn, bc, pos)
        {
            bc.painter.setParams(.Antialiased)
            curSel := me.getSelectedBaseForm()
            rect := pos
            rect.x = rect.horzCenter() - 16
            rect.y = rect.vertCenter() - 16
            rect.width, rect.height = 32
            if !curSel.borderColor.isOpaque()
                bc.painter.drawRoundRect(rect, 5, 5, Pen.createHatch(.CheckBoardLarge, Argb.DarkGray, 5))
            bc.painter.drawRoundRect(rect, 5, 5, curSel.borderColor, 5)
        }

        btn.sigShowPopup += @|me|(btn) {
            curSel := me.getSelectedBaseForm()
            picker := createColorPicker(btn, curSel.borderColor)
            me.pushTypedValue(curSel.borderColor.argb, @typeof(BaseForm), @nameof(FormArrow.borderColor), true)
            picker.sigChanged += @|me, btn|(pk) {
                me.pushTypedValue(pk.getColor().argb, @typeof(BaseForm), @nameof(FormArrow.borderColor), false)
                btn.invalidate()
            }
        }
    }

    mtd propBackColor(stack: *Wnd)
    {
        curSelType := getSelectedFormType()
        if curSelType == FormLine or curSelType == FormArrow
            return

        btn := createColorPickerButton(stack)
        btn.name = "Fill"
        me := self
        btn.sigPaintIcon = @|me|(btn, bc, pos)
        {
            bc.painter.setParams(.Antialiased)
            curSel := me.getSelectedBaseForm()
            rect := pos
            rect.x = rect.horzCenter() - 21
            rect.y = rect.vertCenter() - 21
            rect.width, rect.height = 42
            if !curSel.backColor.isOpaque()
                bc.painter.fillRoundRect(rect, 10, 10, Brush.createHatch(.CheckBoardLarge, Argb.DarkGray))
            bc.painter.fillRoundRect(rect, 10, 10, curSel.backColor)
        }

        btn.sigShowPopup += @|me|(btn) {
            curSel := me.getSelectedBaseForm()
            picker := createColorPicker(btn, curSel.backColor)
            me.pushTypedValue(curSel.backColor.argb, @typeof(BaseForm), @nameof(FormArrow.backColor), true)
            picker.sigChanged += @|me, btn|(pk) {
                me.pushTypedValue(pk.getColor().argb, @typeof(BaseForm), @nameof(FormArrow.backColor), false)
                btn.invalidate()
            }
        }
    }

    mtd propStartSize()
    {
        curSelType := getSelectedFormType()
        if curSelType != FormArrow
            return
        curSel := cast(*FormArrow) getSelectedBaseForm()

        ctrl := createSlider("Start Size", 1, 50, curSel.startSize)
        ctrl.slider.sigStartChange += @|self|(slider) {
            self.pushTypedValue(bitcast(u32) slider.getValue(), @typeof(FormArrow), @nameof(FormArrow.startSize), true)
        }
        ctrl.slider.sigChanged += @|self|(slider) {
            self.pushTypedValue(bitcast(u32) slider.getValue(), @typeof(FormArrow), @nameof(FormArrow.startSize), false)
        }

        if curSel.startShape == .Flat
            ctrl.disable()
    }

    mtd propEndSize()
    {
        curSelType := getSelectedFormType()
        if curSelType != FormArrow
            return

        curSel := cast(*FormArrow) getSelectedBaseForm()
        ctrl := createSlider("End Size", 1, 50, curSel.endSize)
        ctrl.slider.sigStartChange += @|self|(slider) {
            self.pushTypedValue(bitcast(u32) slider.getValue(), @typeof(FormArrow), @nameof(FormArrow.endSize), true)
        }
        ctrl.slider.sigChanged += @|self|(slider) {
            self.pushTypedValue(bitcast(u32) slider.getValue(), @typeof(FormArrow), @nameof(FormArrow.endSize), false)
        }

        if curSel.endShape == .Flat
            ctrl.disable()
    }

    mtd propBorderSize()
    {
        curSel := getSelectedBaseForm()
        ctrl := createSlider("Tickness", 1, 50, curSel.borderSize)
        ctrl.slider.sigStartChange += @|self|(slider) {
            self.pushTypedValue(bitcast(u32) slider.getValue(), @typeof(BaseForm), @nameof(BaseForm.borderSize), true)
        }
        ctrl.slider.sigChanged += @|self|(slider) {
            self.pushTypedValue(bitcast(u32) slider.getValue(), @typeof(BaseForm), @nameof(BaseForm.borderSize), false)
        }
    }

    mtd propOpactiy()
    {
        curSel := getSelectedBaseForm()
        ctrl := createSlider("Opacity", 0, 100, acast curSel.alpha)
        ctrl.slider.sigStartChange += @|self|(slider) {
            self.pushTypedValue(cast(u32) slider.getValue(), @typeof(BaseForm), @nameof(BaseForm.alpha), true)
        }
        ctrl.slider.sigChanged += @|self|(slider) {
            self.pushTypedValue(cast(u32) slider.getValue(), @typeof(BaseForm), @nameof(BaseForm.alpha), false)
        }
    }

    mtd selectionHasChanged()
    {
        while childs.count
        {
            childs[0].destroyNow()
        }

        if getSelectedBaseForm() == null
            return

        stack := StackLayoutCtrl.create(self, .HorzCenter, @{0, 0, 0, 100})
        propBackColor(stack)
        propBorderColor(stack)

        propBorderSize()
        propStartSize()
        propEndSize()
        propOpactiy()

        layoutWnd.padding = 8
        layoutWnd.computeLayout()

        capture := getCapture()
        if capture
            capture.editView.setFocus()
    }
}