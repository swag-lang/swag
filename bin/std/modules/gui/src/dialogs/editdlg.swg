using Core, Pixel

public struct EditDlg
{
    using dialog:   Dialog
    label:          *Label
    edit:           *EditBox
}

public impl EditDlg
{
    // Creates the message box
    func create(from: *Surface)->*EditDlg
    {
        dlg := Wnd.create'EditDlg()
        srf := dlg.createSurface(from, 0, 0, 450, 210)

        metrics := from.wnd.getThemeMetrics()

        dlg.edit = EditBox.create(srf.getView(), "")
        with dlg.edit
        {
            .dockStyle = .Bottom
            .margin = metrics.dialog_Padding
            .setFocus()
        }

        dlg.addButton("OK", Dialog.BtnOk, true)
        dlg.addButton("Cancel", Dialog.BtnCancel)
        dlg.sendResizeEvent()
        return dlg
    }

    // Associate a message above the editbox
    mtd setMessage(message: string, height: s32 = 40)
    {
        if !label
        {
            metrics := getThemeMetrics()
            label = Label.create(surface.getView(), message, @{0, 0, 0, height})
            with label
            {
                .dockStyle = .Center
                .vertAlignmentText = .Center
                .horzAlignmentText = .Center
            }

            edit.margin.y = 0
            pos := surface.position
            pos.height += height
            surface.setPosition(pos)
        }
        else
        {
            label.setText(message)
        }
    }
}