using Core, Pixel

public struct MessageDlg
{
    using dialog:     Dialog
    labelIcon:        *Label
    label:            *Label
}

public impl MessageDlg
{
    // Creates the message box
    func create(from: *Surface, message: string, icon: Icon = {})->*MessageDlg
    {
        let dlg = Wnd.create'MessageDlg()
        let srf = dlg.createSurface(from, 0, 0, 500, 300)

        let metrics = from.wnd.getThemeMetrics()

        dlg.label = Label.create(srf.getView(), message, {})
        with dlg.label
        {
            .dockStyle         = .Center
            .horzAlignmentText = .Justify
            .padding.x         = metrics.dialog_Padding
            .padding.z         = metrics.dialog_Padding
            .padding.w         = metrics.dialog_Padding
        }

        dlg.setIcon(icon)
        dlg.sendResizeEvent()
        return dlg
    }

    // Display the box, centered, and returns the id of the pressed button
    mtd doModal(from: *Surface)->string
    {
        adaptSizeToMessage()
        surface.center(from)
        return doModal()
    }

    internal mtd adaptSizeToMessage()
    {
        let metrics = getThemeMetrics()

        // Evaluate size of text
        var fake: Painter{fake: true}
        var rsf:  RichStringFormat
        rsf.font = getFont()
        rsf.flags |= .WordWrap
        fake.drawRichString({0, 0, 512, 0}, &label.name, rsf)
        label.name.invalidate()

        // Width
        var w = 0'f32
        visit btn in buttons:
            w += btn.position.width
        w += cast(f32) ((buttons.count - 1) * metrics.dialog_BtnPadding)
        w += 2 * metrics.dialog_Padding
        w += metrics.btnPush_Width
        var wl = label.name.boundRect.width + 2 * metrics.dialog_Padding
        wl += 2 * metrics.wnd_BorderSize
        w = Math.max(w, wl)

        // Height
        var h = Math.min(512, label.name.boundRect.height + metrics.dialog_Padding)
        if labelIcon:
            h += labelIcon.position.height
        else:
            h += metrics.dialog_Padding
        h += wndBottom.position.height
        h += 2 * metrics.wnd_BorderSize
        if surface.flags & .SmallCaption:
            h += metrics.wnd_SmallCaptionCY
        elif surface.flags & .BigCaption:
            h += metrics.wnd_BigCaptionCY

        var pos = surface.position
        pos.width  = w
        pos.height = h
        let sdw = surface.app.theme.metrics.surfaceWnd_ShadowSize
        pos.width += sdw * 2
        pos.height += sdw * 2
        surface.setPosition(pos)
    }

    // Set big icon color (icon must has been set before)
    mtd setIconColor(color: Color)
    {
        Debug.assert(labelIcon != null)
        labelIcon.iconColor = color
    }

    // Set big icon
    mtd setIcon(icon: Icon, color: Color = Argb.Zero)
    {
        if !icon.isValid():
            return

        if !labelIcon:
            labelIcon = Label.create(surface.wnd.view, "", {0, 0, 0, icon.sizeX + icon.sizeX * 0.5})

        with labelIcon
        {
            .dockStyle = .Top
            .iconColor = color
            .setAllPaddings(getThemeMetrics().dialog_Padding)
            .icon              = icon
            .horzAlignmentIcon = .Center
            .vertAlignmentIcon = .Center
        }
    }

    // Message box with a 'ok'
    func ok(from: *Surface, message: string, icon: Icon = {})
    {
        with let dlg = create(from, message, icon)
        {
            .addButton("OK", Dialog.BtnOk, true)
        }

        discard dlg.doModal(from)
    }

    // Message box with a 'ok' and 'cancel' button
    func okCancel(from: *Surface, message: string, defaultId = Dialog.BtnCancel, icon: Icon = {})->WndId
    {
        with let dlg = create(from, message, icon)
        {
            .addButton("OK", Dialog.BtnOk, defaultId == Dialog.BtnOk)
            .addButton("Cancel", Dialog.BtnCancel, defaultId == Dialog.BtnCancel)
        }

        return dlg.doModal(from)
    }

    // Message box with a 'yes' and 'no' button
    func yesNo(from: *Surface, message: string, title: string = null, defaultId = Dialog.BtnNo, icon: Icon = {})->WndId
    {
        var ic = icon.isValid() ? icon : from.app.theme.getIcon64(ThemeIcons64.Question)
        with let dlg = create(from, message, ic)
        {
            .surface.setTitle(title)
            .addButton("Yes", Dialog.BtnYes, defaultId == Dialog.BtnYes)
            .addButton("No", Dialog.BtnNo, defaultId == Dialog.BtnNo)
        }

        return dlg.doModal(from)
    }

    // Message box with a 'yes' and 'no' button
    func yesNoCancel(from: *Surface, message: string, defaultId = Dialog.BtnCancel, icon: Icon = {})->WndId
    {
        with let dlg = create(from, message, icon.isValid() ? icon : from.app.theme.getIcon64(ThemeIcons64.Question))
        {
            .addButton("Yes", Dialog.BtnYes, defaultId == Dialog.BtnYes)
            .addButton("No", Dialog.BtnNo, defaultId == Dialog.BtnNo)
            .addButton("Cancel", Dialog.BtnCancel, defaultId == Dialog.BtnCancel)
        }

        return dlg.doModal(from)
    }

    // Message box to 'confirm' something
    func confirm(from: *Surface, message: string, question: string = null)->WndId
    {
        var txt = String.from(message)
        txt += "\n"
        txt += question orelse "Do you want to continue ?"

        return yesNo(from, txt.toString(), "Please confirm")
    }

    // Message box to show the current raised error.
    // @err() must still contain the raised error.
    func error(from: *Surface, message: string = null)
    {
        var txt: String = String.from(message)
        if !txt.isEmpty():
            txt += "\n"

        if @err() != null
        {
            switch @err()
            {
            case Swag.SystemError:
                let err = cast(Swag.SystemError) @err()
                txt += err.message
            }
        }

        var icon = from.app.theme.getIcon64(.Critical)
        with let dlg = create(from, txt.toString(), icon)
        {
            .surface.setTitle("Error")
            .addButton("OK", Dialog.BtnOk, true)
        }

        discard dlg.doModal(from)
    }
}
