public struct MessageDlg
{
    using base: Dialog
}

impl MessageDlg
{
    public func create(from: *Surface, x, y: s32)->*Surface
    {
        surface := assume from.app.createSurface(x, y, 500, 250, SurfaceFlags.BigCaption | SurfaceFlags.CloseBtn | SurfaceFlags.Border | SurfaceFlags.Secondary)
        return surface
    }

    public func yesNo(from: *Surface, x, y: s32, message: string)
    {
        surface := create(from, x, y)
        btnYes := ButtonPush.create(surface.wnd.view, "Yes", @{10, 10, 100, 30})
        btnYes.isDefault = true
        btnYes.dockStyle = .Left
        btnYes.marginRightBottom.y = 4

        btnNo  := ButtonPush.create(surface.wnd.view, "No", @{110, 10, 100, 30})
        btnNo.dockStyle = .Top

        btnNo  = ButtonPush.create(surface.wnd.view, "No", @{110, 10, 100, 30})
        btnNo.dockStyle = .Right
        btnNo  = ButtonPush.create(surface.wnd.view, "No", @{110, 10, 100, 30})
        btnNo.dockStyle = .Bottom

        btnNo  = ButtonPush.create(surface.wnd.view, "No", @{110, 10, 100, 30})
        btnNo.dockStyle = .Left
        btnNo  = ButtonPush.create(surface.wnd.view, "No", @{110, 10, 100, 30})
        btnNo.dockStyle = .Top
        btnNo  = ButtonPush.create(surface.wnd.view, "No", @{110, 10, 100, 30})
        btnNo.dockStyle = .Right
        btnNo  = ButtonPush.create(surface.wnd.view, "No", @{110, 10, 100, 30})
        btnNo.dockStyle = .Bottom

        btnNo  = ButtonPush.create(surface.wnd.view, "Center", @{110, 10, 100, 30})
        btnNo.dockStyle = .Center

        Dialog.doModal(surface)
    }
}