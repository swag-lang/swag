using Core

public struct MessageDlg
{
    using dialog:   Dialog
    labelIcon:      *Label
    label:          *Label
    wndBottom:      *Wnd
    buttons:        Array'(*ButtonPush)
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
                    if btn.isDefault
                    {
                        btn.isDefault = false
                        nextBtn := buttons[(idx + 1) % buttons.count]
                        nextBtn.isDefault = true
                        nextBtn.invalidate()
                        break
                    }
                }

            case Left:
                visit btn, idx: buttons
                {
                    if btn.isDefault
                    {
                        btn.isDefault = false
                        prevBtn := idx ? buttons[idx - 1] : buttons.back()
                        prevBtn.isDefault = true
                        prevBtn.invalidate()
                        break
                    }
                }

            case Enter:
                visit btn: buttons
                {
                    if btn.isDefault
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
    protected const Padding = 20'f32

    const BtnYes    = "MessageDlg.yes"
    const BtnNo     = "MessageDlg.no"
    const BtnOk     = "MessageDlg.ok"
    const BtnCancel = "MessageDlg.cancel"

    func create(from: *Surface, x, y: s32, message: string, icon: Icon = @{})->*MessageDlg
    {
        dlg := Wnd.create'MessageDlg(null)

        srf := assume from.app.createSurface(x, y, 500, 300, SurfaceFlags.Border | SurfaceFlags.Secondary, dlg)
        srf.wnd.view.backgroundStyle = .Dialog

        using dlg
        label = Label.create(srf.wnd.view, message, @{})
        label.dockStyle = .Center
        label.padding = Padding
        label.horzAlignment = .Justify
        label.backgroundStyle = .DialogBar

        h := srf.app.theme.metrics.btnPush_Height
        h += 2 * Padding
        wndBottom = Wnd.create'Wnd(srf.wnd.view, "", @{0, 0, 0, h})
        wndBottom.dockStyle = .Bottom
        wndBottom.backgroundStyle = .DialogBar
        wndBottom.padding = Padding

        dlg.setIcon(icon)
        dlg.setFocus()
        return dlg
    }

    protected mtd display(from: *Surface)->string
    {
        adaptSizeToMessage()
        surface.center(from)
        return Dialog.doModal(surface)
    }

    protected mtd adaptSizeToMessage()
    {
        var fake: Pixel.Painter{fake: true}
        var rsf:  Pixel.RichStringFormat
        rsf.font = surface.app.theme.res.fontDefault
        rsf.flags |= .WordWrap
        fake.drawRichString(@{0, 0, 512, 256}, &label.name, rsf)
        label.name.invalidate()

        // Width
        var w: f32 = 0
        visit btn: buttons w += btn.position.width
        w += (buttons.count - 1) * 5
        w += 2 * Padding
        w += getThemeMetrics().btnPush_Width
        w = Math.max(w, label.name.boundRect.width + getThemeMetrics().btnPush_Width)

        // Height
        h := Math.min(512, label.name.boundRect.height + 2 * Padding)
        if labelIcon
            h += labelIcon.position.height
        h += wndBottom.position.height
        h += 2 * getThemeMetrics().wnd_BorderSize

        pos := surface.position
        pos.width = w
        pos.height = h
        surface.setPosition(pos)
    }

    // Set big icon
    mtd setIcon(icon: Icon)
    {
        if !icon.isValid()
            return
        if !labelIcon
        {
            h := icon.size + icon.size * 0.5
            labelIcon = Label.create(surface.wnd.view, "", @{0,0,0,h})
            labelIcon.dockStyle = .Top
            labelIcon.padding = Padding
        }

        labelIcon.icon = icon
        labelIcon.horzAlignment = .Center
        labelIcon.vertAlignment = .Center
    }

    // Add a button
    mtd addButton(str, id: string, isDefault: bool)->*ButtonPush
    {
        btn := ButtonPush.create(wndBottom, str)
        btn.id = #scopefct.id
        btn.dockStyle = .Right
        btn.isDefault = isDefault
        btn.margin = @{5, 0, 5, 0}
        btn.sigPressed = @(using btn: *ButtonPush) { surface.app.exitModal(btn.id); }
        buttons.insertAt(0, btn)
        return btn
    }

    // Message box with a 'ok'
    func ok(from: *Surface, message: string, icon: Icon = @{})
    {
        dlg := create(from, 0, 0, message, icon)
        discard dlg.addButton("OK", BtnOk, true)
        discard dlg.display(from)
    }

    // Message box with a 'ok' and 'cancel' button
    func okCancel(from: *Surface, message: string, defaultId = BtnCancel, icon: Icon = @{})->string
    {
        dlg := create(from, 0, 0, message, icon)
        discard dlg.addButton("Cancel", BtnCancel, defaultId == BtnCancel)
        discard dlg.addButton("OK", BtnOk, defaultId == BtnOk)
        return dlg.display(from)
    }

    // Message box with a 'yes' and 'no' button
    func yesNo(from: *Surface, message: string, defaultId = BtnNo, icon: Icon = @{})->string
    {
        dlg := create(from, 0, 0, message, icon.isValid() ? icon : from.app.theme.getIcon64(ThemeIcons64.Question))
        discard dlg.addButton("No", BtnNo, defaultId == BtnNo)
        discard dlg.addButton("Yes", BtnYes, defaultId == BtnYes)
        return dlg.display(from)
    }

    // Message box with a 'yes' and 'no' button
    func yesNoCancel(from: *Surface, message: string, defaultId = BtnCancel, icon: Icon = @{})->string
    {
        dlg := create(from, 0, 0, message, icon.isValid() ? icon : from.app.theme.getIcon64(ThemeIcons64.Question))
        discard dlg.addButton("Cancel", BtnCancel, defaultId == BtnCancel)
        discard dlg.addButton("No", BtnNo, defaultId == BtnNo)
        discard dlg.addButton("Yes", BtnYes, defaultId == BtnYes)
        return dlg.display(from)
    }
}