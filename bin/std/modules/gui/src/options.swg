#global public
using Core, Serialization, Properties

struct ApplicationOptions
{
    #[Category("Debug"), Name("Show Clip Rect"), Description("Display an overlay of the current drawing clipping rectangle")]
    showClipRect: bool
}
