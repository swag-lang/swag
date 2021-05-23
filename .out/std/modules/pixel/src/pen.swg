#global public
using Core, Drawing

enum PenStyle
{
    Solid
}

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
    size:  f32 = 1
    color: Color

    jointMiterLimit: f32 = 3

    penStyle:       PenStyle   = Solid
    beginCapStyle:  CapStyle   = Round
    endCapStyle:    CapStyle   = Round
    jointStyle:     JointStyle = Miter
}