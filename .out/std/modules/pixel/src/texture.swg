#global public

struct Texture
{
    handle:        TextureHandle
    pf:            PixelFormat = ?
    width, height: s32
}

enum InterpolationMode
{
    Pixel
    Linear
}

enum UVMode
{
    Fill
    FillSubRect
    Tile
}

#[Swag.EnumIndex]
enum HatchStyle
{
    Dot
    Horizontal
    HorizontalLight
    HorizontalNarrow
    Vertical
    VerticalLight
    VerticalNarrow
    SolidDiamond
    OutlinedDiamond
    HorizontalDark
    VerticalDark
    Grid
    GridLarge
    GridDotted
    Cross
    CrossLarge
    Percent5
    Percent10
    Percent20
    Percent25
    Percent30
    Percent40
    Percent50
    Percent60
    Percent70
    Percent75
    Percent80
    Percent90
    CheckBoard
    CheckBoardLarge
    Weave
    DiagonalForward
    DiagonalForwardLight
    DiagonalBackward
    DiagonalBackwardLight
    DiagonalCross
}