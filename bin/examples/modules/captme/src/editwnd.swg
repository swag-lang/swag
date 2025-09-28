struct EditWnd
{
    using scrollWnd:     ScrollWnd
    editView:            *EditView
}

impl EditWnd
{
    func create(main: *MainWnd, parent: *Wnd)->*EditWnd
    {
        let scroll = Wnd.create'EditWnd(parent)
        let view   = Wnd.create'EditView(scroll)
        view.scroll     = scroll
        view.main       = main
        scroll.editView = view
        scroll.view     = view
        return scroll
    }

    mtd getCapture()->#null *Capture
    {
        return .editView.capture
    }
}
