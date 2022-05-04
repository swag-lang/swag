#global public
using Core, Ogl, Core.Math

#[Swag.Strict]
alias ShaderHandle = uint
const InvalidShaderHandle: ShaderHandle = acast 0

#[Swag.Strict]
alias ShaderParamHandle = int
const InvalidShaderParamHandle: ShaderParamHandle = acast -1

struct ShaderBase
{
    handle:         ShaderHandle
    mvp:            ShaderParamHandle
    mdl:            ShaderParamHandle
    boundRect:      ShaderParamHandle
    textureRect:    ShaderParamHandle
    uvMode:         ShaderParamHandle
    copyMode:       ShaderParamHandle
    textureW:       ShaderParamHandle
    textureH:       ShaderParamHandle
}

struct ShaderAA
{
    using base: ShaderBase
}

struct ShaderBlur
{
    using base: ShaderBase
    radius:     ShaderParamHandle
    axis:       ShaderParamHandle
}