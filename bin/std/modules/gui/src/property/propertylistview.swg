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

        var lastItem: *PropertyListItem
        visit item: ed.items
        {
            if item.label
                item.label.autoHeight()
            if item.labelDesc
                item.labelDesc.autoHeight()
            item.onResize.call(item)
            item.line.sendResizeEvent()
            if item.line.isVisible()
                lastItem = item
        }

        applyLayout()

        // Vertical size is the bottom position of the last line
        if lastItem
            ed.scrollWnd.setScrollSize(0, lastItem.line.position.bottom() + 10)
        else
            ed.scrollWnd.setScrollSize(0, 0)
    }
}
