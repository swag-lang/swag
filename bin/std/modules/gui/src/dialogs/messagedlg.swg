using Core

public struct MessageDlg
{
    using dlg:  Dialog
    label:      *Label
    wndBottom:  *Wnd
}

impl IWnd for MessageDlg
{
    mtd onMouseEvent(evt: *MouseEvent)
    {
        dlg.base.IWnd.onMouseEvent(evt)
    }
}

impl MessageDlg
{
    public func create(from: *Surface, x, y: s32)->*MessageDlg
    {
        dlg := Wnd.create'MessageDlg(null, "", @{})

        surface := assume from.app.createSurface(x, y, 500, 300, SurfaceFlags.Border | SurfaceFlags.Secondary, dlg)
        surface.wnd.view.backgroundStyle = .Dialog

        dlg.label = Label.create(surface.wnd.view, "", @{})
        dlg.label.name = "Lorem ipsum dolor sit amet, consectetur <b>adipiscing<b> elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum."
        dlg.label.dockStyle = .Center
        dlg.label.padding = @{10, 10, 10, 10}

        dlg.wndBottom = Wnd.create'Wnd(surface.wnd.view, "", @{0, 0, 0, 50})
        dlg.wndBottom.dockStyle = .Bottom
        dlg.wndBottom.backgroundStyle = .DialogBar
        dlg.wndBottom.padding = @{10, 10, 10, 10}

        return dlg
    }

    public func yesNo(from: *Surface, x, y: s32, message: string)
    {
        dlg := create(from, x, y)
        surface := dlg.surface

        img := Label.create(surface.wnd.view, "", @{0,0,0,100})
        img.icon = from.app.theme.getIcon64(ThemeIcons64.Question)
        img.dockStyle = .Top
        img.padding = @{10, 10, 10, 10}

        btnNo  := ButtonPush.create(dlg.wndBottom, "No", @{0, 0, 100, 30})
        btnNo.dockStyle = .Right
        btnNo.margin = @{5, 0, 5, 0}
        btnNo.sigPressed = @(btn: *ButtonPush) { btn.surface.app.exitModal(0); }

        btnYes := ButtonPush.create(dlg.wndBottom, "Yes", @{0, 0, 100, 30})
        btnYes.isDefault = true
        btnYes.dockStyle = .Right
        btnYes.margin = @{5, 0, 5, 0}
        btnYes.sigPressed = @(btn: *ButtonPush) { btn.surface.app.exitModal(0); }

        surface.center(from)
        discard Dialog.doModal(surface)
    }
}