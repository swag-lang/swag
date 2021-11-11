public struct Dialog
{
    using base: Wnd
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