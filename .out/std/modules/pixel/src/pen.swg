#global public
using Core, Drawing

enum CapStyle
{
    None
    Triangle
    Round
}

struct Pen
{
    size:  f32
    color: Color

    beginCapStyle:  CapStyle = Round
    endCapStyle:    CapStyle = Triangle
}