#global public

struct Dialog
{
    using wnd: Wnd
    wndBottom: *StackLayoutCtrl
    buttons:   Array'(*PushButton)
}

impl IWnd for Dialog
{
    mtd onSysCommandEvent(evt: *SysCommandEvent)
    {
        if evt.sysKind == .Close
            surface.app.exitModal(BtnCancel)
        evt.accepted = true
    }

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
        srf := assume from.app.createSurface(x, y, width, height, .BigCaption | .Border | .Secondary, self)
        srf.wnd.view.backgroundStyle = .Dialog

        metrics := getThemeMetrics()
        h := metrics.btnPush_Height
        h += 2 * metrics.dialog_Padding
        with wndBottom = Wnd.create'StackLayoutCtrl(srf.wnd.view, "", @{0, 0, 0, h})
        {
            .spacing = 10
            .layoutKind = .HorzCenterVertCenter
            .dockStyle = .Bottom
            .backgroundStyle = .DialogBar
            .setAllPaddings(metrics.dialog_Padding)
        }

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
            .form = isDefault ? .Default : .Strong
            .sigPressed += @(btn) { btn.getApp().exitModal(btn.id); }
            buttons.add(btn)
        }

        if #self.id == BtnCancel or #self.id == BtnNo
            surface.setSurfaceFlags(add: .CloseBtn)

        return btn
    }
}