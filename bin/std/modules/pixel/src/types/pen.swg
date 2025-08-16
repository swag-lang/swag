#global public
using Core

enum LineCapStyle
{
    None
    Triangle
    Round
    Square
    ArrowAnchor
    ArrowAnchor2
    SquareAnchor
    RoundAnchor
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

enum DashStyle
{
    None
    Pattern
    Dot
    Dash
    DashDot
}

enum BorderPos
{
    None
    Inside
}

struct Pen
{
    size:                f32 = 1
    brush:               Brush

    pattern:             Array'f32
    dashBegCapStyle:     DashCapStyle = None
    dashEndCapStyle:     DashCapStyle = None
    borderPos:           BorderPos = None

    capBegScaleX:        f32 = 2
    capBegScaleY:        f32 = 2
    capEndScaleX:        f32 = 2
    capEndScaleY:        f32 = 2

    joinMiterLimit:      f32 = 3
    plotOffset:          f32

    dashStyle:           DashStyle = None
    dashBlankScale:      f32 = 1.0
    begCapStyle:         LineCapStyle = Round
    endCapStyle:         LineCapStyle = Round
    joinStyle:           JoinStyle = Miter
}

impl Pen
{
    // Returns a solid color pen
    func createSolid(color: Color, size: f32 = 1)->Pen
    {
        var result: retval
        result.brush.color = color
        result.brush.type  = .SolidColor
        result.size        = size
        return result
    }

    // Returns a dash pen
    func createDash(style: DashStyle, color: Color, size: f32 = 1)->Pen
    {
        var result: retval
        result.brush.color = color
        result.brush.type  = .SolidColor
        result.dashStyle   = style
        result.size        = size
        return result
    }

    // Return a predefined hatch pen
    func createHatch(type: HatchStyle, color: Color, size: f32 = 1)->Pen
    {
        var result: retval
        result.brush.color  = color
        result.brush.type   = .Hatch
        result.brush.hatch  = type
        result.size         = size
        result.brush.uvMode = .Tile
        return result
    }
}
