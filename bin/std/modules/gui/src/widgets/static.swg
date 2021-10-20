struct Static
{
    using base: Widget
}

impl IWnd for Static
{
    mtd onPaintEvent(evt: *PaintEvent)
    {
    }
}