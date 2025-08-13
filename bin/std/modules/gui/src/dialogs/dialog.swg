#global public

struct Dialog
{
    using wnd:            Wnd
    wndBottom:            *StackLayoutCtrl
    buttons:              Array'(*PushButton)
    sigPressedButton:     func||(*Dialog, WndId)->bool
}

impl IWnd for Dialog
{
    mtd impl onSysCommandEvent(evt: *SysCommandEvent)
    {
        if evt.kind == .Close:
            validateId(BtnCancel)
        evt.accepted = true
    }

    mtd impl onKeyEvent(evt: *KeyEvent)
    {
        if evt.kind == .KeyPressed
        {
            // Pressing enter validates the default button
            switch evt.key
            {
            case Tab, Right:
                foreach btn, idx in buttons
                {
                    if btn.form == .Default
                    {
                        btn.form = .Normal
                        btn.invalidate()
                        let nextBtn = buttons[(idx + 1) % buttons.count]
                        nextBtn.form = .Default
                        nextBtn.invalidate()
                        break
                    }
                }

            case Left:
                foreach btn, idx in buttons
                {
                    if btn.form == .Default
                    {
                        btn.form = .Normal
                        btn.invalidate()
                        let prevBtn = idx ? buttons[idx - 1] : buttons.back()
                        prevBtn.form = .Default
                        prevBtn.invalidate()
                        break
                    }
                }

            case Enter:
                foreach btn in buttons
                {
                    if btn.form == .Default
                    {
                        validateId(btn.id)
                        break
                    }
                }

            case Escape:
                if surface.flags.has(.CloseBtn)
                {
                    validateId(BtnCancel)
                    break
                }

                foreach btn in buttons
                {
                    if btn.id == BtnNo or btn.id == BtnCancel
                    {
                        validateId(btn.id)
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

    // Creates a simple dialog box
    func createDialog(from: *Surface, width = 512, height = 300, title: string = null, flags = SurfaceFlags.Zero)->*Dialog
    {
        let dlg = Wnd.create'Dialog()
        let srf = dlg.createSurface(from, 0, 0, width, height, flags)
        srf.setTitle(title)
        srf.center(from)
        return dlg
    }

    // Creates an associated surface
    mtd createSurface(from: *Surface, x, y: s32, width = 512, height = 300, flags = SurfaceFlags.Zero)->*Surface
    {
        let surfaceFlags = SurfaceFlags.BigCaption | SurfaceFlags.Secondary | flags
        let srf          = assume from.app.createSurface(x, y, width, height, surfaceFlags, self)
        srf.wnd.view.backgroundStyle = .Dialog

        let metrics = getThemeMetrics()
        var h       = metrics.btnPush_Height
        h += 2 * metrics.dialog_Padding
        with wndBottom = Wnd.create'StackLayoutCtrl(srf.wnd.view, "", {0, 0, 0, h})
        {
            .spacing         = 10
            .layoutKind      = .HorzCenterVertCenter
            .dockStyle       = .Bottom
            .backgroundStyle = .DialogBar
            .setAllPaddings(metrics.dialog_Padding)
        }

        setFocus()
        return srf
    }

    // Returns the view where stuff can be put.
    mtd getView()->*Wnd
    {
        return surface.wnd.view
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
        with let btn = PushButton.create(wndBottom, str, {}, #self.id)
        {
            .form = isDefault ? .Default : .Strong

            .sigPressed += mtd||(btn)
            {
                if !sigPressedButton or sigPressedButton(self, btn.id):
                    btn.getApp().exitModal(btn.id)
            }

            buttons.add(btn)
        }

        if #self.id == BtnCancel or #self.id == BtnNo:
            surface.setSurfaceFlags(add: .CloseBtn)

        return btn
    }

    mtd validateId(btnId: WndId)
    {
        if !sigPressedButton or sigPressedButton(self, btnId):
            getApp().exitModal(btnId)
    }
}
