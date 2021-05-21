#global public
using Core, Drawing

enum CapStyle
{
    None
    Triangle
    Round
    Square
}

struct Pen
{
    size:  f32
    color: Color

    beginCapStyle:  CapStyle = Round
    endCapStyle:    CapStyle = Square
}