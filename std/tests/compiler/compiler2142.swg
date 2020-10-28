// A simple point with 2 float coordinates X and Y
public struct(T) Const
{
    const Zero: T = 0
    const One: T = Zero + 1
}

public #bake ConstF = Const'f32

#test
{
    @assert(ConstF.Zero == 0)
    @assert(ConstF.One == 1)
    @assert(@typeof(ConstF.Zero) == f32)
}