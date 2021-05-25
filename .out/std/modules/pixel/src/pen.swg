#global public
using Core, Drawing

enum PenStyle
{
    Solid
    Pattern
}

enum LineCapStyle
{
    None
    Triangle
    Round
    Square
    ArrowAnchor
}

enum DashCapStyle
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
    size:       f32 = 1
    color:      Color

    pattern:               Array'f32
    patternBeginCapStyle:  DashCapStyle = None
    patternEndCapStyle:    DashCapStyle = None

    jointMiterLimit: f32 = 3

    penStyle:       PenStyle        = Solid
    beginCapStyle:  LineCapStyle   = Round
    endCapStyle:    LineCapStyle   = Round
    jointStyle:     JointStyle      = Miter
}