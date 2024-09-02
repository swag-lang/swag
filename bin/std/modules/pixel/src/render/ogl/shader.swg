#global public
using Core, Ogl, Core.Math

#[Swag.Strict]
alias ShaderHandle = u64
const InvalidShaderHandle: ShaderHandle = cast() 0

#[Swag.Strict]
alias ShaderParamHandle = s64
const InvalidShaderParamHandle: ShaderParamHandle = cast() -1

struct ShaderBase
{
    handle:          ShaderHandle
    mvp:             ShaderParamHandle
    mdl:             ShaderParamHandle
    boundRect:       ShaderParamHandle
    textureRect:     ShaderParamHandle
    uvMode:          ShaderParamHandle
    copyMode:        ShaderParamHandle
    textureW:        ShaderParamHandle
    textureH:        ShaderParamHandle
}

struct ShaderSimple
{
    using base: ShaderBase
}

struct ShaderAA
{
    using base: ShaderBase
}

struct ShaderFont
{
    using base: ShaderBase
}

struct ShaderBlur
{
    using base:     ShaderBase
    radius:         ShaderParamHandle
    axis:           ShaderParamHandle
}
