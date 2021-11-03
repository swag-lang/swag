public struct MessageDlg
{
    using base: Dialog
}

impl MessageDlg
{
    public func doModal(from: *Surface, x, y: s32)
    {
        app := from.app

        surface := assume app.createSurface(x, y, 500, 250, SurfaceFlags.BigCaption | SurfaceFlags.CloseBtn | SurfaceFlags.Secondary)
        surface.bringToTop()
        surface.show()

        app.doModalLoop(surface)
    }
}