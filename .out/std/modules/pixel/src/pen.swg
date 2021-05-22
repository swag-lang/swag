#global public
using Core, Drawing

enum CapStyle
{
    None
    Triangle
    Round
    Square
}

enum JointStyle
{
    None
    Bevel
    Round
    Miter
    MiterBevel
}

struct Pen
{
    size:  f32
    color: Color

    jointMiterLimit:    f32 = 3
    beginCapStyle:      CapStyle = Round
    endCapStyle:        CapStyle = Round
    jointStyle:         JointStyle = Miter
}