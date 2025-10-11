using Core, Pixel

#[Swag.Opaque]
public struct PropertiesView
{
    using stackWnd:     StackLayoutCtrl
    ed:                 *Properties
}

impl IWnd for PropertiesView
{
    mtd impl onResizeEvent(evt: *ResizeEvent)
    {
        .stackWnd.IWnd.onResizeEvent(evt)
        if !.ed or !.ed.items.count do
            return

        .applyLayout()

        var lastItem: *PropertiesItem
        foreach item in .ed.items
        {
            if item.label do
                item.label.autoHeight()
            if item.labelDesc do
                item.labelDesc.autoHeight()
            item.sigResize.call(item)
            item.line.applyLayout()
            item.line.sendResizeEvent()
            if item.line.isVisible() do
                lastItem = item
        }

        .applyLayout()

        // Vertical size is the bottom position of the last line
        if lastItem do
            .ed.scrollWnd.setScrollSize(0, lastItem.line.position.bottom() + 10)
        else do
            .ed.scrollWnd.setScrollSize(0, 0)
    }
}
