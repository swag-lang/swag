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

    mtd createOpactiy(capture: *Capture)
    {
        x := 8
        w := parent.position.width - 16

        curSel := capture.getSelectedBaseForm()
        opacity := SliderCtrl.create(self, "Opacity", @{x, 0, w})
        opacity.edit.setForm(.Square)
        opacity.setMinMaxS64(1, 100)
        opacity.setValue(acast curSel.alpha)
        opacity.dockStyle = .Top
        opacity.slider.sigStartChange += @|capture|(slider) {
            undo := capture.newUndo(.SetAlpha)
            undo.valS32 = cast(s32) slider.getValue()
            capture.pushUndo(undo)
        }
        opacity.slider.sigChanged += @|capture|(slider) {
            capture.undo()
            undo := capture.newUndo(.SetAlpha)
            undo.valS32 = cast(s32) slider.getValue()
            capture.pushUndo(undo)
        }
    }

    mtd selectionHasChanged()
    {
        visit c: childs
            c.destroy()

        capture := getCapture()
        if !capture or !capture.selection.count
            return

        createOpactiy(capture)

        capture.editView.setFocus()
    }
}