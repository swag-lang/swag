#global public
using Core, Ogl, Core.Math

#[Swag.Strict]
alias TextureHandle = uint
const InvalidTextureHandle: TextureHandle = acast 0

private const HatchDot: [64] u8 = @[
    1, 0, 1, 0, 1, 0, 1, 0,
    0, 1, 0, 1, 0, 1, 0, 1,
    1, 0, 1, 0, 1, 0, 1, 0,
    0, 1, 0, 1, 0, 1, 0, 1,
    1, 0, 1, 0, 1, 0, 1, 0,
    0, 1, 0, 1, 0, 1, 0, 1,
    1, 0, 1, 0, 1, 0, 1, 0,
    0, 1, 0, 1, 0, 1, 0, 1,
]

private const HatchHorizontal: [64] u8 = @[
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
]

private const HatchHorizontalLight: [64] u8 = @[
    1, 1, 1, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
]

private const HatchHorizontalNarrow: [64] u8 = @[
    1, 1, 1, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 0,
]

private const HatchVertical: [64] u8 = @[
    0, 0, 0, 1, 0, 0, 0, 0,
    0, 0, 0, 1, 0, 0, 0, 0,
    0, 0, 0, 1, 0, 0, 0, 0,
    0, 0, 0, 1, 0, 0, 0, 0,
    0, 0, 0, 1, 0, 0, 0, 0,
    0, 0, 0, 1, 0, 0, 0, 0,
    0, 0, 0, 1, 0, 0, 0, 0,
    0, 0, 0, 1, 0, 0, 0, 0,
]

private const HatchVerticalLight: [64] u8 = @[
    1, 0, 0, 0, 1, 0, 0, 0,
    1, 0, 0, 0, 1, 0, 0, 0,
    1, 0, 0, 0, 1, 0, 0, 0,
    1, 0, 0, 0, 1, 0, 0, 0,
    1, 0, 0, 0, 1, 0, 0, 0,
    1, 0, 0, 0, 1, 0, 0, 0,
    1, 0, 0, 0, 1, 0, 0, 0,
    1, 0, 0, 0, 1, 0, 0, 0,
]

private const HatchVerticalNarrow: [64] u8 = @[
    1, 0, 1, 0, 1, 0, 1, 0,
    1, 0, 1, 0, 1, 0, 1, 0,
    1, 0, 1, 0, 1, 0, 1, 0,
    1, 0, 1, 0, 1, 0, 1, 0,
    1, 0, 1, 0, 1, 0, 1, 0,
    1, 0, 1, 0, 1, 0, 1, 0,
    1, 0, 1, 0, 1, 0, 1, 0,
    1, 0, 1, 0, 1, 0, 1, 0,
]

private const HatchSolidDiamond: [64] u8 = @[
    1, 1, 1, 0, 0, 1, 1, 1,
    1, 1, 0, 0, 0, 0, 1, 1,
    1, 0, 0, 0, 0, 0, 0, 1,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    1, 0, 0, 0, 0, 0, 0, 1,
    1, 1, 0, 0, 0, 0, 1, 1,
    1, 1, 1, 0, 0, 1, 1, 1,
]

private const HatchOutlinedDiamond: [64] u8 = @[
    0, 0, 1, 0, 0, 1, 0, 0,
    0, 1, 0, 0, 0, 0, 1, 0,
    1, 0, 0, 0, 0, 0, 0, 1,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    1, 0, 0, 0, 0, 0, 0, 1,
    0, 1, 0, 0, 0, 0, 1, 0,
    0, 0, 1, 0, 0, 1, 0, 0,
]

private const HatchHorizontalDark: [64] u8 = @[
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
]

private const HatchVerticalDark: [64] u8 = @[
    1, 1, 0, 0, 1, 1, 0, 0,
    1, 1, 0, 0, 1, 1, 0, 0,
    1, 1, 0, 0, 1, 1, 0, 0,
    1, 1, 0, 0, 1, 1, 0, 0,
    1, 1, 0, 0, 1, 1, 0, 0,
    1, 1, 0, 0, 1, 1, 0, 0,
    1, 1, 0, 0, 1, 1, 0, 0,
    1, 1, 0, 0, 1, 1, 0, 0,
]

private const HatchGrid: [64] u8 = @[
    0, 1, 0, 0, 0, 1, 0, 0,
    1, 1, 1, 1, 1, 1, 1, 1,
    0, 1, 0, 0, 0, 1, 0, 0,
    0, 1, 0, 0, 0, 1, 0, 0,
    0, 1, 0, 0, 0, 1, 0, 0,
    1, 1, 1, 1, 1, 1, 1, 1,
    0, 1, 0, 0, 0, 1, 0, 0,
    0, 1, 0, 0, 0, 1, 0, 0,
]

private const HatchGridLarge: [64] u8 = @[
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 0, 0, 0, 0, 0, 0, 0,
    1, 0, 0, 0, 0, 0, 0, 0,
    1, 0, 0, 0, 0, 0, 0, 0,
    1, 0, 0, 0, 0, 0, 0, 0,
    1, 0, 0, 0, 0, 0, 0, 0,
    1, 0, 0, 0, 0, 0, 0, 0,
    1, 0, 0, 0, 0, 0, 0, 0,
]

private const HatchGridDotted: [64] u8 = @[
    1, 0, 1, 0, 1, 0, 1, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    1, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    1, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    1, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
]

private const HatchCross: [64] u8 = @[
    0, 1, 0, 0, 0, 1, 0, 0,
    1, 1, 1, 0, 1, 1, 1, 0,
    0, 1, 0, 0, 0, 1, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 1, 0, 0, 0, 1, 0, 0,
    1, 1, 1, 0, 1, 1, 1, 0,
    0, 1, 0, 0, 0, 1, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
]

private const HatchCrossLarge: [64] u8 = @[
    0, 0, 1, 0, 0, 0, 0, 0,
    0, 0, 1, 0, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 0, 0, 0,
    0, 0, 1, 0, 0, 0, 0, 0,
    0, 0, 1, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
]

private const HatchPercent5: [64] u8 = @[
    0, 0, 0, 0, 1, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    1, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
]

private const HatchPercent10: [64] u8 = @[
    1, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 1, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    1, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 1, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
]

private const HatchPercent20: [64] u8 = @[
    1, 0, 0, 0, 1, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 1, 0, 0, 0, 1, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    1, 0, 0, 0, 1, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 1, 0, 0, 0, 1, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
]

private const HatchPercent25: [64] u8 = @[
    0, 0, 1, 0, 0, 0, 1, 0,
    1, 0, 0, 0, 1, 0, 0, 0,
    0, 0, 1, 0, 0, 0, 1, 0,
    1, 0, 0, 0, 1, 0, 0, 0,
    0, 0, 1, 0, 0, 0, 1, 0,
    1, 0, 0, 0, 1, 0, 0, 0,
    0, 0, 1, 0, 0, 0, 1, 0,
    1, 0, 0, 0, 1, 0, 0, 0,
]

private const HatchPercent30: [64] u8 = @[
    1, 0, 1, 0, 1, 0, 1, 0,
    0, 0, 0, 1, 0, 0, 0, 1,
    1, 0, 1, 0, 1, 0, 1, 0,
    0, 1, 0, 0, 0, 1, 0, 0,
    1, 0, 1, 0, 1, 0, 1, 0,
    0, 0, 0, 1, 0, 0, 0, 1,
    1, 0, 1, 0, 1, 0, 1, 0,
    0, 1, 0, 0, 0, 1, 0, 0,
]

private const HatchPercent40: [64] u8 = @[
    0, 0, 0, 1, 0, 1, 0, 1,
    1, 0, 1, 0, 1, 0, 1, 0,
    0, 1, 0, 1, 0, 1, 0, 1,
    1, 0, 1, 0, 1, 0, 1, 0,
    0, 1, 0, 1, 0, 0, 0, 1,
    1, 0, 1, 0, 1, 0, 1, 0,
    0, 1, 0, 1, 0, 1, 0, 1,
    1, 0, 1, 0, 1, 0, 1, 0,
]

private const HatchPercent50: [64] u8 = @[
    1, 0, 1, 0, 1, 0, 1, 0,
    0, 1, 0, 1, 0, 1, 0, 1,
    1, 0, 1, 0, 1, 0, 1, 0,
    0, 1, 0, 1, 0, 1, 0, 1,
    1, 0, 1, 0, 1, 0, 1, 0,
    0, 1, 0, 1, 0, 1, 0, 1,
    1, 0, 1, 0, 1, 0, 1, 0,
    0, 1, 0, 1, 0, 1, 0, 1,
]

private const HatchPercent60: [64] u8 = @[
    0, 1, 0, 1, 0, 1, 0, 1,
    1, 0, 1, 1, 1, 0, 1, 1,
    0, 1, 0, 1, 0, 1, 0, 1,
    1, 1, 1, 0, 1, 1, 1, 0,
    0, 1, 0, 1, 0, 1, 0, 1,
    1, 0, 1, 1, 1, 0, 1, 1,
    0, 1, 0, 1, 0, 1, 0, 1,
    1, 1, 1, 0, 1, 1, 1, 0,
]

private const HatchPercent70: [64] u8 = @[
    0, 1, 1, 1, 0, 1, 1, 1,
    1, 1, 0, 1, 1, 1, 0, 1,
    0, 1, 1, 1, 0, 1, 1, 1,
    1, 1, 0, 1, 1, 1, 0, 1,
    0, 1, 1, 1, 0, 1, 1, 1,
    1, 1, 0, 1, 1, 1, 0, 1,
    0, 1, 1, 1, 0, 1, 1, 1,
    1, 1, 0, 1, 1, 1, 0, 1,
]

private const HatchPercent75: [64] u8 = @[
    1, 1, 1, 1, 1, 1, 1, 1,
    0, 1, 1, 1, 0, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 0, 1, 1, 1, 0, 1,
    1, 1, 1, 1, 1, 1, 1, 1,
    0, 1, 1, 1, 0, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 0, 1, 1, 1, 0, 1,
]

private const HatchPercent80: [64] u8 = @[
    1, 1, 0, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 0, 1,
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 0, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 0, 1,
    1, 1, 1, 1, 1, 1, 1, 1,
]

private const HatchPercent90: [64] u8 = @[
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1,
    0, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 0, 1, 1, 1,
]

private const HatchCheckBoard: [64] u8 = @[
    1, 1, 0, 0, 1, 1, 0, 0,
    0, 0, 1, 1, 0, 0, 1, 1,
    0, 0, 1, 1, 0, 0, 1, 1,
    1, 1, 0, 0, 1, 1, 0, 0,
    1, 1, 0, 0, 1, 1, 0, 0,
    0, 0, 1, 1, 0, 0, 1, 1,
    0, 0, 1, 1, 0, 0, 1, 1,
    1, 1, 0, 0, 1, 1, 0, 0,
]

private const HatchCheckBoardLarge: [64] u8 = @[
    1, 0, 0, 0, 0, 1, 1, 1,
    1, 0, 0, 0, 0, 1, 1, 1,
    1, 0, 0, 0, 0, 1, 1, 1,
    0, 1, 1, 1, 1, 0, 0, 0,
    0, 1, 1, 1, 1, 0, 0, 0,
    0, 1, 1, 1, 1, 0, 0, 0,
    0, 1, 1, 1, 1, 0, 0, 0,
    1, 0, 0, 0, 0, 1, 1, 1,
]

private const HatchWeave: [64] u8 = @[
    0, 1, 0, 0, 0, 1, 0, 0,
    1, 0, 1, 0, 0, 0, 1, 0,
    0, 0, 0, 1, 0, 0, 0, 1,
    0, 0, 1, 0, 1, 0, 1, 0,
    0, 1, 0, 0, 0, 1, 0, 0,
    1, 0, 1, 0, 0, 0, 0, 0,
    0, 0, 0, 1, 0, 0, 0, 1,
    1, 0, 0, 0, 1, 0, 1, 0,
]

private const HatchDiagonalForward: [64] u8 = @[
    0, 0, 0, 0, 0, 0, 0, 1,
    0, 0, 0, 0, 0, 0, 1, 0,
    0, 0, 0, 0, 0, 1, 0, 0,
    0, 0, 0, 0, 1, 0, 0, 0,
    0, 0, 0, 1, 0, 0, 0, 0,
    0, 0, 1, 0, 0, 0, 0, 0,
    0, 1, 0, 0, 0, 0, 0, 0,
    1, 0, 0, 0, 0, 0, 0, 0,
]

private const HatchDiagonalForwardLight: [64] u8 = @[
    0, 0, 0, 1, 0, 0, 0, 1,
    0, 0, 1, 0, 0, 0, 1, 0,
    0, 1, 0, 0, 0, 1, 0, 0,
    1, 0, 0, 0, 1, 0, 0, 0,
    0, 0, 0, 1, 0, 0, 0, 1,
    0, 0, 1, 0, 0, 0, 1, 0,
    0, 1, 0, 0, 0, 1, 0, 0,
    1, 0, 0, 0, 1, 0, 0, 0,
]

private const HatchDiagonalBackward: [64] u8 = @[
    1, 0, 0, 0, 0, 0, 0, 0,
    0, 1, 0, 0, 0, 0, 0, 0,
    0, 0, 1, 0, 0, 0, 0, 0,
    0, 0, 0, 1, 0, 0, 0, 0,
    0, 0, 0, 0, 1, 0, 0, 0,
    0, 0, 0, 0, 0, 1, 0, 0,
    0, 0, 0, 0, 0, 0, 1, 0,
    0, 0, 0, 0, 0, 0, 0, 1,
]

private const HatchDiagonalBackwardLight: [64] u8 = @[
    1, 0, 0, 0, 1, 0, 0, 0,
    0, 1, 0, 0, 0, 1, 0, 0,
    0, 0, 1, 0, 0, 0, 1, 0,
    0, 0, 0, 1, 0, 0, 0, 1,
    1, 0, 0, 0, 1, 0, 0, 0,
    0, 1, 0, 0, 0, 1, 0, 0,
    0, 0, 1, 0, 0, 0, 1, 0,
    0, 0, 0, 1, 0, 0, 0, 1,
]

private const HatchDiagonalCross: [64] u8 = @[
    1, 0, 0, 0, 0, 0, 0, 1,
    0, 1, 0, 0, 0, 0, 1, 0,
    0, 0, 1, 0, 0, 1, 0, 0,
    0, 0, 0, 1, 1, 0, 0, 0,
    0, 0, 0, 1, 1, 0, 0, 0,
    0, 0, 1, 0, 0, 1, 0, 0,
    0, 1, 0, 0, 0, 0, 1, 0,
    1, 0, 0, 0, 0, 0, 0, 1,
]

public struct RenderOgl
{
    vertexbuffer:       GLuint
    shaderImage:        GLuint
    width, height:      s32
    worldTransform:     Transform2
    modelMatrix:        Matrix4x4
    whiteTexture:       GLuint
    hatchTextures:      [@countof(HatchStyle)] GLuint
}

impl RenderOgl
{
    private func generateTexture(datas: const [..] u8, w, h: s32)->GLuint
    {
        Debug.assert(@countof(datas) <= 8*8)

        var arr: [64] u32
        visit p, i: datas arr[i] = p ? 0xFFFFFFFF : 0

        var result: GLuint
        glGenTextures(1, &result)
        glBindTexture(GL_TEXTURE_2D, result)
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, &arr[0])
        return result
    }

    protected func init(using self, w, h: s32)
    {
        width = w
        height = h

        glInitExtensions()
        createShaders()

        glGenBuffers(1, &vertexbuffer)
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer)

        // Vertex buffer layout
        var offset: const *void = null
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, @sizeof(VertexLayout), offset)
        offset += @sizeof(Vector2) + @sizeof(f32)
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, @sizeof(VertexLayout), offset)
        offset += @sizeof(Vector2)
        glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, @sizeof(VertexLayout), offset)
        offset += @sizeof(u32)
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, @sizeof(VertexLayout), offset)
        offset += @sizeof(Vector4)
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, @sizeof(VertexLayout), offset)
        offset += @sizeof(Vector4)
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, @sizeof(VertexLayout), offset)
        offset += @sizeof(Vector4)
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, @sizeof(VertexLayout), offset)
        offset += @sizeof(Vector4)
        glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, @sizeof(VertexLayout), offset)
        offset += @sizeof(Vector4)
        glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, @sizeof(VertexLayout), offset)
        offset += @sizeof(Vector4)
        glVertexAttribPointer(9, 1, GL_FLOAT, GL_FALSE, @sizeof(VertexLayout), offset)

        // Generate predefined textures
        whiteTexture = generateTexture(@[1], 1, 1)
        using HatchStyle
        hatchTextures[Dot] = generateTexture(HatchDot, 8, 8)
        hatchTextures[Horizontal] = generateTexture(HatchHorizontal, 8, 8)
        hatchTextures[HorizontalLight] = generateTexture(HatchHorizontalLight, 8, 8)
        hatchTextures[HorizontalNarrow] = generateTexture(HatchHorizontalNarrow, 8, 8)
        hatchTextures[Vertical] = generateTexture(HatchVertical, 8, 8)
        hatchTextures[VerticalLight] = generateTexture(HatchVerticalLight, 8, 8)
        hatchTextures[VerticalNarrow] = generateTexture(HatchVerticalNarrow, 8, 8)
        hatchTextures[SolidDiamond] = generateTexture(HatchSolidDiamond, 8, 8)
        hatchTextures[OutlinedDiamond] = generateTexture(HatchOutlinedDiamond, 8, 8)
        hatchTextures[HorizontalDark] = generateTexture(HatchHorizontalDark, 8, 8)
        hatchTextures[VerticalDark] = generateTexture(HatchVerticalDark, 8, 8)
        hatchTextures[Grid] = generateTexture(HatchGrid, 8, 8)
        hatchTextures[GridLarge] = generateTexture(HatchGridLarge, 8, 8)
        hatchTextures[GridDotted] = generateTexture(HatchGridDotted, 8, 8)
        hatchTextures[Cross] = generateTexture(HatchCross, 8, 8)
        hatchTextures[CrossLarge] = generateTexture(HatchCrossLarge, 8, 8)
        hatchTextures[Percent5] = generateTexture(HatchPercent5, 8, 8)
        hatchTextures[Percent10] = generateTexture(HatchPercent10, 8, 8)
        hatchTextures[Percent20] = generateTexture(HatchPercent20, 8, 8)
        hatchTextures[Percent25] = generateTexture(HatchPercent25, 8, 8)
        hatchTextures[Percent30] = generateTexture(HatchPercent30, 8, 8)
        hatchTextures[Percent40] = generateTexture(HatchPercent40, 8, 8)
        hatchTextures[Percent50] = generateTexture(HatchPercent50, 8, 8)
        hatchTextures[Percent60] = generateTexture(HatchPercent60, 8, 8)
        hatchTextures[Percent70] = generateTexture(HatchPercent70, 8, 8)
        hatchTextures[Percent75] = generateTexture(HatchPercent75, 8, 8)
        hatchTextures[Percent80] = generateTexture(HatchPercent80, 8, 8)
        hatchTextures[Percent90] = generateTexture(HatchPercent90, 8, 8)
        hatchTextures[CheckBoard] = generateTexture(HatchCheckBoard, 8, 8)
        hatchTextures[CheckBoardLarge] = generateTexture(HatchCheckBoardLarge, 8, 8)
        hatchTextures[Weave] = generateTexture(HatchWeave, 8, 8)
        hatchTextures[DiagonalForward] = generateTexture(HatchDiagonalForward, 8, 8)
        hatchTextures[DiagonalForwardLight] = generateTexture(HatchDiagonalForwardLight, 8, 8)
        hatchTextures[DiagonalBackward] = generateTexture(HatchDiagonalBackward, 8, 8)
        hatchTextures[DiagonalBackwardLight] = generateTexture(HatchDiagonalBackwardLight, 8, 8)
        hatchTextures[DiagonalCross] = generateTexture(HatchDiagonalCross, 8, 8)
    }

    protected func start(using self)
    {
        worldTransform.setIdentity()
        glClearStencil(0)
        glClearColor(0, 0, 0, 0)
        glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT)
    }

    func resize(using self, width, height: s32)
    {
        self.width = #scopefct.width
        self.height = #scopefct.height
    }

    private func ortho(b, t, l, r, n, f: f32, m: [4,4] f32)
    {
        m[0,0] = 2 / (r - l);
        m[0,1] = 0;
        m[0,2] = 0;
        m[0,3] = 0;

        m[1,0] = 0;
        m[1,1] = 2 / (t - b);
        m[1,2] = 0;
        m[1,3] = 0;

        m[2,0] = 0;
        m[2,1] = 0;
        m[2,2] = -2 / (f - n);
        m[2,3] = 0;

        m[3,0] = -(r + l) / (r - l);
        m[3,1] = -(t + b) / (t - b);
        m[3,2] = -(f + n) / (f - n);
        m[3,3] = 1;
    }

    private func buidModelMatrix(using self, modelTransform: Transform2)
    {
        tr := worldTransform
        tr.multiply(modelTransform)

        // Received matrix is in row major order, so transpose it

        // x
        modelMatrix.m[0, 0] = tr.m11
        modelMatrix.m[0, 1] = tr.m12

        // y
        modelMatrix.m[1, 0] = tr.m21
        modelMatrix.m[1, 1] = tr.m22

        // z
        modelMatrix.m[2, 2] = 1

        // translation
        modelMatrix.m[3, 0] = tr.m31
        modelMatrix.m[3, 1] = tr.m32
    }

    // Register an image for rendering
    func addImage(using self, image: Image)->Texture
    {
        var fmt: GLenum = 0
        internal := GL_RGB

        #[Swag.Complete]
        switch image.pf
        {
        case BGR8:  fmt = GL_BGR
        case BGRA8: fmt = GL_BGRA; internal = GL_RGBA
        case RGB8:  fmt = GL_RGB
        case RGBA8: fmt = GL_RGBA; internal = GL_RGBA
        }

        var textureID: GLuint
        glGenTextures(1, &textureID)
        glBindTexture(GL_TEXTURE_2D, textureID)
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1)
        glTexImage2D(GL_TEXTURE_2D, 0, internal, cast(s32) image.width, cast(s32) image.height, 0, fmt, GL_UNSIGNED_BYTE, image.pixels)

        var result: retval
        result.handle = cast(TextureHandle) textureID
        result.width = image.width
        result.height = image.height
        result.pf = image.pf
        return result
    }

    // Translate the world transform
    func translateTransform(using self, x, y: f32)
    {
        tr := Transform2.createTranslation(x, y)
        worldTransform.multiply(tr)
    }

    // Rotate the world transform
    func rotateTransform(using self, angle: f32)
    {
        rad := Math.toRadians(angle)
        tr := Transform2.createRotation(rad, 0, 0)
        worldTransform.multiply(tr)
    }

    // Rotate the world transform
    func rotateTransform(using self, angle: f32, centerX, centerY: f32)
    {
        rad := Math.toRadians(angle)
        tr := Transform2.createRotation(rad, centerX, centerY)
        worldTransform.multiply(tr)
    }

    // Scale the world transform
    func scaleTransform(using self, x, y: f32)
    {
        tr := Transform2.createScale(x, y, 0, 0)
        worldTransform.multiply(tr)
    }

    // Reset the world transform
    func resetTransform(using self)
    {
        worldTransform.setIdentity()
        modelMatrix.setIdentity()
    }

    // Draw the given painter
    func draw(using self, dc: *Painter)
    {
        if height == 0 or width == 0
            return

        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer)
        glBufferData(GL_ARRAY_BUFFER, dc.vertexBuffer.count * @sizeof(VertexLayout), dc.vertexBuffer.buffer, GL_STATIC_DRAW);

        glUseProgram(shaderImage)

        // View, ortho projection
        mvp := glGetUniformLocation(shaderImage, @dataof("mvp"))
        var orthoMatrix: Matrix4x4
        ortho(cast(f32) height, 0, 0, cast(f32) width, 0, 1.0, orthoMatrix.m)
        glUniformMatrix4fv(mvp, 1, GL_FALSE, &orthoMatrix.m[0, 0])
	    glViewport(0, 0, width, height)

        // Draw matrix
        mdl := glGetUniformLocation(shaderImage, @dataof("mdl"))
        buidModelMatrix(Transform2.Identity)
        glUniformMatrix4fv(mdl, 1, GL_FALSE, &modelMatrix.m[0, 0])

        // UV parameters
        uvmode      := glGetUniformLocation(shaderImage, @dataof("uvmode"))
        boundRect   := glGetUniformLocation(shaderImage, @dataof("boundRect"))
        textureRect := glGetUniformLocation(shaderImage, @dataof("textureRect"))

        // Default state
        glEnable(GL_BLEND)
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)
        glBindTexture(GL_TEXTURE_2D, whiteTexture)
        glDisable(GL_SCISSOR_TEST)
        glDisable(GL_STENCIL_TEST)
        glDisable(GL_DEPTH_TEST)
        glClearStencil(0)
        glStencilMask(0xFF)
        glDepthMask(GL_FALSE)

        glActiveTexture(GL_TEXTURE0)
        glBindTexture(GL_TEXTURE_2D, whiteTexture)
        glActiveTexture(GL_TEXTURE1)
        glBindTexture(GL_TEXTURE_2D, whiteTexture)

        loop i: 10 glEnableVertexAttribArray(cast(GLuint) i)

        transparencyMode := false
        visit cmd: dc.commandBuffer
        {
            switch cmd.id
            {
            case Clear:
                params := &cmd.params.clear
                (r, g, b) := params.color.toRgbf()
                glClearColor(r, g, b, 0)
                glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT)

            case Transform:
                params := &cmd.params.transform
                buidModelMatrix(params.tr)
                glUniformMatrix4fv(mdl, 1, GL_FALSE, &modelMatrix.m[0, 0])

            case StartClippingRegion:
                params := &cmd.params.clippingRegion
                glEnable(GL_STENCIL_TEST)
                glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE)
                glStencilFunc(GL_ALWAYS, 1, 0xFF)
                fallthrough

            case SetClippingRegionMode:
                params := &cmd.params.clippingRegion
                mode := params.mode == .Set ? GL_REPLACE : cast(GLenum) GL_ZERO
                glStencilOp(mode, mode, mode)

            case EndClippingRegion:
                glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE)
                glStencilFunc(GL_EQUAL, 1, 0xFF)
                glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP)

            case ResetClippingRegion:
                glDisable(GL_STENCIL_TEST)

            case SetClippingRect:
                params := &cmd.params.clippingRect
                glEnable(GL_SCISSOR_TEST)
                glScissor(cast(GLint) params.rect.x, cast(GLint) (self.height - params.rect.y - params.rect.height), cast(GLint) params.rect.width, cast(GLint) params.rect.height)

            case ResetClippingRect:
                glDisable(GL_SCISSOR_TEST)

            case StartTransparency:
                glEnable(GL_DEPTH_TEST)
                glDepthMask(GL_TRUE)
                glClear(GL_DEPTH_BUFFER_BIT)
                transparencyMode = true

            case StopTransparency:
                glDisable(GL_DEPTH_TEST)
                transparencyMode = false

            case DrawTriangles:
                params := &cmd.params.drawTriangles
                if !transparencyMode
                {
                    glDrawArrays(GL_TRIANGLES, cast(GLint) params.start, cast(GLsizei) params.count)
                }
                else
                {
                    // First only write to the depth buffer. Each triangle should have
                    // a different Z value to avoid overwritting each others
                    glDepthMask(GL_TRUE)
                    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE)
                    glDrawArrays(GL_TRIANGLES, cast(GLint) params.start, cast(GLsizei) params.count)

                    // Then write to the color buffer by using the Z buffer
                    glDepthMask(GL_FALSE)
                    glDepthFunc(GL_LEQUAL)
                    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE)
                    glDrawArrays(GL_TRIANGLES, cast(GLint) params.start, cast(GLsizei) params.count)
                }

            case UpdateFont:
                font := cmd.params.font.fontRef
                font.dirty = false
                visit *atlas: font.atlases
                {
                    if !atlas.dirty continue
                    atlas.dirty = false
                    if cast(u32) atlas.texture.handle == 0
                        glGenTextures(1, cast(*GLuint) &atlas.texture.handle)
                    atlas.texture.width = atlas.width / atlas.bpp
                    atlas.texture.height = atlas.height
                    atlas.texture.pf = atlas.bpp == 1 ? .RGBA8 : .RGB8
                    glBindTexture(GL_TEXTURE_2D, cast(GLuint) atlas.texture.handle)
                    glPixelStorei(GL_UNPACK_ALIGNMENT, 1)
                    fmt := atlas.bpp == 1 ? GL_RED : GL_RGB
                    glTexImage2D(GL_TEXTURE_2D, 0, fmt, cast(GLsizei) atlas.texture.width, cast(GLsizei) atlas.texture.height, 0, fmt, GL_UNSIGNED_BYTE, atlas.datas.buffer)
                }

            case ResetTexture0:
                glActiveTexture(GL_TEXTURE0)
                glBindTexture(GL_TEXTURE_2D, whiteTexture)

            case ResetTexture1:
                glActiveTexture(GL_TEXTURE1)
                glBindTexture(GL_TEXTURE_2D, whiteTexture)

            case BindTexture0:
                params := &cmd.params.texture0
                glUniform4fv(boundRect, 1, &params.boundRect.x)
                glUniform4fv(textureRect, 1, &params.textureRect.x)
                glUniform1f(uvmode, cast(GLfloat) params.uvMode)
                glActiveTexture(GL_TEXTURE0)

                #[Swag.Complete]
                switch params.type
                {
                case SolidColor:
                    glBindTexture(GL_TEXTURE_2D, whiteTexture)
                case Texture:
                    glBindTexture(GL_TEXTURE_2D, cast(GLuint) params.texture.handle)
                case Hatch:
                    glBindTexture(GL_TEXTURE_2D, hatchTextures[params.hatch])
                }

                #[Swag.Complete]
                switch params.uvMode
                {
                case Fill, FillSubRect:
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE)
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE)
                case Tile:
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT)
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT)
                }

                #[Swag.Complete]
                switch params.interpolationMode
                {
                case Pixel:
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST)
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST)
                case Linear:
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR)
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR)
                }

            case BindTexture1:
                params := &cmd.params.texture1
                glActiveTexture(GL_TEXTURE1)
                glBindTexture(GL_TEXTURE_2D, cast(GLuint) params.texture.handle)
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE)
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE)
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST)
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST)
            }
        }

        loop i: 10 glDisableVertexAttribArray(cast(GLuint) i)
    }
}