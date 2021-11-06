public struct MessageDlg
{
    using base: Dialog
}

impl MessageDlg
{
    public func create(from: *Surface, x, y: s32)->*Surface
    {
        surface := assume from.app.createSurface(x, y, 500, 250, SurfaceFlags.BigCaption | SurfaceFlags.CloseBtn | SurfaceFlags.Sizeable | SurfaceFlags.Border | SurfaceFlags.Secondary)
        return surface
    }

    public func yesNo(from: *Surface, x, y: s32, message: string)
    {
        surface := create(from, x, y)

        wndBottom := Wnd.create'Wnd(surface.wnd.view, "", @{0, 0, 0, 50})
        wndBottom.dockStyle = .Bottom

        btnNo  := ButtonPush.create(wndBottom, "No", @{110, 10, 100, 30})
        btnNo.dockStyle = .Right
        btnNo.marginRightBottom = @{10, 10}
        btnNo.marginLeftTop = @{10, 10}

        btnYes := ButtonPush.create(wndBottom, "Yes", @{10, 10, 100, 30})
        btnYes.isDefault = true
        btnYes.dockStyle = .Right
        btnYes.marginRightBottom = @{0, 10}
        btnYes.marginLeftTop = @{10, 10}

        Dialog.doModal(surface)
    }
}