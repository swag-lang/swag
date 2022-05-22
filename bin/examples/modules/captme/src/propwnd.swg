using Gui

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

    mtd createBorderSize(capture: *Capture)
    {
        curSel := capture.getSelectedBaseForm()

        ctrl := SliderCtrl.create(self, "Tickness")
        ctrl.edit.setForm(.Square)
        ctrl.setMinMaxS64(1, 50)
        ctrl.setValue(acast curSel.borderSize)
        ctrl.dockStyle = .Top
        ctrl.slider.sigStartChange += @|capture|(slider) {
            undo := capture.newUndo(.SetBorderSize)
            undo.valS32 = cast(s32) slider.getValue()
            capture.pushUndo(undo)
        }
        ctrl.slider.sigChanged += @|capture|(slider) {
            capture.undo()
            undo := capture.newUndo(.SetBorderSize)
            undo.valS32 = cast(s32) slider.getValue()
            capture.pushUndo(undo)
        }
    }

    mtd createOpactiy(capture: *Capture)
    {
        curSel := capture.getSelectedBaseForm()

        ctrl := SliderCtrl.create(self, "Opacity")
        ctrl.edit.setForm(.Square)
        ctrl.setMinMaxS64(1, 100)
        ctrl.setValue(acast curSel.alpha)
        ctrl.dockStyle = .Top
        ctrl.slider.sigStartChange += @|capture|(slider) {
            undo := capture.newUndo(.SetAlpha)
            undo.valS32 = cast(s32) slider.getValue()
            capture.pushUndo(undo)
        }
        ctrl.slider.sigChanged += @|capture|(slider) {
            capture.undo()
            undo := capture.newUndo(.SetAlpha)
            undo.valS32 = cast(s32) slider.getValue()
            capture.pushUndo(undo)
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