#global public
using Core.Drawing

#[Swag.enumindex]
enum BrushTextureType
{
    // No texturing. Solid rendering
    None

    // User specified texture/image
    Custom

    // Predefined hatch textures
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

enum BrushStretchMode
{
    Pixel
    Linear
}

struct Brush
{
    color:          Color = Argb.White
    textureType:    BrushTextureType = None
    texture:        Texture
    wrapMode:       WrapMode = Clamp
    stretchMode:    BrushStretchMode = Pixel
}