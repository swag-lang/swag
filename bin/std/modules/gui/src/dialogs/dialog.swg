public struct Dialog
{
    using base: Wnd
}

impl Dialog
{
    public func doModal(dlg: *Surface)->s32
    {
        dlg.bringToTop()
        dlg.show()
        return dlg.app.doModalLoop(dlg)
    }
}