#global public

struct Dialog
{
    using wnd:            Wnd
    wndBottom:            *StackLayoutCtrl
    buttons:              Array'(*PushButton)
    sigPressedButton:     func||(*Dialog, WndId)->bool
    fromSurface:          *Surface
}

impl IWnd for Dialog
{
    mtd impl onSysCommandEvent(evt: *SysCommandEvent)
    {
        if evt.kind == .Close:
            me.validateId(BtnCancel)
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
                foreach btn, idx in me.buttons
                {
                    if btn.form == .Default
                    {
                        btn.form = .Normal
                        btn.invalidate()
                        let nextBtn = me.buttons[(idx + 1) % me.buttons.count]
                        nextBtn.form = .Default
                        nextBtn.invalidate()
                        break
                    }
                }

            case Left:
                foreach btn, idx in me.buttons
                {
                    if btn.form == .Default
                    {
                        btn.form = .Normal
                        btn.invalidate()
                        let prevBtn = idx ? me.buttons[idx - 1] : me.buttons.back()
                        prevBtn.form = .Default
                        prevBtn.invalidate()
                        break
                    }
                }

            case Enter:
                foreach btn in me.buttons
                {
                    if btn.form == .Default
                    {
                        me.validateId(btn.id)
                        break
                    }
                }

            case Escape:
                if me.surface.flags.has(.CloseBtn)
                {
                    me.validateId(BtnCancel)
                    break
                }

                foreach btn in me.buttons
                {
                    if btn.id == BtnNo or btn.id == BtnCancel
                    {
                        me.validateId(btn.id)
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
    newCmdId("BtnYes")
    newCmdId("BtnNo")
    newCmdId("BtnOk")
    newCmdId("BtnCancel")

    // Creates a simple dialog box
    func createDialog(from: *Surface, width = 512, height = 300, title: string = null, flags = SurfaceFlags.Zero)->*Dialog
    {
        let dlg = Wnd.create'Dialog()
        let srf = dlg.createSurface(from, 0, 0, width, height, flags)
        srf.setTitle(title)
        srf.center(from)
        dlg.fromSurface = from
        return dlg
    }

    // Creates an associated surface
    mtd createSurface(from: *Surface, x, y: s32, width = 512, height = 300, flags = SurfaceFlags.Zero)->*Surface
    {
        let surfaceFlags = SurfaceFlags.BigCaption | SurfaceFlags.Secondary | flags
        let srf          = assume from.app.createSurface(x, y, width, height, surfaceFlags, me)
        srf.wnd.view.backgroundStyle = .Dialog

        let metrics = me.getThemeMetrics()
        var h       = metrics.btnPush_Height
        h += 2 * metrics.dialog_Padding
        with me.wndBottom = Wnd.create'StackLayoutCtrl(srf.wnd.view, "", {0, 0, 0, h})
        {
            .spacing         = 10
            .layoutKind      = .HorzCenterVertCenter
            .dockStyle       = .Bottom
            .backgroundStyle = .DialogBar
            .setAllPaddings(metrics.dialog_Padding)
        }

        me.setFocus()
        return srf
    }

    // Returns the view where stuff can be put.
    mtd getView()->*Wnd
    {
        return me.surface.wnd.view
    }

    // Display dialog as modal, and returns the user selected window id
    mtd doModal()->string
    {
        me.surface.bringToTop()
        me.surface.show()
        if me.fromSurface and me.fromSurface.isTopMost():
            me.surface.setTopMost()
        return me.surface.app.doModalLoop(me.surface)
    }

    // Add a button
    #[Swag.Discardable]
    mtd addButton(str: string, id: WndId, isDefault: bool = false)->*PushButton
    {
        with let btn = PushButton.create(me.wndBottom, str, {}, id)
        {
            .form = isDefault ? .Default : .Strong

            .sigPressed += mtd||(btn)
            {
                if !me.sigPressedButton or me.sigPressedButton(me, btn.id):
                    btn.getApp().exitModal(btn.id)
            }

            me.buttons.add(btn)
        }

        if id == BtnCancel or id == BtnNo:
            me.surface.setSurfaceFlags(add: .CloseBtn)

        return btn
    }

    mtd validateId(btnId: WndId)
    {
        if !me.sigPressedButton or me.sigPressedButton(me, btnId):
            me.getApp().exitModal(btnId)
    }
}
