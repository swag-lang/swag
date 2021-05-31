#global public
using Core, Drawing

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

enum PenStyle
{
    SolidColor
}

enum DashStyle
{
    None
    Pattern
    Dot
    Dash
    DashDot
}

struct Pen
{
    size:       f32 = 1
    brush:      Brush

    pattern:            Array'f32
    dashBeginCapStyle:  DashCapStyle = None
    dashEndCapStyle:    DashCapStyle = None

    arrowAnchorScaleX:  f32 = 2
    arrowAnchorScaleY:  f32 = 2.0
    squareAnchorScaleX: f32 = 1.5
    squareAnchorScaleY: f32 = 2
    joinMiterLimit:     f32 = 3

    penStyle:       PenStyle     = SolidColor
    dashStyle:      DashStyle    = None
    beginCapStyle:  LineCapStyle = Round
    endCapStyle:    LineCapStyle = Round
    joinStyle:      JoinStyle    = Miter
}

impl Pen
{
    // Returns a solid color pen
    func createSolid(color: Color, size: f32 = 1)->Pen
    {
        var result: retval
        result.brush.color = color
        result.brush.type = .SolidColor
        result.size = size
        return result
    }

    // Return a predefined hatch pen
    func createHatch(type: HatchType, color: Color, size: f32 = 1)->Pen
    {
        var result: retval
        result.brush.color = color
        result.brush.type = .Hatch
        result.brush.hatch = type
        result.size = size
        result.brush.uvMode = .Tile
        return result
    }
}