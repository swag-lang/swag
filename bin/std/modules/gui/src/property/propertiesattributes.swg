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

    // Min and max values
    attr MinMax(min, max: s64)

    // Native float edit width a slider
    attr SliderMinMax(min, max: f32)

    // Editbox with a unit text
    attr Unit(value: string)

    // Value edit size
    attr EditSize(size: s32)

    // No Alpha edition in a color
    attr Opaque()    

    // If the value changed, need to recompute the properties layout
    attr RecalcLayout()
}
