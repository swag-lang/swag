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
    SquareAnchor
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

    arrowAnchorScaleX:  f32 = 2
    arrowAnchorScaleY:  f32 = 2.0
    squareAnchorScaleX: f32 = 1.5
    squareAnchorScaleY: f32 = 2
    joinMiterLimit:     f32 = 3

    penStyle:       PenStyle     = Solid
    beginCapStyle:  LineCapStyle = Round
    endCapStyle:    LineCapStyle = Round
    joinStyle:      JoinStyle    = Miter
}