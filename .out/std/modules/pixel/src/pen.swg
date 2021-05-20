#global public
using Core, Drawing

enum CapStyle
{
    None
    Triangle
}

struct Pen
{
    size:  f32
    color: Color

    beginCapStyle:  CapStyle = Triangle
    endCapStyle:    CapStyle = Triangle
}