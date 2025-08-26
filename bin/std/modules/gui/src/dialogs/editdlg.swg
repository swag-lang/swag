#global marked
using Core, Pixel

public struct EditDlg
{
    using dialog:     Dialog
    label:            *Label
    edit:             *EditBox
    richEdit:         *RichEditCtrl
}

public impl EditDlg
{
    // Creates the message box
    func createSingleLine(from: *Surface)->*EditDlg
    {
        let dlg     = Wnd.create'EditDlg()
        let metrics = from.wnd.getThemeMetrics()
        let srf     = dlg.createSurface(from, 0, 0, 450, 210)

        dlg.edit = EditBox.create(srf.getView(), "")
        with dlg.edit
        {
            .dockStyle = .Bottom
            .margin    = metrics.dialog_Padding
            .style.addStyleSheetColors("edit_Bk $dlg_BtnBarBk")
            .style.addStyleSheetColors("edit_FocusBk $dlg_BtnBarBk")
            .setForm(.Round)
            .setFocus()
        }

        dlg.addButton("OK", Dialog.BtnOk, true)
        dlg.addButton("Cancel", Dialog.BtnCancel)
        dlg.sendResizeEvent()
        return dlg
    }

    // Creates the message box
    func createMultiLine(from: *Surface, heightEdit = 150)->*EditDlg
    {
        let dlg     = Wnd.create'EditDlg()
        let metrics = from.wnd.getThemeMetrics()
        let srf     = dlg.createSurface(from, 0, 0, 500, 180 + heightEdit, .Sizeable)
        srf.minSize.x = 500
        srf.minSize.y = 180 + heightEdit

        dlg.richEdit = RichEditCtrl.create(srf.getView(), {0, 0, 0, heightEdit})
        with dlg.richEdit
        {
            .anchorStyle = .Height
            .dockStyle   = .Bottom
            .margin      = metrics.dialog_Padding
            .richEdFlags |= .AutoLoseFocus
            .bkColorModel = "$dlg_BtnBarBk"
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
        if me.edit:
            me.edit.setText(text)
        else:
            me.richEdit.setText(text)
    }

    // Get the text
    mtd getText()->String
    {
        if me.edit:
            return me.edit.text
        return me.richEdit.getText()
    }

    // Associate a message above the editbox
    mtd setMessage(message: string, height: s32 = 40)
    {
        if !me.label
        {
            me.label = Label.create(me.surface.getView(), message, {0, 0, 0, height})
            with me.label
            {
                .dockStyle         = .Center
                .vertAlignmentText = .Center
                .horzAlignmentText = .Center
            }

            if me.edit:
                me.edit.margin.y = 0
            else:
                me.richEdit.margin.y = 0
            var pos = me.surface.position
            pos.height += height
            me.surface.minSize.y += height

            me.richEdit.anchorStyle = .None
            me.surface.setPosition(pos)
            me.richEdit.anchorStyle = .Height
        }
        else
        {
            me.label.setText(message)
        }
    }
}
