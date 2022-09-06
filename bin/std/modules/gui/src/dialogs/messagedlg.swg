using Core

public struct MessageDlg
{
    using dialog:   Dialog
    labelIcon:      *Label
    label:          *Label
    wndBottom:      *Wnd
    buttons:        Array'(*PushButton)
}

impl IWnd for MessageDlg
{
    mtd onKeyEvent(evt: *KeyEvent)
    {
        if evt.kind == .KeyPressed
        {
            // Pressing enter validates the default button
            switch evt.key
            {
            case Tab, Right:
                visit btn, idx: buttons
                {
                    if btn.form == .Default
                    {
                        btn.form = .Normal
                        btn.invalidate()
                        nextBtn := buttons[(idx + 1) % buttons.count]
                        nextBtn.form = .Default
                        nextBtn.invalidate()
                        break
                    }
                }

            case Left:
                visit btn, idx: buttons
                {
                    if btn.form == .Default
                    {
                        btn.form = .Normal
                        btn.invalidate()
                        prevBtn := idx ? buttons[idx - 1] : buttons.back()
                        prevBtn.form = .Default
                        prevBtn.invalidate()
                        break
                    }
                }

            case Enter:
                visit btn: buttons
                {
                    if btn.form == .Default
                    {
                        surface.app.exitModal(btn.id)
                        break
                    }
                }

            // Pressing escape validates some specific buttons
            case Escape:
                visit btn: buttons
                {
                    if btn.id == BtnNo or btn.id == BtnCancel
                    {
                        surface.app.exitModal(btn.id)
                        break
                    }
                }
            }
        }

        evt.accepted = true
    }
}

public impl MessageDlg
{
    const BtnYes    = "MessageDlg.yes"
    const BtnNo     = "MessageDlg.no"
    const BtnOk     = "MessageDlg.ok"
    const BtnCancel = "MessageDlg.cancel"

    // Creates the message box
    func create(from: *Surface, x, y: s32, message: string, icon: Icon = @{})->*MessageDlg
    {
        dlg := Wnd.create'MessageDlg()

        srf := assume from.app.createSurface(x, y, 500, 300, SurfaceFlags.Border | SurfaceFlags.Secondary, dlg)
        srf.wnd.view.backgroundStyle = .Dialog
        metrics := &from.app.theme.metrics

        with dlg
        {
            label = Label.create(srf.wnd.view, message, @{})
            label.dockStyle = .Center
            label.horzAlignmentText = .Justify
            label.padding.x = metrics.msgDlg_Padding
            label.padding.z = metrics.msgDlg_Padding
            label.padding.w = metrics.msgDlg_Padding

            h := srf.app.theme.metrics.btnPush_Height
            h += 2 * metrics.msgDlg_Padding
            wndBottom = Wnd.create'Wnd(srf.wnd.view, "", @{0, 0, 0, h})
            wndBottom.dockStyle = .Bottom
            wndBottom.backgroundStyle = .DialogBar
            wndBottom.setAllPaddings(metrics.msgDlg_Padding)

            setIcon(icon)
            setFocus()
        }

        dlg.sendResizeEvent()
        return dlg
    }

    // Display the box, centered, and returns the id of the pressed button
    mtd doModal(from: *Surface)->string
    {
        adaptSizeToMessage()
        surface.center(from)
        return Dialog.doModal(surface)
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
        var w: f32 = 0
        visit btn: buttons w += btn.position.width
        w += cast(f32) ((buttons.count - 1) * metrics.msgDlg_BtnPadding)
        w += 2 * metrics.msgDlg_Padding
        w += metrics.btnPush_Width
        wl := label.name.boundRect.width + 2 * metrics.msgDlg_Padding
        wl += 2 * metrics.wnd_BorderSize
        w = Math.max(w, wl)

        // Height
        h := Math.min(512, label.name.boundRect.height + metrics.msgDlg_Padding)
        if labelIcon
            h += labelIcon.position.height
        h += wndBottom.position.height
        h += 2 * metrics.wnd_BorderSize

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
        {
            h := icon.sizeX + icon.sizeX * 0.5
            labelIcon = Label.create(surface.wnd.view, "", @{0,0,0,h})
            labelIcon.dockStyle = .Top
            labelIcon.setAllPaddings(getThemeMetrics().msgDlg_Padding)
        }

        labelIcon.icon = icon
        labelIcon.horzAlignmentIcon = .Center
        labelIcon.vertAlignmentIcon = .Center
    }

    // Add a button
    #[Swag.Discardable]
    mtd addButton(str: string, id: WndId, isDefault: bool = false)->*PushButton
    {
        btn := PushButton.create(wndBottom, str, @{}, #self.id)
        btn.dockStyle = .Right
        btn.form = isDefault ? .Default : .Strong
        metrics := getThemeMetrics()
        btn.margin = @{metrics.msgDlg_BtnPadding, 0, metrics.msgDlg_BtnPadding, 0}
        btn.sigPressed += @(btn) { btn.surface.app.exitModal(btn.id); }
        buttons.insertAt(0, btn)
        return btn
    }

    // Message box with a 'ok'
    func ok(from: *Surface, message: string, icon: Icon = @{})
    {
        dlg := create(from, 0, 0, message, icon)
        dlg.addButton("OK", BtnOk, true)
        discard dlg.doModal(from)
    }

    // Message box with a 'ok' and 'cancel' button
    func okCancel(from: *Surface, message: string, defaultId = BtnCancel, icon: Icon = @{})->WndId
    {
        dlg := create(from, 0, 0, message, icon)
        dlg.addButton("Cancel", BtnCancel, defaultId == BtnCancel)
        discard dlg.addButton("OK", BtnOk, defaultId == BtnOk)
        return dlg.doModal(from)
    }

    // Message box with a 'yes' and 'no' button
    func yesNo(from: *Surface, message: string, defaultId = BtnNo, icon: Icon = @{})->WndId
    {
        dlg := create(from, 0, 0, message, icon.isValid() ? icon : from.app.theme.getIcon64(ThemeIcons64.Question))
        dlg.addButton("No", BtnNo, defaultId == BtnNo)
        dlg.addButton("Yes", BtnYes, defaultId == BtnYes)
        return dlg.doModal(from)
    }

    // Message box with a 'yes' and 'no' button
    func yesNoCancel(from: *Surface, message: string, defaultId = BtnCancel, icon: Icon = @{})->WndId
    {
        dlg := create(from, 0, 0, message, icon.isValid() ? icon : from.app.theme.getIcon64(ThemeIcons64.Question))
        dlg.addButton("Cancel", BtnCancel, defaultId == BtnCancel)
        dlg.addButton("No", BtnNo, defaultId == BtnNo)
        dlg.addButton("Yes", BtnYes, defaultId == BtnYes)
        return dlg.doModal(from)
    }
}