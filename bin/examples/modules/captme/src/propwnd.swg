using Gui

struct PropWnd
{
    using wnd:  Wnd
    main:       *MainWnd
}

impl PropWnd
{
    mtd getCapture()->*Capture
    {
        return main.editWnd.editView.capture
    }

    mtd selectionHasChanged()
    {
        visit c: childs
            c.destroy()

        capture := getCapture()
        if !capture or !capture.selection.count
            return
        curSel := capture.getSelectedBaseForm()

        opacity := SliderCtrl.create(self, "Opacity", @{0, 0, 300})
        opacity.setMinMaxS64(1, 100)
        opacity.setValue(acast curSel.alpha)
        opacity.dockStyle = .Top
        opacity.slider.sigStartChange += @|capture|(slider) {
            undo := capture.newUndo(.SetAlpha)
            undo.valS32 = cast(s32) slider.getValue()
            capture.pushUndo(undo)
            capture.editView.invalidate()
        }
        opacity.slider.sigChanged += @|capture|(slider) {
            capture.undo()
            undo := capture.newUndo(.SetAlpha)
            undo.valS32 = cast(s32) slider.getValue()
            capture.pushUndo(undo)
            capture.editView.invalidate()
        }

        capture.editView.setFocus()
    }
}