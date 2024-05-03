#global public
using Core, Serialization, PropertyList

struct ApplicationOptions
{
    #[PropertyList.Category("Debug")]
    #[Name("Show Clip Rect")]
    #[Description("Display an overlay of the current drawing clipping rectangle")]
    showClipRect: bool
}