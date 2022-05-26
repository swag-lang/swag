using Gui, Core

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

    mtd createBackColor(capture: *Capture)
    {
        curSel := capture.getSelectedBaseForm()
        icon := getTheme().getIcon64(.Question, 48)
        btn := IconButton.create(self, icon, @{0, 0, 48, 88})
        btn.name = "Fill"
        btn.iconBtnFlags = .Popup | .CheckableToggle
        btn.checkedForm = .None
        btn.marginTopIcon = 16
        btn.marginBottomText = 56

        btn.sigShowPopup += @|capture|(btn) {
            pos     := btn.getSurfaceRect()
            topView := btn.getTopView()
            pt      := topView.surfaceToLocal(@{pos.x, pos.bottom()})
            picker  := ColorPickerCtrl.create(topView, @{pt.x, pt.y})
            picker.setFrameForm(.Round)
            picker.setFrameFlags(FrameWndFlags.SmallShadow)

            curSel  := capture.getSelectedBaseForm()
            picker.setColor(curSel.backColor)

            sdw := btn.getThemeMetrics().frameWnd_SmallShadowSize
            picker.fitPosInParent(sdw.z, sdw.w)

            btn.popup = picker
        }
    }

    mtd createStartSize(capture: *Capture)
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

    mtd createEndSize(capture: *Capture)
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

    mtd createBorderSize(capture: *Capture)
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

    mtd createOpactiy(capture: *Capture)
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

        createBackColor(capture)
        createBorderSize(capture)
        createStartSize(capture)
        createEndSize(capture)
        createOpactiy(capture)

        layoutWnd.padding = 8
        layoutWnd.computeLayout()

        capture.editView.setFocus()
    }
}