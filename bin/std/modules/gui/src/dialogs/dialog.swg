public struct Dialog
{
    using wnd: Wnd
}

impl Dialog
{
    public func doModal(dlg: *Surface)->string
    {
        dlg.bringToTop()
        dlg.show()
        return dlg.app.doModalLoop(dlg)
    }
}