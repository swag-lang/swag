#global public
using Core, Pixel

impl PropertyList
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
}
