#global public

struct Dialog
{
    using wnd: Wnd
    wndBottom: *Wnd
    buttons:   Array'(*PushButton)
}

impl IWnd for Dialog
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

impl Dialog
{
    const BtnYes    = "Dialog.yes"
    const BtnNo     = "Dialog.no"
    const BtnOk     = "Dialog.ok"
    const BtnCancel = "Dialog.cancel"

    mtd createSurface(from: *Surface, x, y: s32, width = 512, height = 300)->*Surface
    {
        srf := assume from.app.createSurface(x, y, width, height, SurfaceFlags.Border | SurfaceFlags.Secondary, self)
        srf.wnd.view.backgroundStyle = .Dialog

        metrics := getThemeMetrics()
        h := metrics.btnPush_Height
        h += 2 * metrics.msgDlg_Padding
        wndBottom = Wnd.create'Wnd(srf.wnd.view, "", @{0, 0, 0, h})
        wndBottom.dockStyle = .Bottom
        wndBottom.backgroundStyle = .DialogBar
        wndBottom.setAllPaddings(metrics.msgDlg_Padding)

        setFocus()

        return srf
    }

    // Display dialog as modal, and returns the user selected window id
    mtd doModal()->string
    {
        surface.bringToTop()
        surface.show()
        return surface.app.doModalLoop(surface)
    }

    // Add a button
    #[Swag.Discardable]
    mtd addButton(str: string, id: WndId, isDefault: bool = false)->*PushButton
    {
        metrics := getThemeMetrics()
        with btn := PushButton.create(wndBottom, str, @{}, #self.id)
        {
            .dockStyle = .Right
            .form = isDefault ? .Default : .Strong
            .margin = @{metrics.msgDlg_BtnPadding, 0, metrics.msgDlg_BtnPadding, 0}
            .sigPressed += @(btn) { btn.surface.app.exitModal(btn.id); }
            buttons.insertAt(0, btn)
        }
        return btn
    }
}