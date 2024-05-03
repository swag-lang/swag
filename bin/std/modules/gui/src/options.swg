#global public
using Core, Serialization, PropertyList

#[PropertyList.Category("Debug")]
struct Options
{
    #[Name("Show Clip Rect")]
    #[Description("Display an overlay of the current drawing clipping rectangle")]
    showClipRect: bool
}