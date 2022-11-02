using Core, Pixel

#[Swag.Opaque]
public struct PropertyListView
{
    using stackWnd:  StackLayoutCtrl
    ed: *PropertyList
}

impl IWnd for PropertyListView
{
    mtd onResizeEvent(evt: *ResizeEvent)
    {
        stackWnd.IWnd.onResizeEvent(evt)

        if ed and ed.items.count
        {
            applyLayout()
            bk := ed.items.back()
            ed.scrollWnd.setScrollSize(0, bk.line.position.bottom())
        }
    }
}
