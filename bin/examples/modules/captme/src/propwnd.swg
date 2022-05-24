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

    func pushTypedValue(capture: *Capture, val: u32, name: string)
    {
        undo := capture.newUndo(.SetTypedValue)
        undo.valU32 = val
        undo.valRef = Reflection.getField(@typeof(BaseForm), name)
        capture.pushUndo(undo)
    }

    mtd createBorderSize(capture: *Capture)
    {
        curSel := capture.getSelectedBaseForm()

        ctrl := createSlider("Tickness", 1, 50, curSel.borderSize)
        ctrl.slider.sigStartChange += @|capture|(slider) {
            pushTypedValue(capture, bitcast(u32) slider.getValue(), @nameof(BaseForm.borderSize))
        }
        ctrl.slider.sigChanged += @|capture|(slider) {
            capture.undo()
            pushTypedValue(capture, bitcast(u32) slider.getValue(), @nameof(BaseForm.borderSize))
        }
    }

    mtd createOpactiy(capture: *Capture)
    {
        curSel := capture.getSelectedBaseForm()

        ctrl := createSlider("Opacity", 0, 100, acast curSel.alpha)
        ctrl.slider.sigStartChange += @|capture|(slider) {
            pushTypedValue(capture, cast(u32) slider.getValue(), @nameof(BaseForm.alpha))
        }
        ctrl.slider.sigChanged += @|capture|(slider) {
            capture.undo()
            pushTypedValue(capture, cast(u32) slider.getValue(), @nameof(BaseForm.alpha))
        }
    }

    mtd selectionHasChanged()
    {
        while childs.count
            childs[0].destroyNow()

        capture := getCapture()
        if !capture or !capture.selection.count
            return

        createBorderSize(capture)
        createOpactiy(capture)

        layoutWnd.padding = 8
        layoutWnd.computeLayout()

        capture.editView.setFocus()
    }
}