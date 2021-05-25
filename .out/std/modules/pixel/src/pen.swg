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

enum JoinStyle
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

    pattern:            Array'f32
    dashBeginCapStyle:  DashCapStyle = None
    dashEndCapStyle:    DashCapStyle = None

    joinMiterLimit: f32 = 3

    penStyle:       PenStyle     = Solid
    beginCapStyle:  LineCapStyle = Round
    endCapStyle:    LineCapStyle = Round
    joinStyle:      JoinStyle    = Miter
}