public struct Dialog
{
    using base: Wnd
}

impl Dialog
{
    public func doModal(dlg: *Surface)
    {
        dlg.bringToTop()
        dlg.show()
        dlg.app.doModalLoop(dlg)
    }
}