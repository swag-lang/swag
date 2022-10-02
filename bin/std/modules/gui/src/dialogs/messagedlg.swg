using Core

public struct MessageDlg
{
    using dialog:   Dialog
    labelIcon:      *Label
    label:          *Label
}

public impl MessageDlg
{
    // Creates the message box
    func create(from: *Surface, message: string, icon: Icon = @{})->*MessageDlg
    {
        dlg := Wnd.create'MessageDlg()
        srf := dlg.createSurface(from, 0, 0, 500, 300)

        metrics := from.wnd.getThemeMetrics()

        with dlg
        {
            .label = Label.create(srf.getView(), message, @{})
            .label.dockStyle = .Center
            .label.horzAlignmentText = .Justify
            .label.padding.x = metrics.dialog_Padding
            .label.padding.z = metrics.dialog_Padding
            .label.padding.w = metrics.dialog_Padding
            .setIcon(icon)
            .sendResizeEvent()
        }

        return dlg
    }

    // Display the box, centered, and returns the id of the pressed button
    mtd doModal(from: *Surface)->string
    {
        adaptSizeToMessage()
        surface.center(from)
        return doModal()
    }

    private mtd adaptSizeToMessage()
    {
        metrics := getThemeMetrics()

        // Evaluate size of text
        var fake: Pixel.Painter{fake: true}
        var rsf:  Pixel.RichStringFormat
        rsf.font = surface.app.theme.res.fontDefault
        rsf.flags |= .WordWrap
        fake.drawRichString(@{0, 0, 512, 0}, &label.name, rsf)
        label.name.invalidate()

        // Width
        w := 0'f32
        visit btn: buttons
            w += btn.position.width
        w += cast(f32) ((buttons.count - 1) * metrics.dialog_BtnPadding)
        w += 2 * metrics.dialog_Padding
        w += metrics.btnPush_Width
        wl := label.name.boundRect.width + 2 * metrics.dialog_Padding
        wl += 2 * metrics.wnd_BorderSize
        w = Math.max(w, wl)

        // Height
        h := Math.min(512, label.name.boundRect.height + metrics.dialog_Padding)
        if labelIcon
            h += labelIcon.position.height
        else
            h += metrics.dialog_Padding
        h += wndBottom.position.height
        h += 2 * metrics.wnd_BorderSize
        if surface.flags & .SmallCaption
            h += metrics.wnd_SmallCaptionCY
        elif surface.flags & .BigCaption
            h += metrics.wnd_BigCaptionCY

        pos := surface.position
        pos.width = w
        pos.height = h
        sdw := surface.app.theme.metrics.surfaceWnd_ShadowSize
        pos.width += sdw * 2
        pos.height += sdw * 2
        surface.setPosition(pos)
    }

    // Set big icon
    mtd setIcon(icon: Icon)
    {
        if !icon.isValid()
            return

        if !labelIcon
            labelIcon = Label.create(surface.wnd.view, "", @{0,0,0,icon.sizeX + icon.sizeX * 0.5})

        with labelIcon
        {
            .dockStyle = .Top
            .setAllPaddings(getThemeMetrics().dialog_Padding)
            .icon = icon
            .horzAlignmentIcon = .Center
            .vertAlignmentIcon = .Center
        }
    }

    // Message box with a 'ok'
    func ok(from: *Surface, message: string, icon: Icon = @{})
    {
        with dlg := create(from, message, icon)
        {
            .addButton("OK", Dialog.BtnOk, true)
        }

        discard dlg.doModal(from)
    }

    // Message box with a 'ok' and 'cancel' button
    func okCancel(from: *Surface, message: string, defaultId = Dialog.BtnCancel, icon: Icon = @{})->WndId
    {
        with dlg := create(from, message, icon)
        {
            .addButton("Cancel", Dialog.BtnCancel, defaultId == Dialog.BtnCancel)
            .addButton("OK", Dialog.BtnOk, defaultId == Dialog.BtnOk)
        }

        return dlg.doModal(from)
    }

    // Message box with a 'yes' and 'no' button
    func yesNo(from: *Surface, message: string, defaultId = Dialog.BtnNo, icon: Icon = @{})->WndId
    {
        with dlg := create(from, message, icon.isValid() ? icon : from.app.theme.getIcon64(ThemeIcons64.Question))
        {
            .addButton("No", Dialog.BtnNo, defaultId == Dialog.BtnNo)
            .addButton("Yes", Dialog.BtnYes, defaultId == Dialog.BtnYes)
        }

        return dlg.doModal(from)
    }

    // Message box with a 'yes' and 'no' button
    func yesNoCancel(from: *Surface, message: string, defaultId = Dialog.BtnCancel, icon: Icon = @{})->WndId
    {
        with dlg := create(from, message, icon.isValid() ? icon : from.app.theme.getIcon64(ThemeIcons64.Question))
        {
            .addButton("Cancel", Dialog.BtnCancel, defaultId == Dialog.BtnCancel)
            .addButton("No", Dialog.BtnNo, defaultId == Dialog.BtnNo)
            .addButton("Yes", Dialog.BtnYes, defaultId == Dialog.BtnYes)
        }

        return dlg.doModal(from)
    }
}