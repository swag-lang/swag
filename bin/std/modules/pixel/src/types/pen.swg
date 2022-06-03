#global public
using Core

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
    plotOffset:         f32

    dashStyle:      DashStyle    = None
    beginCapStyle:  LineCapStyle = Round
    endCapStyle:    LineCapStyle = Round
    joinStyle:      JoinStyle    = Miter
}

impl Pen
{
    func getCapStyleExtent(cap: LineCapStyle, borderSize: f32)->f32
    {
        #[Swag.Complete]
        switch cap
        {
        case None:
            return 0
        case Triangle, Round, Square:
            return borderSize * 0.5
        case ArrowAnchor, SquareAnchor:
            return borderSize
        }

        return 0
    }

    // Returns a solid color pen
    func createSolid(color: Color, size: f32 = 1)->Pen
    {
        var result: retval
        result.brush.color = color
        result.brush.type = .SolidColor
        result.size = size
        return result
    }

    // Returns a dash pen
    func createDash(style: DashStyle, color: Color, size: f32 = 1)->Pen
    {
        var result: retval
        result.brush.color = color
        result.brush.type = .SolidColor
        result.dashStyle = style
        result.size = size
        return result
    }

    // Return a predefined hatch pen
    func createHatch(type: HatchStyle, color: Color, size: f32 = 1)->Pen
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