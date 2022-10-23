using Core, Pixel

public struct EditDlg
{
    using dialog:   Dialog
    label:          *Label
    edit:           *EditBox
    richEdit:       *RichEditCtrl
}

public impl EditDlg
{
    // Creates the message box
    func createSingleLine(from: *Surface, multiLine = false)->*EditDlg
    {
        dlg := Wnd.create'EditDlg()

        srf := dlg.createSurface(from, 0, 0, 450, 210)

        metrics := from.wnd.getThemeMetrics()

        dlg.edit = EditBox.create(srf.getView(), "")
        with dlg.edit
        {
            .dockStyle = .Bottom
            .margin = metrics.dialog_Padding
            .setForm(.Round)
            .setFocus()
        }

        dlg.addButton("OK", Dialog.BtnOk, true)
        dlg.addButton("Cancel", Dialog.BtnCancel)
        dlg.sendResizeEvent()
        return dlg
    }

    // Creates the message box
    func createMultiLine(from: *Surface, heightEdit = 100)->*EditDlg
    {
        dlg := Wnd.create'EditDlg()
        srf := dlg.createSurface(from, 0, 0, 450, 180 + heightEdit)
        metrics := from.wnd.getThemeMetrics()

        dlg.richEdit = RichEditCtrl.create(srf.getView(), @{0, 0, 0, heightEdit})
        with dlg.richEdit
        {
            .dockStyle = .Bottom
            .margin = metrics.dialog_Padding
            .setForm(.Edit)
            .setFrameForm(.Round)
            .setFrameFlags(.FocusBorder)
            .setFocus()
        }

        dlg.addButton("OK", Dialog.BtnOk, true)
        dlg.addButton("Cancel", Dialog.BtnCancel)
        dlg.sendResizeEvent()
        return dlg
    }

    // Set the text
    mtd setText(text: string)
    {
        if edit
            edit.setText(text)
        else
            richEdit.setText(text)
    }

    // Get the text
    mtd getText()->String
    {
        if edit
            return edit.text
        return richEdit.getText()
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