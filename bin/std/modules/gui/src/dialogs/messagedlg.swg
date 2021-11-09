using Core

public struct MessageDlg
{
    using dlg: Dialog
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
    public func create(from: *Surface, x, y: s32)->*Surface
    {
        view := Wnd.create'MessageDlg(null, "", @{})
        surface := assume from.app.createSurface(x, y, 500, 250, SurfaceFlags.Border | SurfaceFlags.Secondary, view)
        return surface
    }

    public func yesNo(from: *Surface, x, y: s32, message: string)
    {
        surface := create(from, x, y)
        surface.wnd.view.backgroundStyle = .Dialog

        label := Label.create(surface.wnd.view, "", @{})
        label.name = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum."
        label.dockStyle = .Center
        label.padding = @{10, 10, 10, 10}

        wndBottom := Wnd.create'Wnd(surface.wnd.view, "", @{0, 0, 0, 50})
        wndBottom.dockStyle = .Bottom
        wndBottom.backgroundStyle = .DialogBar

        btnNo  := ButtonPush.create(wndBottom, "No", @{0, 0, 100, 30})
        btnNo.dockStyle = .Right
        btnNo.margin = @{10, 10, 10, 10}
        btnNo.sigPressed = @(btn: *ButtonPush) { btn.surface.app.exitModal(0); }

        btnYes := ButtonPush.create(wndBottom, "Yes", @{0, 0, 100, 30})
        btnYes.isDefault = true
        btnYes.dockStyle = .Right
        btnYes.margin = @{10, 10, 0, 10}
        btnYes.sigPressed = @(btn: *ButtonPush) { btn.surface.app.exitModal(0); }

        surface.center(from)
        discard Dialog.doModal(surface)
    }
}