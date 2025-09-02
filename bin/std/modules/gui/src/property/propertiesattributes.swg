#global public
using Core, Pixel

impl Properties
{
    // Start a new category
    attr Category(name: string)

    // Change the field display name
    attr Name(name: string)

    // Add a description
    attr Description(desc: string)

    // The width of the edition widget
    attr EditSize(size: s32)

    // If the value changed, need to recompute the properties layout
    attr RecalcLayout()

    // Value edition with an editbox, min and max values
    attr MinMax(min, max: s64)

    // Value edition with an editbox, show a unit string
    attr Unit(value: string)

    // Float edition, edit width a slider instead of an editbox
    attr SliderMinMax(min, max: f32)

    // Color edition, force an opaque color
    attr Opaque()

    // Color edition, keep the original alpha value
    attr KeepAlpha()
}
