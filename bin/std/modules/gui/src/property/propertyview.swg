using Core, Pixel

#[Swag.Opaque]
public struct PropertyView
{
    using stackWnd:  StackLayoutCtrl
    ed: *PropertyCtrl
}

impl IWnd for PropertyView
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

