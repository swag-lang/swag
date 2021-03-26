#global testerror

impl PointBase
{
    const Zero: Self'T{1, 666}
}

struct(T) PointBase
{
    x: T
    y: T
}

alias Point = PointBase'f32
