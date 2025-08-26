#global marked
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
        me.stackWnd.IWnd.onResizeEvent(evt)
        if !me.ed or !me.ed.items.count:
            return

        me.applyLayout()

        var lastItem: *PropertiesItem
        foreach item in me.ed.items
        {
            if item.label:
                item.label.autoHeight()
            if item.labelDesc:
                item.labelDesc.autoHeight()
            item.sigResize.call(item)
            item.line.applyLayout()
            item.line.sendResizeEvent()
            if item.line.isVisible():
                lastItem = item
        }

        me.applyLayout()

        // Vertical size is the bottom position of the last line
        if lastItem:
            me.ed.scrollWnd.setScrollSize(0, lastItem.line.position.bottom() + 10)
        else:
            me.ed.scrollWnd.setScrollSize(0, 0)
    }
}
