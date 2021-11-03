public struct MessageDlg
{
    using base: Dialog
}

impl MessageDlg
{
    public func doModal(surface: *Surface, x, y: s32)
    {
        app := surface.app

        srf := assume app.createSurface(x, y, 500, 250, SurfaceFlags.BigCaption | SurfaceFlags.CloseBtn | SurfaceFlags.Secondary)
        srf.show()
        srf.bringToTop()

        app.doModalLoop(srf)
    }
}