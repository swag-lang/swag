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
        if !ed or !ed.items.count
            return

        applyLayout()

        visit item: ed.items
        {
            if item.label
                item.label.autoHeight()
            if item.labelDesc
                item.labelDesc.autoHeight()
            item.onResize.call(item)
            item.line.sendResizeEvent()
        }

        applyLayout()

        // Vertical size is the bottom position of the last line
        bk := ed.items.back()
        ed.scrollWnd.setScrollSize(0, bk.line.position.bottom() + 10)
    }
}
